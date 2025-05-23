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
#include <QElapsedTimer>
#include <gst/app/gstappsink.h>
#include <QImage>



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

// gboolean VideoReceiver::bus_call(GstBus * /*bus*/, GstMessage *msg, gpointer data) {
//     auto *self = static_cast<VideoReceiver*>(data);
//     switch (GST_MESSAGE_TYPE(msg)) {
//     case GST_MESSAGE_STATE_CHANGED: {
//         if (GST_MESSAGE_SRC(msg) == GST_OBJECT(self->pipeline)) {
//             GstState oldS, newS, pend;
//             gst_message_parse_state_changed(msg, &oldS, &newS, &pend);
//             if (newS == GST_STATE_PLAYING)
//                 emit self->cameraStarted();
//         }
//         break;
//     }
//     case GST_MESSAGE_ERROR: {
//         GError *err = nullptr;
//         gchar  *dbg = nullptr;
//         gst_message_parse_error(msg, &err, &dbg);
//         QString txt = QString::fromUtf8(err->message);
//         qDebug() << "[VideoReceiver] ERROR:" << txt;
//         emit self->cameraError(txt);
//         gst_element_set_state(self->pipeline, GST_STATE_READY);
//         gst_element_set_state(self->pipeline, GST_STATE_PLAYING);
//         g_error_free(err);
//         g_free(dbg);
//         break;
//     }
//     case GST_MESSAGE_EOS:
//         qDebug() << "[VideoReceiver] End of stream";
//         emit self->cameraError("End of stream");
//         gst_element_set_state(self->pipeline, GST_STATE_READY);
//         gst_element_set_state(self->pipeline, GST_STATE_PLAYING);
//         break;
//     default:
//         break;
//     }
//     return TRUE;
// }


gboolean VideoReceiver::bus_call(GstBus* /*bus*/, GstMessage* msg, gpointer data) {
    auto *self = static_cast<VideoReceiver*>(data);
    static bool wasPlaying = false;               // remember last known state
    static QElapsedTimer errorTimer;              // throttle restarts
    if (!errorTimer.isValid()) errorTimer.start();

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_STATE_CHANGED: {
        GstState oldS, newS, pend;
        gst_message_parse_state_changed(msg, &oldS, &newS, &pend);
        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(self->pipeline)) {
            bool nowPlaying = (newS == GST_STATE_PLAYING);
            if (nowPlaying != wasPlaying) {
                wasPlaying = nowPlaying;
                if (nowPlaying)
                    emit self->cameraStarted();
                else
                    emit self->cameraError("Stream stopped");
            }
        }
        break;
    }
    case GST_MESSAGE_ERROR: {
        if (errorTimer.elapsed() > 2000) {        // at most once every 2 s
            GError *err = nullptr;
            gchar  *dbg = nullptr;
            gst_message_parse_error(msg, &err, &dbg);
            QString what = QString::fromUtf8(err->message);
            g_error_free(err);
            g_free(dbg);

            emit self->cameraError(what);
            // try to recover
            gst_element_set_state(self->pipeline, GST_STATE_READY);
            gst_element_set_state(self->pipeline, GST_STATE_PLAYING);
            errorTimer.restart();
        }
        break;
    }
    case GST_MESSAGE_EOS: {
        if (errorTimer.elapsed() > 2000) {
            emit self->cameraError("End of stream");
            gst_element_set_state(self->pipeline, GST_STATE_READY);
            gst_element_set_state(self->pipeline, GST_STATE_PLAYING);
            errorTimer.restart();
        }
        break;
    }
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

    //Using PC camera for testing purposes:
    // temporarily use the laptop camera:
    // const char *webcamUri = "v4l2:///dev/video0";
    // g_object_set(pipeline,
    //              "uri",       webcamUri,
    //              "video-sink", videosink,
    //              nullptr);

    // 4) Watch the bus for EOS / errors / state changes
    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, VideoReceiver::bus_call, this);
    gst_object_unref(bus);

    // 5) Fire it up
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    qDebug() << "[VideoReceiver] playbin → PLAYING";
}



// VideoReceiver::VideoReceiver(QObject *parent)
//     : QObject(parent),
//     pipeline(nullptr),
//     videosink(nullptr),
//     appsink(nullptr)
// {
//     gst_init(nullptr, nullptr);

//     // 1) Create all elements
//     auto* src      = gst_element_factory_make("rtspsrc",      "src");
//     auto* depay    = gst_element_factory_make("rtph264depay", "depay");
//     auto* parser   = gst_element_factory_make("h264parse",    "parser");
//     auto* decoder  = gst_element_factory_make("avdec_h264",   "decoder");
//     auto* tee      = gst_element_factory_make("tee",          "tee");
//     auto* q1       = gst_element_factory_make("queue",        "q1");
//     auto* q2       = gst_element_factory_make("queue",        "q2");
//     auto* convert1 = gst_element_factory_make("videoconvert","convert1");
//     videosink      = gst_element_factory_make("xvimagesink",  "videosink");
//     auto* convert2 = gst_element_factory_make("videoconvert","convert2");
//     appsink        = gst_element_factory_make("appsink",       "appsink");

//     if (!src||!depay||!parser||!decoder||!tee||!q1||!q2
//         ||!convert1||!videosink||!convert2||!appsink)
//     {
//         qCritical() << "[VideoReceiver] failed to create elements";
//         return;
//     }

//     // ── **HERE** ──
//     // 2) Configure RTSP source with low‐latency
//     QString uri = getRtspUriFromConfig().trimmed();
//     g_object_set(src,
//                  "location", uri.toUtf8().constData(),
//                  "latency",   (guint)100,    // 100 ms jitter buffer
//                  nullptr);

//     // appsink wants raw RGB
//     GstCaps* caps = gst_caps_new_simple("video/x-raw",
//                                         "format", G_TYPE_STRING, "RGB",
//                                         nullptr);
//     g_object_set(appsink,
//                  "caps",         caps,
//                  "emit-signals", FALSE,
//                  "sync",         FALSE,
//                  nullptr);
//     gst_caps_unref(caps);

//     // 3) Build pipeline
//     pipeline = gst_pipeline_new("video-receiver");
//     gst_bin_add_many(GST_BIN(pipeline),
//                      src,
//                      depay, parser, decoder,
//                      tee,
//                      q1, convert1, videosink,
//                      q2, convert2, appsink,
//                      nullptr);

//     gst_element_link_many(depay, parser, decoder, tee, nullptr);
//     gst_element_link_many(tee, q1, convert1, videosink, nullptr);
//     gst_element_link_many(tee, q2, convert2, appsink,     nullptr);

//     // 4) Dynamic pad from rtspsrc → depay
//     g_signal_connect(src, "pad-added",
//                      G_CALLBACK(+[](
//                                      GstElement* /*rtspsrc*/,
//                                      GstPad*      newPad,
//                                      gpointer     data)
//                                 {
//                                     GstPad* sink = gst_element_get_static_pad(
//                                         static_cast<GstElement*>(data),
//                                         "sink");
//                                     if (!GST_PAD_IS_LINKED(sink))
//                                         gst_pad_link(newPad, sink);
//                                     gst_object_unref(sink);
//                                 }),
//                      depay);

//     // 5) Bus watch, start playing, etc.…
//     GstBus* bus = gst_element_get_bus(pipeline);
//     gst_bus_add_watch(bus, VideoReceiver::bus_call, this);
//     gst_object_unref(bus);

//     gst_element_set_state(pipeline, GST_STATE_PLAYING);
//     qDebug() << "[VideoReceiver] Pipeline set to PLAYING";
// }


void VideoReceiver::setWindowId(WId id) {
    if (videosink) {
        gst_video_overlay_set_window_handle(
            GST_VIDEO_OVERLAY(videosink),
            (guintptr)id
            );
    }
}


VideoReceiver::~VideoReceiver() {
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
    }
}

void VideoReceiver::start() {
    if (pipeline) {
        // If already playing, this is a no-op
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        qDebug() << "[VideoReceiver] Pipeline set back to PLAYING";
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

QImage VideoReceiver::grabFrame()
{
    if (!appsink) return {};

    // Pull the most recent sample (non-blocking)
    GstSample *sample = gst_app_sink_try_pull_sample(GST_APP_SINK(appsink), 0);
    if (!sample) return {};

    GstBuffer *buf = gst_sample_get_buffer(sample);
    GstMapInfo info;
    if (!gst_buffer_map(buf, &info, GST_MAP_READ)) {
        gst_sample_unref(sample);
        return {};
    }

    GstCaps *caps = gst_sample_get_caps(sample);
    auto *s      = gst_caps_get_structure(caps, 0);
    int width, height;
    gst_structure_get_int(s, "width",  &width);
    gst_structure_get_int(s, "height", &height);

    // appsink caps forced us to RGB
    QImage img((uchar*)info.data, width, height, QImage::Format_RGB888);

    // copy before we unmap/unref
    QImage copy = img.copy();

    gst_buffer_unmap(buf, &info);
    gst_sample_unref(sample);
    return copy;
}
