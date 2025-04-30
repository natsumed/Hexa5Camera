// VideoReceiver.cpp
#include "VideoReceiver.h"
#include <QDebug>
#include <QMetaObject>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <gst/video/videooverlay.h>


// --- pad-added handler -----------------------------------------------------
static void on_pad_added(GstElement *decodebin,
                         GstPad     *newPad,
                         gpointer    user_data)
{
    auto *convert = static_cast<GstElement*>(user_data);
    GstPad *sinkPad = gst_element_get_static_pad(convert, "sink");
    if (gst_pad_is_linked(sinkPad)) {
        gst_object_unref(sinkPad);
        return;
    }

    // check caps
    GstCaps *caps = gst_pad_get_current_caps(newPad);
    GstStructure *str = gst_caps_get_structure(caps, 0);
    const char *name = gst_structure_get_name(str);
    qDebug() << "[VideoReceiver] pad-added type:" << name;

    if (g_str_has_prefix(name, "video/")) {
        if (gst_pad_link(newPad, sinkPad) != GST_PAD_LINK_OK) {
            qDebug() << "[VideoReceiver] Failed to link decodebin → convert";
        } else {
            qDebug() << "[VideoReceiver] Linked decodebin → convert OK";
        }
    }

    gst_caps_unref(caps);
    gst_object_unref(sinkPad);
}

gboolean VideoReceiver::bus_call(GstBus * /*bus*/, GstMessage *msg, gpointer data) {
    auto *self = static_cast<VideoReceiver*>(data);
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_STATE_CHANGED: {
        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(self->pipeline)) {
            GstState oldS, newS, pend;
            gst_message_parse_state_changed(msg, &oldS, &newS, &pend);
            if (newS == GST_STATE_PLAYING)
                emit self->cameraStarted();
        }
        break;
    }
    case GST_MESSAGE_ERROR: {
        GError *err = nullptr;
        gchar  *dbg = nullptr;
        gst_message_parse_error(msg, &err, &dbg);
        QString txt = QString::fromUtf8(err->message);
        qDebug() << "[VideoReceiver] ERROR:" << txt;
        emit self->cameraError(txt);
        gst_element_set_state(self->pipeline, GST_STATE_READY);
        gst_element_set_state(self->pipeline, GST_STATE_PLAYING);
        g_error_free(err);
        g_free(dbg);
        break;
    }
    case GST_MESSAGE_EOS:
        qDebug() << "[VideoReceiver] End of stream";
        emit self->cameraError("End of stream");
        gst_element_set_state(self->pipeline, GST_STATE_READY);
        gst_element_set_state(self->pipeline, GST_STATE_PLAYING);
        break;
    default:
        break;
    }
    return TRUE;
}

VideoReceiver::VideoReceiver(QObject *parent)
    : QObject(parent),
    pipeline(nullptr),
    videosink(nullptr)
{
    gst_init(nullptr, nullptr);

    QString uri = getRtspUriFromConfig();
    qDebug() << "[VideoReceiver] Using RTSP URI:" << uri;

    // 1) Create playbin
    pipeline  = gst_element_factory_make("playbin", "player");
    if (!pipeline) {
        qCritical() << "[VideoReceiver] Failed to create playbin";
        return;
    }

    GstElement* convert = gst_element_factory_make("videoconvert",  "convert");

    // 2) Create the sink and tell playbin to use it
    videosink = gst_element_factory_make("xvimagesink", "videosink");
    if (!videosink) {
        qCritical() << "[VideoReceiver] Failed to create xvimagesink";
        gst_object_unref(pipeline);
        pipeline = nullptr;
        return;
    }
    // disable internal clock sync so frames show immediately
    g_object_set(videosink, "sync", FALSE, nullptr);

    // 3) Configure playbin: set our RTSP URI, low latency, and video sink
    g_object_set(pipeline,
                 "uri",         uri.toUtf8().constData(),
                 "latency",     100,            // 100 ms jitter buffer
                 "video-sink",  videosink,
                 nullptr);

    // 4) Watch the bus for EOS / errors / state changes
    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, VideoReceiver::bus_call, this);
    gst_object_unref(bus);

    // 5) Fire it up
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    qDebug() << "[VideoReceiver] playbin → PLAYING";
}

void VideoReceiver::setWindowId(WId id) {
    if (videosink) {
        gst_video_overlay_set_window_handle(
            GST_VIDEO_OVERLAY(videosink),
            id
            );
    }
}

VideoReceiver::~VideoReceiver() {
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
    }
}


void VideoReceiver::stop() {
    if (pipeline)
        gst_element_set_state(pipeline, GST_STATE_NULL);
}

// --- read URI from JSON config (unchanged) --------------------------------
QString VideoReceiver::getRtspUriFromConfig() {
    // Get the standard config location (typically ~/.config)
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

    // Define a subfolder name for our camera configuration
    QString subFolder = "Haxa5Camera";

    // Create a QDir object for the config directory
    QDir configDirectory(configDir);
    // Create the subfolder if it doesn't exist
    if (!configDirectory.exists(subFolder)) {
        if (!configDirectory.mkdir(subFolder)) {
            qWarning() << "Failed to create subfolder:" << subFolder;
        }
    }

    // Build the full path for the config file in the subfolder
    QString configFile = configDirectory.filePath(subFolder + "/Hexa5CameraConfig.json");

    // Default values
    QString defaultIP = "10.14.11.3";
    int defaultPort = 8554;
    QString defaultPath = "/main.264";
    QString defaultUri = QString("rtsp://%1:%2%3\n").arg(defaultIP).arg(defaultPort).arg(defaultPath);

    QFile file(configFile);
    if (!file.exists()) {
        // If the config file does not exist, create it with default values.
        QJsonObject obj;
        obj["ip"] = defaultIP;
        obj["port"] = defaultPort;
        obj["path"] = defaultPath;
        QJsonDocument doc(obj);

        // Ensure the subfolder exists (already attempted above)
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson(QJsonDocument::Indented));
            file.close();
            qDebug() << "Created config file with default RTSP URI at" << configFile;
        } else {
            qWarning() << "Failed to create config file at" << configFile << "; using default RTSP URI.";
        }
        return defaultUri;
    } else {
        // Read and parse the config file.
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            file.close();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                QString ip = obj.value("ip").toString(defaultIP);
                int port = obj.value("port").toInt(defaultPort);
                QString path = obj.value("path").toString(defaultPath);
                return QString("rtsp://%1:%2%3").arg(ip).arg(port).arg(path);
            } else {
                qWarning() << "Config file is not a valid JSON object; using default RTSP URI.";
                return defaultUri;
            }
        } else {
            qWarning() << "Failed to open config file; using default RTSP URI.";
            return defaultUri;
        }
    }
}

bool VideoReceiver::isPlaying() const {
    if (!pipeline) return false;
    GstState cur, pending;
    // zero timeout means “just query”
    if (gst_element_get_state(pipeline, &cur, &pending, 0)
        == GST_STATE_CHANGE_SUCCESS) {
        return cur == GST_STATE_PLAYING;
    }
    return false;
}
