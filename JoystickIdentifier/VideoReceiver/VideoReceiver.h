#ifndef VIDEORECEIVER_H
#define VIDEORECEIVER_H

#include <QObject>
#include <QWidget>  // Include this header to define WId
#include <gst/gst.h>

class VideoReceiver : public QObject {
    Q_OBJECT

public:
    explicit VideoReceiver(QObject *parent = nullptr);
    ~VideoReceiver();

    void setWindowId(WId id);
    QString getRtspUriFromConfig();
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);
    void stop(); 

private:
    GstElement *pipeline;
    GstElement *videosink;


};

#endif // VIDEORECEIVER_H
