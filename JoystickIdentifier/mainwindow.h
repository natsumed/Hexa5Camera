#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>
#include <QTimer>
#include <QTime>
#include <QLabel>
#include <QListWidgetItem>
#include <memory>
#include <thread>
#include "thirdparty/SIYI-SDK/src/sdk.h"
#include <QCloseEvent>
#include <QEvent>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <mutex>
#include <QElapsedTimer>
#include <gst/gst.h>
#include <QProcess>

namespace Ui {
class MainWindow;
}

// mainwindow.h, at the top with the other includes
class VideoRecorderWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void updateDeviceList();
    void pollAxisValues();
    void updateAxisValues(int js, int axis, qreal value);
    void updateButtonState(int js, int button, bool pressed);
    void onJoystickItemClicked(QListWidgetItem *item);
    void onSwitchToKeyboard();
    void onSwitchToJoystick();
    void sendGimbalCommands();
    void saveConfig();
    void saveDefaultConfig();
    void handleCommandFeedback(const QString& commandId, bool success);
    void onCameraStarted();
    void onCameraError(const QString &msg);
    void refreshCameraStatus();
    void onJoystickAxisChanged(int device, int axis, qreal value);
    void on_RecordButton_clicked();
    void updateRecordTime();
    void onRecordingFinished(int exitCode, QProcess::ExitStatus status);
    void on_ScreenshotButton_clicked();
    void onFullUp();
    void onFullDown();
    void onFullLeft();
    void onFullRight();
    void onZoomMaxIn();
    void onZoomMaxOut();
    void onStop();

private:
    Ui::MainWindow *ui;

    // Mode and joystick selection:
    enum class InputMode { None, Keyboard, Joystick };
    InputMode inputMode = InputMode::None;
    int cameraJoystickIndex;

    // Gimbal control shared state:
    //int currentYawSpeed;
    //int currentPitchSpeed;
    float currentZoom;

    // Constants for control increments:
    static constexpr int YAW_SPEED_CONSTANT = 50;
    static constexpr int PITCH_SPEED_CONSTANT = 50;
    static constexpr float ZOOM_STEP_CONSTANT = 1.0f;
    const int MAX_SPEED = 50;
    const int MOVE_SPEED = 50; 
    const float ACCELERATION_STEP = 0.5f;
    float currentYawAccel = 0.0f;
    float currentPitchAccel = 0.0f;
    QElapsedTimer keyHoldTimer;

    QMutex commandMutex;
    QTimer *commandTimer;

    // Pointer to the SIYI SDK instance
    SIYI_SDK* sdk;

    std::thread receiveThread;
    std::atomic<bool> keepRunning;
    std::atomic<int> currentYawSpeed{0};
    std::atomic<int> currentPitchSpeed{0};
    QMutex shutdownMutex;
    bool isShuttingDown = false;
    VideoRecorderWidget *videoWidget = nullptr;

    static constexpr float MIN_ZOOM = 1.0f;
    static constexpr float MAX_ZOOM = 30.0f;
    static constexpr float ZOOM_SPEED = 1.0f;
    int lastZoomAxis = 0;
    //const int   ZOOM_LEVELS = 5;
    int ZOOM_LEVELS = int((MAX_ZOOM - MIN_ZOOM) / ZOOM_STEP_CONSTANT + 0.5f);
    int lastZoomSign = 0;
    int lastZoomIndex = -1;
    int lastZoomLevel = -1;
    GstElement *recordPipeline = nullptr;
    QString     rtspUri;

    // For the on‐screen timer:
    QProcess         *recProcess    = nullptr;
    QElapsedTimer     recordTimer;
    //QTimer           *recordUiTimer = nullptr;
    //QLabel           *recordOverlay = nullptr;
    bool              isRecording   = false;
    QString           currentRecordPath;


    // in MainWindow private:
    enum class RecordState { Idle, Recording };
    RecordState recordState{RecordState::Idle};
    QProcess*   recordProcess = nullptr;
    QElapsedTimer       recordClock;
    QTimer*     recordUiTimer = nullptr;
    QLabel*     recordOverlay = nullptr;
    QString     lastRecordPath;
    bool useLocalCamera = false;
    QString loadControlIp() const;

};

#endif // MAINWINDOW_H

