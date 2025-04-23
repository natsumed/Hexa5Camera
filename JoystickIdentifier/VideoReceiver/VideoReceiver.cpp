#include "VideoReceiver.h"
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <QDebug>
#include <QtCore>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

// Temporarily undefine the "signals" macro to avoid conflicts.
#ifdef signals
#  undef signals
#  define RESTORE_SIGNALS_AFTER_GST_RTSP
#endif

#include <gst/rtsp/gstrtspconnection.h>

#ifdef RESTORE_SIGNALS_AFTER_GST_RTSP
#  define signals Q_SIGNALS
#  undef RESTORE_SIGNALS_AFTER_GST_RTSP
#endif


// Forward declaration for the pad-added callback.
static void on_pad_added(GstElement *src, GstPad *new_pad, gpointer data);

gboolean VideoReceiver::bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
    Q_UNUSED(bus);
    VideoReceiver *self = static_cast<VideoReceiver*>(data);
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        qDebug() << "End of stream";
        break;
    case GST_MESSAGE_ERROR: {
        gchar *debug;
        GError *error;
        gst_message_parse_error(msg, &error, &debug);
        qDebug() << "Error:" << error->message;
        g_free(debug);
        g_error_free(error);
        break;
    }
    default:
        break;
    }
    return TRUE;
}

VideoReceiver::VideoReceiver(QObject *parent) : QObject(parent) {
    gst_init(nullptr, nullptr);

    QString rtspUri = getRtspUriFromConfig();
    qDebug() << "Using RTSP URI:" << rtspUri;

    // 1) Create pipeline and elements
    pipeline       = gst_pipeline_new("video-receiver");
    GstElement *src = gst_element_factory_make("uridecodebin", "source");
    GstElement *convert = gst_element_factory_make("videoconvert", "convert");
    videosink      = gst_element_factory_make("xvimagesink", "videosink");  
    // xvimagesink supports GstVideoOverlay

    if (!pipeline || !src || !convert || !videosink) {
        qDebug() << "Failed to create one or more GStreamer elements.";
        return;
    }

    // 2) Configure uridecodebin
    g_object_set(src, "uri", rtspUri.toStdString().c_str(), nullptr);
    g_object_set(src, "timeout", (guint)30000000, nullptr);

    // 3) Build pipeline: add & link the static parts
    gst_bin_add_many(GST_BIN(pipeline), src, convert, videosink, nullptr);
    if (!gst_element_link(convert, videosink)) {
        qDebug() << "Could not link videoconvert → xvimagesink";
        return;
    }

    // 4) Dynamic pad: when uridecodebin has decoded video, link it into videoconvert
    g_signal_connect(src, "pad-added", G_CALLBACK(+[](
        GstElement* /*decodebin*/, GstPad* newPad, gpointer user_data) {
        
        GstElement *convert = static_cast<GstElement*>(user_data);
        GstPad *sinkPad = gst_element_get_static_pad(convert, "sink");
        if (gst_pad_is_linked(sinkPad)) {
            gst_object_unref(sinkPad);
            return;
        }

        GstCaps *caps = gst_pad_query_caps(newPad, nullptr);
        GstStructure *str = gst_caps_get_structure(caps, 0);
        const char *name = gst_structure_get_name(str);
        qDebug() << "uridecodebin new pad type:" << name;

        // only link video streams
        if (g_str_has_prefix(name, "video/")) {
            if (gst_pad_link(newPad, sinkPad) != GST_PAD_LINK_OK)
                qDebug() << "Failed to link decodebin → videoconvert";
            else
                qDebug() << "Linked decodebin → videoconvert OK";
        }
        gst_caps_unref(caps);
        gst_object_unref(sinkPad);
    }), convert);

    // 5) Set up bus watch
    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, VideoReceiver::bus_call, this);
    gst_object_unref(bus);

    // 6) Start playing
    qDebug() << "Setting pipeline to PLAYING";
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    qDebug() << "Pipeline state set";
}




VideoReceiver::~VideoReceiver() {
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

void VideoReceiver::setWindowId(WId id) {
    if (videosink) {
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(videosink), id);
    }
}

// The pad-added callback to link the dynamic pad from rtspsrc to rtph264depay.
static void on_pad_added(GstElement *src, GstPad *new_pad, gpointer data) {
    GstElement *rtph264depay = static_cast<GstElement*>(data);
    GstPad *sink_pad = gst_element_get_static_pad(rtph264depay, "sink");
    if (!sink_pad) {
        qDebug() << "Failed to get static pad for rtph264depay.";
        return;
    }
    if (gst_pad_is_linked(sink_pad)) {
        gst_object_unref(sink_pad);
        return;
    }

    // <-- use query_caps here -->
    GstCaps *new_pad_caps = gst_pad_query_caps(new_pad, nullptr);
    GstStructure *new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    const gchar *new_pad_type = gst_structure_get_name(new_pad_struct);
    qDebug() << "New pad type:" << new_pad_type;

    if (g_str_has_prefix(new_pad_type, "application/x-rtp")) {
        if (gst_pad_can_link(new_pad, sink_pad)) {
            GstPadLinkReturn ret = gst_pad_link(new_pad, sink_pad);
            if (ret == GST_PAD_LINK_OK) {
                qDebug() << "Pad linked successfully.";
            } else {
                qDebug() << "Link failed, code:" << ret;
            }
        } else {
            qDebug() << "Pads cannot be linked.";
        }
    } else {
        qDebug() << "Unsupported pad type:" << new_pad_type;
    }

    gst_caps_unref(new_pad_caps);
    gst_object_unref(sink_pad);
}



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


void VideoReceiver::stop() {
  if (pipeline) {
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    pipeline = nullptr;
  }
}

