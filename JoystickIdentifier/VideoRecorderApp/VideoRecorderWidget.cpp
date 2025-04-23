#include "VideoRecorderWidget.h"
#include <QVBoxLayout>
#include <QDebug>

VideoRecorderWidget::VideoRecorderWidget(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    // Create the VideoReceiver object.
    receiver = new VideoReceiver(this);
    // (Optionally, add a placeholder widget or set a background color here.)
    setLayout(layout);
    qDebug() << "VideoRecorderWidget initialized";
}

VideoRecorderWidget::~VideoRecorderWidget() {
    delete receiver;
}

void VideoRecorderWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    setFocus(); 
    if (receiver) {
        // This sets the GStreamer videosink window to display inside this widget.
        receiver->setWindowId(winId());
        qDebug() << "VideoReceiver window handle set to" << winId();
    }
}

void VideoRecorderWidget::stopPipeline() {
  if (receiver) {
    receiver->stop();
  }
}

