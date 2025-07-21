#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QTime>
#ifdef SDL_SUPPORTED
#include <SDL.h>
#endif
#include "QJoysticks.h"
#include "VideoRecorderWidget.h"
#include <QKeyEvent>
#include <QDockWidget>
#include <QListWidget>
#include <QProgressBar>
#include <QLabel>
#include <QMutexLocker>
#include <thread>
#include <QKeyEvent>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QListWidgetItem>
#include <QApplication>
#include <QShowEvent>
#include "thirdparty/SIYI-SDK/src/sdk.h"
#include <csignal>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <QMessageBox>
#include <QProcess>
#include <gst/video/videooverlay.h>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QtConcurrent/QtConcurrent>

//#include "servo_client.hpp"

//static const char *CONTROL_IP = "10.14.11.3";
static const int CONTROL_PORT = 37260;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      keepRunning(true),
      sdk(nullptr),
      currentZoom(1.0f)

{
    ui->setupUi(this);

    ui->toolButtonLeft->hide();
    ui->toolButtonRight->hide();

    ui->toggleButton->setFocusPolicy(Qt::NoFocus);

    // allow children to get focus again
    this->setFocusPolicy(Qt::StrongFocus);
    // and ensure our central widget can accept focus too
    ui->centralwidget->setFocusPolicy(Qt::StrongFocus);

    ui->lineEditIP  ->setFocusPolicy(Qt::StrongFocus);
    ui->lineEditPort->setFocusPolicy(Qt::StrongFocus);
    ui->lineEditPath->setFocusPolicy(Qt::StrongFocus);


    // ui->centralwidget->hide();
    // statusBar()->hide();

    // // --- 1) HIDE YOUR SIDE PANEL & TOGGLE BUTTON ---
    // ui->controlsContainer->hide();
    // ui->toggleButton->hide();

    // // (If you also hid your status‑bar during splash:)
    // statusBar()->hide();

    // // --- 2) INSTANTIATE THE SPLASH INSIDE VideoRecorderSection ---
    // splashVideo = new QVideoWidget(ui->centralwidget);
    // splashVideo->setGeometry(ui->centralwidget->rect());
    // splashVideo->setAspectRatioMode(Qt::IgnoreAspectRatio);
    // splashVideo->show();
    // splashVideo->raise();

    // // 2) hook the player as before
    // splashPlayer = new QMediaPlayer(this);
    // splashPlayer->setVideoOutput(splashVideo);
    // splashPlayer->setSource(QUrl("qrc:/intro.mp4"));
    // splashPlayer->play();

    // // --- 4) WHEN THE CLIP ENDS, TEAR DOWN & REVEAL ---
    // connect(splashPlayer, &QMediaPlayer::mediaStatusChanged,
    //         this, [this](QMediaPlayer::MediaStatus status){
    //             if (status == QMediaPlayer::EndOfMedia ||
    //                 splashPlayer->playbackState() != QMediaPlayer::PlayingState)
    //             {
    //                 // slight delay to let the last frame linger
    //                 QTimer::singleShot(100, this, [this]() {
    //                     splashVideo->hide();
    //                     //splashPlayer->stop();
    //                     splashPlayer->deleteLater();
    //                     splashVideo->deleteLater();

    //                     this->releaseKeyboard();
    //                     ui->centralwidget->show();

    //                     // now un‑hide your real UI:
    //                     ui->controlsContainer->show();
    //                     ui->toggleButton->show();
    //                     statusBar()->show();
    //                 });
    //             }
    //         });


    //this->centralWidget()->setStyleSheet("background-color: magenta;");
    // Apply stylesheet to central widget to inherit to children
    // QFile ff(":/hexa5.css");
    // if (ff.open(QFile::ReadOnly | QFile::Text)) {
    //     QString css = QString::fromUtf8(ff.readAll());
    //     centralWidget()->setStyleSheet(css);
    // }

    QStatusBar* statusBarr = new QStatusBar();
    statusBarr->setStyleSheet("background-color: #2d2d44; color: #aaaaaa;");
    setStatusBar(statusBarr);

    // // Add status indicators
    // QLabel* connectionStatus = new QLabel("Camera: Disconnected");
    // connectionStatus->setProperty("status", "inactive");
    // statusBarr->addWidget(connectionStatus);

    // QLabel* recStatus = new QLabel("Recording: Inactive");
    // recStatus->setProperty("status", "inactive");
    // statusBarr->addPermanentWidget(recStatus);



    /////////////////////////////////////////////////////
    /// \brief setWindowTitle
    /// Main styling
    setWindowTitle("Hexa5Camera");
    // if you have an application icon
    setWindowIcon(QIcon(":/hexa5.png"));


    QFont f = font();
    f.setPointSize(10);
    setFont(f);

    // Add some internal margins around your main layout
    // ui->centralwidget->layout()->setContentsMargins(8,8,8,8);
    // ui->centralwidget->layout()->setSpacing(6);

    ui->VideoRecorderSection->setFlat(true);


    /////////////////////////////////////////////////////

    /////////////////////////////////////////////////////
    /// controlsContainer Creation

    // 1) Create a horizontal layout to replace the absolute geometry
    auto *hl = new QHBoxLayout(ui->centralwidget);
    hl->setContentsMargins(0,0,0,0);
    hl->setSpacing(0);
    hl->addWidget(ui->VideoRecorderSection, 1);
    hl->addWidget(ui->toggleButton,         0);
    hl->addWidget(ui->controlsContainer,    0);

    // 2) completely kill any padding inside controlsContainer itself
    if (auto *inner = qobject_cast<QBoxLayout*>(ui->controlsContainer->layout())) {
        inner->setContentsMargins(0,0,0,0);
        inner->setSpacing(0);
    }

    // 3) make the container truly Fixed‐width so 0px is honored
    ui->controlsContainer->setSizePolicy(
        QSizePolicy::Fixed,
        ui->controlsContainer->sizePolicy().verticalPolicy()
        );
    ui->controlsContainer->setMinimumWidth(0);
    ui->controlsContainer->setMaximumWidth(0);
    ui->controlsContainer->hide();  // start fully hidden

    // 4) measure its “full” width
    int fullW = ui->controlsContainer->sizeHint().width();
    if (fullW < 10) fullW = 320; // fallback

    // 5) build a single animation on its maximumWidth
    auto *anim = new QPropertyAnimation(ui->controlsContainer, "maximumWidth", this);
    anim->setDuration(250);
    anim->setStartValue(0);
    anim->setEndValue(fullW);

    // 6) wire up the toggle button
    ui->toggleButton->setArrowType(Qt::RightArrow);
    connect(anim, &QPropertyAnimation::finished, this, [this, anim]() {
        auto *ctr  = ui->controlsContainer;
        auto *lay   = ui->centralwidget->layout();

        if (anim->direction() == QAbstractAnimation::Backward) {
            // fully collapse
            ctr->hide();
            ctr->setMaximumWidth(0);
            ctr->setFixedWidth(0);
        } else {
            // ensure it’s back to its full size
            int fullW = anim->endValue().toInt();
            ctr->show();
            ctr->setMaximumWidth(fullW);
            ctr->setFixedWidth(fullW);
            ui->lineEditIP->setFocus();
        }

        // Force the parent layout to re‐do its math
        if (lay) {
            lay->invalidate();
            lay->activate();
        }
        ui->centralwidget->updateGeometry();
    });


    // 7) once a “close” animation finishes, hide it completely
    connect(ui->toggleButton, &QToolButton::clicked, this, [this, anim]() {
        bool closed = (ui->controlsContainer->maximumWidth() == 0);
        if (closed) {
            ui->controlsContainer->show();          // ← bring it back into view
            anim->setDirection(QAbstractAnimation::Forward);
            ui->toggleButton->setArrowType(Qt::LeftArrow);
        } else {
            anim->setDirection(QAbstractAnimation::Backward);
            ui->toggleButton->setArrowType(Qt::RightArrow);
        }
        anim->start();
    });




    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    /// Style
    ui->tabWidget->setStyleSheet(R"(
  /* overall pane */
  QTabWidget::pane {
    border: 1px solid #4A4A4A;
    background: #2B2B2B;
    top: -1px;              /* overlap tabs’ bottom border */
  }
  /* the tabs */
  QTabBar::tab {
    background: #3C3F41;
    color: #A9B7C6;
    padding: 6px 12px;
    margin-right: 2px;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
    min-width: 80px;
  }
  QTabBar::tab:selected {
    background: #4E5254;
    color: #FFF;
  }
  QTabBar::tab:hover {
    background: #505354;
  }
  /* remove focus outline */
  QTabBar::tab:focus { outline: none; }
)");


    qApp->setStyleSheet(R"(
  QPushButton {
    background: #3C3F41;
    border: 1px solid #5A5A5A;
    padding: 6px 12px;
    color: #DDD;
    border-radius: 3px;
  }
  QPushButton:hover {
    background: #505354;
  }
  QPushButton:pressed {
    background: #2A2D2F;
  }
)");

    qApp->setStyleSheet(R"(
QDockWidget {
  background: #2b2b2b;
  titlebar-close-icon: none;  /* just in case */
}

/* style its title bar */
QDockWidget::title {
  text-align: left;
  padding: 4px 8px;
  background: qlineargradient(
      x1:0, y1:0, x2:0, y2:1,
      stop:0 #393939, stop:1 #2b2b2b
  );
  color: #ffffff;
  font-weight: bold;
  border-bottom: 1px solid #444444;
}

/* when floating, give it a thin border */
QDockWidget[floating="true"] {
  border: 1px solid #555555;
}

)");



    // Joystick button: switch mode *and* select tab 0
    connect(ui->switchtojoystick, &QPushButton::clicked, this, [this]() {
        onSwitchToJoystick();                // existing logic to flip into JOYSTICK mode
        ui->tabWidget->setCurrentIndex(0);   // show the “Joystick” tab
    });

    // Keyboard button: switch mode *and* select tab 1
    connect(ui->switchtokeyboard, &QPushButton::clicked, this, [this]() {
        onSwitchToKeyboard();                // existing logic to flip into KEYBOARD mode
        ui->tabWidget->setCurrentIndex(1);   // show the “Keyboard” tab
    });

    // Camera-Config button: flip into CONFIG mode *and* select tab 2
    connect(ui->pushButtonConfiguration, &QPushButton::clicked, this, [this]() {
        // If you have a function for config-mode, call it here.
        // e.g. onSwitchToConfig();
        ui->tabWidget->setCurrentIndex(2);   // show the “Camera Configuration” tab
    });



    //QApplication::instance()->installEventFilter(this);
    connect(QJoysticks::getInstance(),
            &QJoysticks::axisChanged,
            this,
            &MainWindow::onJoystickAxisChanged);
    videoWidget = new VideoRecorderWidget(this);
    //videoWidget->installEventFilter(this);
    videoWidget->setFocusPolicy(Qt::NoFocus);
    videoWidget->getReceiver()->setWindowId(videoWidget->winId());
    QVBoxLayout *videoLayout = new QVBoxLayout();
    videoLayout->setContentsMargins(0, 0, 0, 0);
    videoLayout->addWidget(videoWidget);
    if (ui->VideoRecorderSection) {
        ui->VideoRecorderSection->setLayout(videoLayout);
    } else {
        #ifdef _DEBUG
        qDebug() << "VideoRecorderWidget not found in the Video Recorder section!";
        #endif
    }

    rtspUri = videoWidget->getReceiver()->getRtspUriFromConfig();

    // 1) give an initial “checking” state
    ui->lineEditCameraStatus->setText("Checking…");
    ui->lineEditCameraStatus->setStyleSheet(
        "background-color: lightgray; color: black;");

    // 2) get the receiver and connect
    auto *vr = videoWidget->getReceiver();
    vr->setWindowId(videoWidget->winId());
    connect(vr, &VideoReceiver::cameraStarted,
            this, &MainWindow::onCameraStarted);
    connect(vr, &VideoReceiver::cameraError,
            this, &MainWindow::onCameraError);

    QTimer *cameraPoll = new QTimer(this);
    connect(cameraPoll, &QTimer::timeout, this, &MainWindow::refreshCameraStatus);
    cameraPoll->start(5000);

    refreshCameraStatus();

    // Connect the Rescan button and joystick signals.
    connect(ui->Rescan, &QPushButton::clicked, this, &MainWindow::updateDeviceList);
    connect(QJoysticks::getInstance(), &QJoysticks::countChanged,
            this, &MainWindow::updateDeviceList);
    connect(QJoysticks::getInstance(), &QJoysticks::buttonChanged,
            this, &MainWindow::updateButtonState);
    connect(ui->listWidget, &QListWidget::itemClicked,
            this, &MainWindow::onJoystickItemClicked);

    // Mode switch buttons.
    connect(ui->switchtokeyboard, &QPushButton::clicked, this, &MainWindow::onSwitchToKeyboard);
    connect(ui->switchtojoystick, &QPushButton::clicked, this, &MainWindow::onSwitchToJoystick);

    // in MainWindow::MainWindow(...)
    connect(ui->toolButtonUp,    &QToolButton::clicked, this, &MainWindow::onFullUp);
    connect(ui->toolButtonDown,  &QToolButton::clicked, this, &MainWindow::onFullDown);
    connect(ui->toolButtonLeft,  &QToolButton::clicked, this, &MainWindow::onFullLeft);
    connect(ui->toolButtonRight, &QToolButton::clicked, this, &MainWindow::onFullRight);
    connect(ui->toolButtonStop, &QToolButton::clicked, this, &MainWindow::onStop);

    // and your zoom buttons:
    connect(ui->toolButtonZoomPlus,  &QToolButton::clicked, this, &MainWindow::onZoomMaxIn);
    connect(ui->toolButtonZoomMinus, &QToolButton::clicked, this, &MainWindow::onZoomMaxOut);


    connect(ui->pushButtonSaveConfig, &QPushButton::clicked, this, &MainWindow::saveConfig);
    connect(ui->DefaultConfig, &QPushButton::clicked, this, &MainWindow::saveDefaultConfig);

    // Set focus policy so that key events arrive at the main window.
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    updateDeviceList();
    statusBar()->showMessage("Ready");
    #ifdef _DEBUG
    qDebug() << "Detected Joysticks:" << QJoysticks::getInstance()->deviceNames();
    #endif
    for (int i = 0; i < QJoysticks::getInstance()->count(); i++) {
        #ifdef _DEBUG
        qDebug() << "Joystick" << i << "axis count:"
                 << QJoysticks::getInstance()->getNumAxes(i);
        #endif
    }

    QTimer *pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &MainWindow::pollAxisValues);
    pollTimer->start(50);

    // Set up a timer to send gimbal commands every 100ms.
    commandTimer = new QTimer(this);
    connect(commandTimer, &QTimer::timeout, this, &MainWindow::sendGimbalCommands);
    commandTimer->setInterval(50);
    commandTimer->start(10);

    // Create the SIYI SDK instance.

    std::string ip = "10.14.11.3";
    int port = 37260;
    sdk = new SIYI_SDK(ip.c_str(), 37260);
    if (sdk->request_firmware_version()) {
        qDebug() << "Requested firmware version. Waiting for response...";
        std::this_thread::sleep_for(std::chrono::seconds(2)); 
        auto [code_version, gimbal_version, zoom_version] = sdk->get_firmware_version();
        qDebug() << "Code Board: " << code_version.c_str() 
                 << "  Gimbal: " << gimbal_version.c_str()
                 << "  Zoom: " << zoom_version.c_str();
    } else {
        qDebug() << "Failed to request firmware version.";
    }
    if (sdk->request_gimbal_center()){
        qDebug() << "Requested gimbal center . Waiting for response...";
    }
    if(sdk->request_autofocus()){
        qDebug() << "Requested autofocus. Waiting for response...";
    }

    receiveThread = std::thread([this]() {
        bool keepRunningLocal = keepRunning.load();
        sdk->receive_message_loop(keepRunningLocal);
    });
    #ifdef _DEBUG
    qDebug() << "Camera control initialized";
    #endif

    //Recording Video Section
    useLocalCamera = true;
    // Recording overlay + timer
    recordOverlay = new QLabel(videoWidget);
    recordOverlay->setStyleSheet(R"(
  background-color: rgba(0,0,0,128);
  color: red;
  font: bold 16px;
)");
    recordOverlay->setAlignment(Qt::AlignCenter);
    recordOverlay->setFixedHeight(30);
    recordOverlay->setFixedWidth(videoWidget->width());
    recordOverlay->move(0,0);
    recordOverlay->hide();
    recordOverlay->raise();

    // Create the timer for updating the overlay clock
    recordUiTimer = new QTimer(this);
    recordUiTimer->setInterval(500);
    connect(recordUiTimer, &QTimer::timeout,
            this,         &MainWindow::updateRecordTime);

    // Button hookup
    // connect(ui->RecordButton, &QPushButton::clicked,
    //         this,            &MainWindow::on_RecordButton_clicked);
    recordState = RecordState::Idle;
    ui->RecordButton->setText("Start Recording");

    //Screenshot
    connect(ui->ScreenshotButton, &QPushButton::clicked,
            this,               &MainWindow::on_ScreenshotButton_clicked);


    // e.g. read it from your camera‐config QLineEdits, or just hard‑code
    QString servoIp   = "127.0.0.1";
    int     servoPort = 8000;

    // 1) instantiate
    _servo = std::make_unique<ServoControl::ServoClient>(
        servoIp.toStdString(),
        servoPort,
        /*timeout_ms=*/ 2000
        );

    // 2) try to connect
    if (!_servo->connect()) {
        statusBar()->showMessage(
            QString("Servo connect failed: %1")
                .arg(QString::fromStdString(_servo->getLastError())),
            5000
            );
    } else {
        statusBar()->showMessage("Servo connected", 2000);
    }

    // 1) take ownership of your existing client and make a worker
    auto client = std::move(_servo);
    auto* thread = new QThread(this);
    auto* worker = new ServoWorker(std::move(client));
    worker->moveToThread(thread);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    thread->start();

    // 2) expose a signal so we can tell the worker "new position!"
    connect(this, &MainWindow::servoPositionChanged,
            worker, &ServoWorker::setPosition,
            Qt::QueuedConnection);

    // 3) initialize value (if you like)
    emit servoPositionChanged(_servoPosition);
}


MainWindow::~MainWindow() {

    if (videoWidget && videoWidget->getReceiver())
        videoWidget->getReceiver()->stop();

    // 1. Stop command timer
    commandTimer->stop();
    
    // 2. Signal threads to stop
    keepRunning = false;
    
    // 3. Join threads (use . operator, not ->)
    if (receiveThread.joinable()) {  // CORRECTED
        receiveThread.join();
    }
    
    // 4. Delete SDK instance
    delete sdk;
}

void MainWindow::onJoystickItemClicked(QListWidgetItem *item) {
    int jsIndex = item->data(Qt::UserRole).toInt();
    cameraJoystickIndex = jsIndex;
    qDebug() << "Camera joystick set to index:" << cameraJoystickIndex;
}

void MainWindow::updateDeviceList() {
    ui->listWidget->clear();
    QStringList names = QJoysticks::getInstance()->deviceNames();
    if (names.isEmpty()) {
        ui->listWidget->addItem("No Joysticks Detected");
        statusBar()->showMessage("No joysticks detected", 3000);
        qDebug() << "No joysticks detected";
    } else {
        for (int i = 0; i < names.size(); ++i) {
            int axisCount = QJoysticks::getInstance()->getNumAxes(i);
            QListWidgetItem *item = new QListWidgetItem(names[i]);
            item->setData(Qt::UserRole, i);
            if (axisCount == 3) {
                item->setBackground(Qt::green);
                item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            } else {
                item->setBackground(Qt::red);
                item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
            }
            ui->listWidget->addItem(item);
            qDebug() << "Joystick" << i << "axis count:" << axisCount << "name:" << names[i];
        }
        statusBar()->showMessage(QString::number(names.size()) + " joystick(s) detected", 3000);
    }
    statusBar()->showMessage("Device list updated at " +
                             QDateTime::currentDateTime().toString("hh:mm:ss"), 3000);
}

void MainWindow::pollAxisValues() {
#ifdef SDL_SUPPORTED
    if (inputMode != InputMode::Keyboard) {
        SDL_PumpEvents();
        SDL_Joystick *sdl_joystick = SDL_JoystickOpen(cameraJoystickIndex);
        if (sdl_joystick) {
            int numAxes = SDL_JoystickNumAxes(sdl_joystick);
            for (int a = 0; a < numAxes; a++) {
                Sint16 raw = SDL_JoystickGetAxis(sdl_joystick, a);
                qreal normalized = (raw < 0) ? (raw / 32768.0) : (raw / 32767.0);
                updateAxisValues(cameraJoystickIndex, a, normalized);
            }
        }
    }
#endif
}

void MainWindow::updateAxisValues(int js, int axis, qreal value) {
    if (axis == 0 || axis == 1) {
        int percent = static_cast<int>((value + 1.0) * 50);
        if (axis == 0)
            ui->progressBar->setValue(percent);
        else if (axis == 1)
            ui->progressBar_2->setValue(percent);
    } else if (axis == 2) {
        int steps = static_cast<int>((currentZoom - MIN_ZOOM)/ZOOM_STEP_CONSTANT + 0.5f);
        int maxSteps = static_cast<int>((MAX_ZOOM - MIN_ZOOM)/ZOOM_STEP_CONSTANT + 0.5f);
        ui->progressBar_3->setRange(0, maxSteps);
        ui->progressBar_3->setTextVisible(true);
        ui->progressBar_3->setFormat("%v");
        ui->progressBar_3->setValue(steps);
    } else {
        qDebug() << "Unknown axis index:" << axis;
    }
    if (inputMode != InputMode::Keyboard && js == cameraJoystickIndex) {
        onJoystickAxisChanged(js, axis, value);
    }
}

void MainWindow::updateButtonState(int js, int button, bool pressed) {
    if (js != 0)
        return;
    if (button == 0) {
        ui->toolButton->setText(pressed ? "Pressed" : "Released");
        ui->toolButton->setStyleSheet(pressed ? "background-color: green;" : "background-color: red;");
    }
    else if (button == 1) {
        ui->toolButton_2->setText(pressed ? "Pressed" : "Released");
        ui->toolButton_2->setStyleSheet(pressed ? "background-color: green;" : "background-color: red;");
    }
}

void MainWindow::onJoystickAxisChanged(int dev, int axis, qreal value)
{
    // Only when in joystick‐mode and on the selected device
    if (inputMode != InputMode::Joystick || dev != cameraJoystickIndex)
        return;

    // 1) Dead-zone and cubic mapping
    constexpr qreal DEAD_ZONE = 0.05;
    auto applyCurve = [&](qreal v) {
        if (qAbs(v) < DEAD_ZONE) return 0.0;
        qreal sign = v < 0 ? -1.0 : 1.0;
        qreal m = (qAbs(v) - DEAD_ZONE) / (1.0 - DEAD_ZONE);
        return sign * (m * m * m);
    };
    qreal curved = applyCurve(value);

    // 2) Handle the three axes
    if (axis == 0) {
        // Left/right → yaw
        int yaw = static_cast<int>(curved * MOVE_SPEED);
        QMutexLocker locker(&commandMutex);
        currentYawSpeed = yaw;
    }
    else if (axis == 1) {
        // Up/down → pitch
        int pitch = static_cast<int>(curved * MOVE_SPEED);
        QMutexLocker locker(&commandMutex);
        currentPitchSpeed = pitch;
    }
    // else if (axis == 2) {
    //     // Third axis → zoom
    //     float target = currentZoom + curved * ZOOM_SPEED;
    //     // Clamp to your min/max
    //     float clamped = qBound(MIN_ZOOM, target, MAX_ZOOM);
    //     if (!qFuzzyCompare(clamped, currentZoom)) {
    //         currentZoom = clamped;
    //         sdk->set_absolute_zoom(currentZoom, 1);
    //         sdk->request_autofocus();
    //     }
    //     return;   // don’t also send a gimbal‐move
    // }
    else if (axis == 2) {
        // 1) ignore all “pull back” (negative) values so center=0
        qreal v = qMax<qreal>(value, 0.0);

        // 2) choose how many steps you want: e.g. 7 steps → levels 0..7
        constexpr int ZOOM_STEPS = 5;
        //int ZOOM_STEPS = int((MAX_ZOOM - MIN_ZOOM) / ZOOM_STEP_CONSTANT + 0.5f);
        // 3) map [0..1] → [0..ZOOM_STEPS], rounding to nearest integer
        int level = int(v * ZOOM_STEPS + 0.5);
        level = qBound(0, level, ZOOM_STEPS);

        // 4) only change zoom when we actually cross into a new step
        if (level != lastZoomLevel) {
            // compute the actual zoom value for this step
            float newZoom = MIN_ZOOM + level * ZOOM_STEP_CONSTANT;
            newZoom = qBound(MIN_ZOOM, newZoom, MAX_ZOOM);

            currentZoom = newZoom;
            sdk->set_absolute_zoom(currentZoom, 1);
            sdk->request_autofocus();

            lastZoomLevel = level;
        }

        // update your little UI slider (if you like):
        ui->progressBar_3->setRange(0, ZOOM_STEPS);
        ui->progressBar_3->setFormat("%v");
        ui->progressBar_3->setValue(level);

        return;   // don't send any yaw/pitch
    }

    else {
        // Other axes: ignore
        return;
    }

// 3) (Optional) fire autofocus immediately
// sdk->request_autofocus();

#ifdef _DEBUG
    qDebug() << "[JS] axis="<<axis
             << "raw="<<value
             << "curved="<<curved
             << " → YawSpeed="<<currentYawSpeed
             << " PitchSpeed="<<currentPitchSpeed
        ;
#endif
}


void MainWindow::onSwitchToKeyboard() {
    QMutexLocker locker(&commandMutex);
    inputMode = InputMode::Keyboard;
    currentYawSpeed = 0;
    currentPitchSpeed = 0;
    sdk->set_gimbal_speed(0, 0);
    statusBar()->showMessage("Keyboard mode active");
    qDebug() << "[MODE] keyboard";
    ui->switchtokeyboard->setStyleSheet("background-color: green;");
    ui->switchtojoystick->setStyleSheet("");
    QWidget* w = ui->tabWidget->currentWidget();
    QPropertyAnimation* fade = new QPropertyAnimation(w, "windowOpacity", this);
    fade->setDuration(200);
    fade->setStartValue(0.0);
    fade->setEndValue(1.0);
    fade->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::onSwitchToJoystick() {
    QMutexLocker locker(&commandMutex);
    inputMode = InputMode::Joystick;
    currentYawSpeed = 0;
    currentPitchSpeed = 0;
    sdk->set_gimbal_speed(0, 0);
    statusBar()->showMessage("Joystick mode active");
    qDebug() << "[MODE] joystick";
    ui->switchtojoystick->setStyleSheet("background-color: green;");
    ui->switchtokeyboard->setStyleSheet("");
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    // Let QLineEdits handle their own typing
    if (qobject_cast<QLineEdit*>(qApp->focusWidget())) {
        return QMainWindow::keyPressEvent(event);
    }

    if (inputMode == InputMode::Keyboard) {
        int oldPos = _servoPosition;

        switch (event->key()) {
        case Qt::Key_Z:  // tilt up
            _servoPosition = qBound(0, _servoPosition - 5, 180);
            ui->toolButtonUp->setStyleSheet("background-color: green;");
            break;
        case Qt::Key_S:  // tilt down
            _servoPosition = qBound(0, _servoPosition + 5, 180);
            ui->toolButtonDown->setStyleSheet("background-color: green;");
            break;
        case Qt::Key_Q:  // pan left
            currentYawSpeed = -MOVE_SPEED;
            ui->toolButtonLeft->setStyleSheet("background-color: green;");
            break;
        case Qt::Key_D:  // pan right
            currentYawSpeed = MOVE_SPEED;
            ui->toolButtonRight->setStyleSheet("background-color: green;");
            break;
        // … your zoom cases unchanged …
        default:
            return QMainWindow::keyPressEvent(event);
        }
        event->accept();

        // Only emit if the position actually changed
        if (_servoPosition != oldPos) {
            emit servoPositionChanged(_servoPosition);
        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
}


void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    // same “let line‑edit” guard
    if (qobject_cast<QLineEdit*>(qApp->focusWidget())) {
        return QMainWindow::keyReleaseEvent(event);
    }

    if (inputMode == InputMode::Keyboard) {
        switch (event->key()) {
        case Qt::Key_Z:
        case Qt::Key_S:
            ui->toolButtonUp->setStyleSheet("");
            ui->toolButtonDown->setStyleSheet("");
            break;
        case Qt::Key_Q:
        case Qt::Key_D:
            currentYawSpeed = 0;
            ui->toolButtonLeft->setStyleSheet("");
            ui->toolButtonRight->setStyleSheet("");
            break;
        // … zoom key‑up styling as before …
        default:
            QMainWindow::keyReleaseEvent(event);
        }
        event->accept();
    } else {
        QMainWindow::keyReleaseEvent(event);
    }
}


void MainWindow::sendGimbalCommands() {

    if (inputMode == InputMode::None)
        return;
    QMutexLocker locker(&commandMutex); 
    // Always send commands regardless of speed values
    bool success = sdk->set_gimbal_speed(currentYawSpeed, currentPitchSpeed);
    //sdk->request_autofocus();
    //qDebug() << "Command sent - Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed << "Success:" << success;
    //qDebug() << "Command:" << currentYawSpeed << "," << currentPitchSpeed 
    //         << (success ? "Succeeded" : "Failed");
    //#ifdef _DEBUG
    //qDebug() << "Command sent - Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed << "Success:" << success;
    //#endif
    if (inputMode != InputMode::Joystick) {
        currentYawSpeed   = 0;
        currentPitchSpeed = 0;
    }
    if (inputMode != InputMode::Keyboard) {
        currentYawSpeed   = 0;
        currentPitchSpeed = 0;
    }
}




void MainWindow::saveConfig() {
    // Retrieve user input from QLineEdits.
    QString ip = ui->lineEditIP->text();
    QString portStr = ui->lineEditPort->text();
    QString path = ui->lineEditPath->text();

    // Validate that port is numeric.
    bool ok;
    int port = portStr.toInt(&ok);
    if (!ok) {
        statusBar()->showMessage("Invalid port number", 3000);
        return;
    }

    // Build the config object.
    QJsonObject obj;
    obj["ip"] = ip;
    obj["port"] = port;
    obj["path"] = path;
    QJsonDocument doc(obj);

    // Get the standard config location (usually ~/.config)
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString subFolder = "Haxa5Camera";  // our desired subfolder name

    // Build the full directory path.
    QDir configDirectory(configDir);
    if (!configDirectory.exists(subFolder)) {
        if (!configDirectory.mkdir(subFolder)) {
            statusBar()->showMessage("Failed to create config subdirectory", 3000);
            return;
        }
    }

    // Build the full path for the configuration file.
    QString configFile = configDirectory.filePath(subFolder + "/Hexa5CameraConfig.json");

    // Write JSON to the file.
    QFile file(configFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        statusBar()->showMessage("Configuration saved successfully", 3000);
        qDebug() << "Saved config to " << configFile;
    } else {
        statusBar()->showMessage("Failed to open config file for writing", 3000);
        qDebug() << "Failed to open file " << configFile;
    }
}



void MainWindow::saveDefaultConfig() {
    // Default configuration values:
    QString defaultIP = "192.168.144.25";
    int defaultPort = 8554;
    QString defaultPath = "/main.264";
    QJsonObject obj;
    obj["ip"] = defaultIP;
    obj["port"] = defaultPort;
    obj["path"] = defaultPath;

    QJsonDocument doc(obj);

    // Get the configuration directory (e.g., ~/.config)
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString subFolder = "Haxa5Camera";
    QDir configDirectory(configDir);
    if (!configDirectory.exists(subFolder)) {
        if (!configDirectory.mkdir(subFolder)) {
            statusBar()->showMessage("Failed to create config subdirectory", 3000);
            qWarning() << "Could not create subfolder" << subFolder << "in" << configDir;
            return;
        }
    }

    QString configFile = configDirectory.filePath(subFolder + "/Hexa5CameraConfig.json");

    QFile file(configFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        statusBar()->showMessage("Default configuration saved", 3000);
        qDebug() << "Saved default config to " << configFile;
    } else {
        statusBar()->showMessage("Failed to open config file for writing", 3000);
        qWarning() << "Failed to open file" << configFile;
    }
}



void MainWindow::handleCommandFeedback(const QString& commandId, bool success) {
    QString cmdName;
    if (commandId == "07") {  // From message.h's GIMBAL_ROTATION
        cmdName = "Gimbal Rotation";
    } else if (commandId == "0f") {  // From message.h's ABSOLUTE_ZOOM
        cmdName = "Absolute Zoom";
    } else {
        cmdName = "Unknown Command";
    }
    statusBar()->showMessage(QString("%1: %2").arg(cmdName).arg(success ? "Success" : "Failed"), 3000);
}


void killExistingInstances_() {
    FILE* pipe = popen("ps -aux | grep JoystickIdentifier | grep -v grep", "r");
    if (!pipe) {
        perror("popen failed");
        return;
    }

    std::vector<pid_t> pids;
    char buffer[256];
    
    while (fgets(buffer, sizeof(buffer), pipe)) {
        std::string line(buffer);
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        
        while (iss >> token) {
            tokens.push_back(token);
        }
        
        if (tokens.size() > 1) {
            try {
                pid_t pid = static_cast<pid_t>(std::stoi(tokens[1]));
                pids.push_back(pid);
            } catch (const std::exception& e) {
                // Invalid PID format, skip
            }
        }
    }
    pclose(pipe);

    for (pid_t pid : pids) {
        if (kill(pid, SIGKILL) == 0) {
            printf("Killed process %d\n", pid);
        } else {
            perror(("Failed to kill process " + std::to_string(pid)).c_str());
        }
    }
}

void MainWindow::showEvent(QShowEvent *event) {
    // 1) always forward to base
    QMainWindow::showEvent(event);

    // 2) your existing grabKeyboard()
    //grabKeyboard();

}

void MainWindow::resizeEvent(QResizeEvent *ev) {
    QMainWindow::resizeEvent(ev);
    // always keep the splash sized to fill:
    if (splashVideo && splashVideo->isVisible()) {
        splashVideo->setGeometry(ui->centralwidget->rect());
    }

    if (recordOverlay && recordOverlay->isVisible()) {
        recordOverlay->setFixedWidth(videoWidget->width());
        recordOverlay->move(0, 0);
    }
}

void MainWindow::closeEvent(QCloseEvent* ev)
{
    // 1) gracefully shut down the stream
    if (videoWidget) {
        auto *rcv = videoWidget->getReceiver();
        if (rcv) {
            // asynchronously stop the pipeline
            QtConcurrent::run([rcv]() {
                rcv->stop();     // this will block—but not on the GUI thread
            });
        }
    }

    // 2) (optional) kill any *other* instances—but do NOT SIGKILL your own PID
    killExistingInstances_(); // ← drop this

    // 3) Finish closing
    QMainWindow::closeEvent(ev);
    QCoreApplication::quit();

    if (_servo && _servo->isConnected()) {
        _servo->disconnect();
    }
}

// bool MainWindow::eventFilter(QObject *watched, QEvent *event) {

//     if (inputMode != InputMode::Joystick) {
//         if (event->type() == QEvent::KeyPress) {
//             auto *ke = static_cast<QKeyEvent*>(event);
//             keyPressEvent(ke);
//             return true;
//         }
//         if (event->type() == QEvent::KeyRelease) {
//             auto *ke = static_cast<QKeyEvent*>(event);
//             keyReleaseEvent(ke);
//             return true;
//         }
//     }
//     if (watched == videoWidget && event->type() == QEvent::Resize) {
//         // keep your overlay full-width and pinned at top
//         recordOverlay->setFixedWidth(videoWidget->width());
//         recordOverlay->move(0,0);
//         return false;  // allow default handling too
//     }
//     return QMainWindow::eventFilter(watched, event);
// }

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    // 1) If it's a key event, and the focus widget is a QLineEdit,
    if ((event->type() == QEvent::KeyPress ||
         event->type() == QEvent::KeyRelease))
    {
        if (qobject_cast<QLineEdit*>(QApplication::focusWidget())) {
            return false;
        }
    }

    // 2) Only when we're in Keyboard‐mode do we intercept keys:
    if (inputMode == InputMode::Keyboard) {
        if (event->type() == QEvent::KeyPress) {
            keyPressEvent(static_cast<QKeyEvent*>(event));
            return true;
        }
        if (event->type() == QEvent::KeyRelease) {
            keyReleaseEvent(static_cast<QKeyEvent*>(event));
            return true;
        }
    }

    // 3) Keep your video‐resize handling:
    if (watched == videoWidget && event->type() == QEvent::Resize) {
        recordOverlay->setFixedWidth(videoWidget->width());
        recordOverlay->move(0,0);
        return false;
    }

    return QMainWindow::eventFilter(watched, event);
}



void MainWindow::onCameraStarted() {
    ui->lineEditCameraStatus->setText("Camera Working");
    ui->lineEditCameraStatus->setStyleSheet(
        "background-color: #ccffcc; color: darkgreen;");
    auto *vr = videoWidget->getReceiver();
    vr->setWindowId(videoWidget->winId());
}

void MainWindow::onCameraError(const QString &msg) {
    ui->lineEditCameraStatus->setText("Camera Not Working");
    ui->lineEditCameraStatus->setStyleSheet(
        "background-color: #ffcccc; color: darkred;");
    qDebug() << "GStreamer error:" << msg;
}


// void MainWindow::refreshCameraStatus() {
//     auto *vr = videoWidget->getReceiver();
//     connect(vr, &VideoReceiver::cameraStarted, this, &MainWindow::onCameraStarted);
//     connect(vr, &VideoReceiver::cameraError,   this, &MainWindow::onCameraError);
// }

#include <QProcess>    // at top of mainwindow.cpp

// …


QString MainWindow::loadControlIp() const
{
    // where VideoReceiver already looks:
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString subFolder = "Haxa5Camera";
    QDir dir(configDir);
    QString cfgFile = dir.filePath(subFolder + "/Hexa5CameraConfig.json");

    // defaults in case JSON is missing or invalid
    const QString defaultIp = QString::fromUtf8("192.168.1.64");

    QFile f(cfgFile);
    if (!f.open(QIODevice::ReadOnly))
        return defaultIp;

    auto data = f.readAll();
    f.close();

    auto doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
        return defaultIp;

    auto obj = doc.object();
    return obj.value("ip").toString(defaultIp);
}

void MainWindow::refreshCameraStatus() {
    // Don’t start a new ping if one is already running
    if (pingProcess && pingProcess->state() != QProcess::NotRunning)
        return;

    QString camIp = loadControlIp();
    if (!pingProcess) {
        pingProcess = new QProcess(this);
        connect(pingProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &MainWindow::handlePingFinished);
    }

    // Fire off one ping; we won’t wait() here:
    pingProcess->start("ping", { "-c", "1", "-W", "1", camIp });
}

void MainWindow::handlePingFinished(int exitCode, QProcess::ExitStatus status) {
    Q_UNUSED(status);
    bool alive = (exitCode == 0);

    if (!alive) {
        onCameraError(QStringLiteral("Camera unreachable (ping failed)"));
    } else {
        // Now that ping is good, check your stream
        auto *vr = videoWidget->getReceiver();
        if (vr && vr->isPlaying())
            onCameraStarted();
        else
            onCameraError(QStringLiteral("Stream not playing"));
    }

    // Clean up so future pings can fire
    // (or leave it around if you want to throttle)
    // pingProcess->deleteLater();
    // pingProcess = nullptr;
}


#include <signal.h>  // for SIGINT
#include <unistd.h>  // for ::kill

void MainWindow::on_RecordButton_clicked()
{
    // 0) if the camera isn’t active, warn and bail
    if (auto *vr = videoWidget->getReceiver()) {
        if (!vr->isPlaying()) {
            QMessageBox::warning(
                this,
                tr("Recording unavailable"),
                tr("The camera stream is not active.\nRecording is unavailable.")
                );
            return;
        }
    }

    // 1) Trim any stray newline
    QString uri = rtspUri.trimmed();

    if (recordState == RecordState::Idle) {
        // ── START RECORDING ──

        // 2) prepare path
        QString dir = QDir::homePath() + "/Hexa5CameraRecordedVideos";
        QDir().mkpath(dir);
        QString fn = QDateTime::currentDateTime()
                         .toString("yyyyMMdd_hhmmss") + ".mp4";
        lastRecordPath = dir + "/" + fn;

        // 3) launch ffmpeg
        QStringList args = {
            "-rtsp_transport", "tcp",
            "-i",              uri,
            "-c",              "copy",
            "-y",
            lastRecordPath
        };
        qDebug() << "[Record] will run: ffmpeg" << args;

        delete recordProcess;
        recordProcess = new QProcess(this);
        recordProcess->setProcessChannelMode(QProcess::MergedChannels);
        connect(recordProcess, &QProcess::readyReadStandardError, [this]() {
            auto err = recordProcess->readAllStandardError();
            qDebug() << "[ffmpeg]" << err.trimmed();
        });
        recordProcess->start("ffmpeg", args);

        // 4) wait up to 2 s for it to actually start
        if (!recordProcess->waitForStarted(2000) ||
            recordProcess->state() != QProcess::Running)
        {
            QMessageBox::warning(
                this,
                tr("Recording"),
                tr("Could not start ffmpeg — check your URI and network.")
                );
            delete recordProcess;
            recordProcess = nullptr;
            return;
        }

        // 5) update UI
        recordOverlay->setFixedWidth(videoWidget->width());
        recordOverlay->move(0, 0);
        recordState = RecordState::Recording;
        ui->RecordButton->setText(tr("Stop Recording"));
        recordClock.start();
        recordOverlay->setText(tr("● REC   00:00"));
        recordOverlay->show();
        recordUiTimer->start();
        statusBar()->showMessage(tr("🔴 Recording started"), 2000);

    } else {
        // ── STOP RECORDING ──

        recordUiTimer->stop();

        if (recordProcess) {
            // ask ffmpeg to finish cleanly (SIGINT == Ctrl+C)
            qint64 pid = recordProcess->processId();
            if (pid > 0) ::kill(pid, SIGINT);

            // give it up to 5 s to write the trailer
            if (!recordProcess->waitForFinished(5000)) {
                recordProcess->kill();
                recordProcess->waitForFinished();
            }

            delete recordProcess;
            recordProcess = nullptr;
        }

        // restore UI
        recordOverlay->hide();
        recordState = RecordState::Idle;
        ui->RecordButton->setText(tr("Start Recording"));
        statusBar()->showMessage(
            tr("Recording saved to:\n%1").arg(lastRecordPath),
            5000
            );
    }
}


// void MainWindow::on_RecordButton_clicked()
// {
//     // Trim any stray newline
//     QString uri = rtspUri.trimmed();

//     if (recordState == RecordState::Idle) {
//         // ── START RECORDING ──

//         // 1) prepare path
//         QString dir = QDir::homePath() + "/Hexa5CameraRecordedVideos";
//         QDir().mkpath(dir);
//         QString fn = QDateTime::currentDateTime()
//                          .toString("yyyyMMdd_hhmmss") + ".mp4";
//         lastRecordPath = dir + "/" + fn;

//         // 2) launch ffmpeg
//         QStringList args = {
//             "-rtsp_transport", "tcp",
//             "-i",              uri,
//             "-c",              "copy",
//             "-y",
//             lastRecordPath
//         };
//         qDebug() << "[Record] will run: ffmpeg" << args;

//         delete recordProcess;
//         recordProcess = new QProcess(this);
//         recordProcess->setProcessChannelMode(QProcess::MergedChannels);
//         connect(recordProcess, &QProcess::readyReadStandardError, [this]() {
//             auto err = recordProcess->readAllStandardError();
//             qDebug() << "[ffmpeg]" << err.trimmed();
//         });
//         recordProcess->start("ffmpeg", args);

//         // 3) wait up to 2 s for it to actually start
//         if (!recordProcess->waitForStarted(2000) ||
//             recordProcess->state() != QProcess::Running)
//         {
//             QMessageBox::warning(this, "Recording",
//                                  "Could not start ffmpeg — check your URI and network.");
//             delete recordProcess;
//             recordProcess = nullptr;
//             return;
//         }

//         // 4) update UI
//         recordState = RecordState::Recording;
//         ui->RecordButton->setText("Stop Recording");
//         recordClock.start();
//         recordOverlay->setText("● REC   00:00");
//         recordOverlay->show();
//         recordUiTimer->start();
//         statusBar()->showMessage("🔴 Recording started", 2000);
//     }
//     else {
//         // ── STOP RECORDING ──

//         // stop the overlay timer
//         recordUiTimer->stop();

//         if (recordProcess) {
//             // ask ffmpeg to finish cleanly (SIGINT == Ctrl+C)
//             qint64 pid = recordProcess->processId();
//             if (pid > 0) {
//                 ::kill(pid, SIGINT);
//             }

//             // give it up to 5 s to write the trailer
//             if (!recordProcess->waitForFinished(5000)) {
//                 // if it’s still alive, force-kill
//                 recordProcess->kill();
//                 recordProcess->waitForFinished();
//             }

//             delete recordProcess;
//             recordProcess = nullptr;
//         }

//         // restore UI
//         recordOverlay->hide();
//         recordState = RecordState::Idle;
//         ui->RecordButton->setText("Start Recording");
//         statusBar()->showMessage(
//             QString("Recording saved to:\n%1").arg(lastRecordPath),
//             5000
//             );
//     }
// }

//Test with PC camera
// void MainWindow::on_RecordButton_clicked()
// {
//     // shorthand
//     auto *rcvr = videoWidget->getReceiver();

//     if (recordState == RecordState::Idle) {
//         // ── STOP THE PREVIEW ──
//         rcvr->stop();

//         // ── START RECORDING ──
//         QString dir = QDir::homePath() + "/Hexa5CameraRecordedVideos";
//         QDir().mkpath(dir);
//         QString fn = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".mp4";
//         lastRecordPath = dir + "/" + fn;

//         QStringList args = {
//             "-f",       "v4l2",
//             "-framerate","30",
//             "-video_size","640x480",
//             "-i",       "/dev/video0",
//             "-c:v",     "libx264",
//             "-preset",  "veryfast",
//             "-y",
//             lastRecordPath
//         };

//         delete recordProcess;
//         recordProcess = new QProcess(this);
//         recordProcess->setProcessChannelMode(QProcess::MergedChannels);

//         qDebug() << "[Recording] starting ffmpeg" << args;
//         recordProcess->start("ffmpeg", args);

//         if (!recordProcess->waitForStarted(2000) ||
//             recordProcess->state() != QProcess::Running)
//         {
//             QMessageBox::warning(this, "Recording",
//                                  "Could not start ffmpeg — /dev/video0 busy or missing.");
//             // if ffmpeg failed, restart preview:
//             rcvr->start();
//             return;
//         }

//         // UI
//         recordState = RecordState::Recording;
//         ui->RecordButton->setText("Stop Recording");
//         recordClock.start();
//         recordOverlay->setText("● REC   00:00");
//         recordOverlay->show();
//         recordUiTimer->start();
//         statusBar()->showMessage("🔴 Recording started");

//     } else {
//         // ── STOP RECORDING ──
//         recordUiTimer->stop();

//         if (recordProcess) {
//             recordProcess->terminate();
//             if (!recordProcess->waitForFinished(3000))
//                 recordProcess->kill();
//             delete recordProcess;
//             recordProcess = nullptr;
//         }

//         recordOverlay->hide();
//         recordState = RecordState::Idle;
//         ui->RecordButton->setText("Start Recording");
//         statusBar()->showMessage(
//             QString("Recording saved to:\n%1").arg(lastRecordPath),
//             5000
//             );

//         // ── RESTART THE PREVIEW ──
//         rcvr->start();
//     }
// }


void MainWindow::updateRecordTime()
{
    int total = recordClock.elapsed() / 1000;
    int m = (total / 60) % 60;
    int s = total % 60;
    recordOverlay->setText(
        QString("● REC   %1:%2")
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0')));
}



void MainWindow::onRecordingFinished(int exitCode,
                                     QProcess::ExitStatus status)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(status);

    // Clean up the process object
    recProcess->deleteLater();
    recProcess = nullptr;

    isRecording = false;
    ui->RecordButton->setEnabled(true);
    ui->RecordButton->setText("Start Recording");
    recordOverlay->hide();

    QMessageBox::information(
        this,
        "Recording Complete",
        QString("Saved to:\n%1").arg(currentRecordPath));
}


#include <QGuiApplication>
#include <QScreen>


void MainWindow::on_ScreenshotButton_clicked()
{
    // 1) prepare directory & filename
    QString dir = QDir::homePath() + "/Hexa5CameraScreenshots";
    QDir().mkpath(dir);
    QString fn = QDateTime::currentDateTime()
                     .toString("yyyyMMdd_hhmmss") + ".png";
    QString fullPath = dir + "/" + fn;

    // 2) grab the X11 window that xvimagesink is drawing into
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) {
        statusBar()->showMessage("📸 No screen available!", 3000);
        return;
    }

    // videoWidget is your VideoRecorderWidget* embedded in the UI
    WId videoXid = this->videoWidget->winId();
    QPixmap pix = screen->grabWindow(videoXid);

    // 3) save to disk
    if (!pix.save(fullPath, "PNG")) {
        statusBar()->showMessage("📸 Screenshot failed!", 3000);
        return;
    }

    // 4) feedback
    statusBar()->showMessage(
        QString("📸 Screenshot saved to:\n%1").arg(fullPath),
        5000
        );
}


// Slam‐to‐end moves.  MOVE_SPEED is your max gimbal speed constant.
void MainWindow::onFullUp() {
    // negative pitch is “up” (tweak if your axes are reversed)
    sdk->set_gimbal_speed(0, -MOVE_SPEED);
    sdk->request_autofocus();
}

void MainWindow::onFullDown() {
    sdk->set_gimbal_speed(0, MOVE_SPEED);
    sdk->request_autofocus();
}

void MainWindow::onFullLeft() {
    sdk->set_gimbal_speed(-MOVE_SPEED, 0);
    sdk->request_autofocus();
}

void MainWindow::onFullRight() {
    sdk->set_gimbal_speed(MOVE_SPEED, 0);
    sdk->request_autofocus();
}

// Zoom - jump straight to min/max
void MainWindow::onZoomMaxIn() {
    currentZoom = MAX_ZOOM;
    sdk->set_absolute_zoom(currentZoom, 1);
    sdk->request_autofocus();
}

void MainWindow::onZoomMaxOut() {
    currentZoom = MIN_ZOOM;
    sdk->set_absolute_zoom(currentZoom, 1);
    sdk->request_autofocus();
}

void MainWindow::onStop() {
    sdk->set_gimbal_speed(0, 0);
    sdk->request_autofocus();
}


void MainWindow::playIntro(const QString& splashUrl, const QString& css) {
    QUrl videoUrl;

    // Always try local file first
    if (QFile::exists(splashUrl)) {
        videoUrl = QUrl::fromLocalFile(splashUrl);
        qDebug() << "Using local file:" << splashUrl;
    }
    // Then try resource path
    else if (splashUrl.startsWith("qrc:") || QFile::exists(":" + splashUrl)) {
        videoUrl = QUrl(splashUrl);
        qDebug() << "Using resource path:" << splashUrl;
    }
    // Fallback to embedded resource
    else {
        videoUrl = QUrl("qrc:/intro.mp4");
        qWarning() << "Using fallback resource video";
    }

    qDebug() << "Final video URL:" << videoUrl;
    qDebug() << "Video exists:" << QFile::exists(videoUrl.toLocalFile());

    // 1) overlay video widget
    auto* vw = new QVideoWidget(this);
    vw->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    vw->setGeometry(this->rect());
    vw->show();

    // 2) player
    auto* player = new QMediaPlayer(this);
    player->setVideoOutput(vw);
    player->setSource(videoUrl);  // Use the validated URL

    // 3) when it’s done...
    connect(player, &QMediaPlayer::mediaStatusChanged, this,
            [this, vw, player, css](auto st){
                if (st == QMediaPlayer::EndOfMedia) {
                    player->stop();
                    vw->deleteLater();
                    player->deleteLater();

                    // now reveal and style
                    this->showMaximized();
                    this->setStyleSheet(css);
                    ui->controlsContainer->show();
                    ui->toggleButton     ->show();
                    statusBar()         ->show();
                    this->releaseKeyboard();
                }
            });

    // 4) kick it off
    player->play();

    //Error Handling
    connect(player, &QMediaPlayer::errorOccurred, this, [](auto error, auto errorString) {
        qWarning() << "Media player error:" << error << "-" << errorString;
    });
}

