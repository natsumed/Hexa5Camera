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
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include "SiyiCameraController.h"
#include "ServoCameraController.h"


//#include "servo_client.hpp"

//static const char *CONTROL_IP = "10.14.11.3";
static const int CONTROL_PORT = 37260;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      keepRunning(true),
      //sdk(nullptr),
      currentZoom(1.0f)

{
    ui->setupUi(this);
#ifdef _DEBUG
    QPushButton *dbg = new QPushButton("DBG: pan+50", this);
    dbg->setToolTip("Sends a single setGimbalSpeed(50,0) to see if gimbal moves");
    dbg->setFixedSize(110,24);
    dbg->move(10, 10); // move somewhere unobtrusive
    connect(dbg, &QPushButton::clicked, this, [this]() {
        if (cameraController && cameraController->isRunning()) {
            qDebug() << "[DBG] sending single gimbal speed 50,0";
            cameraController->setGimbalSpeed(50,0);
            QTimer::singleShot(300, this, [this]() { // stop after 300 ms
                if (cameraController) cameraController->setGimbalSpeed(0,0);
            });
        } else {
            qWarning() << "[DBG] controller not ready";
        }
    });
#endif

    qApp->installEventFilter(this);

    ui->toggleButton->setFocusPolicy(Qt::NoFocus);

    // allow children to get focus again
    this->setFocusPolicy(Qt::StrongFocus);
    // and ensure our central widget can accept focus too
    ui->centralwidget->setFocusPolicy(Qt::StrongFocus);

    // ui->lineEditIP  ->setFocusPolicy(Qt::StrongFocus);
    // ui->lineEditPort->setFocusPolicy(Qt::StrongFocus);
    // ui->lineEditPath->setFocusPolicy(Qt::StrongFocus);

    // Ensure cameraTypeStack starts on chooser page:
    if (ui->cameraTypeStack) {
        ui->cameraTypeStack->setCurrentIndex(0); // chooser page

        // Connect chooser buttons
        connect(ui->btnSelectSiyi, &QPushButton::clicked, this, &MainWindow::onSelectSiyiClicked);
        connect(ui->btnSelectServo, &QPushButton::clicked, this, &MainWindow::onSelectServoClicked);

        // Connect back buttons (both pages call same back slot)
        connect(ui->btnSiyiBack, &QPushButton::clicked, this, &MainWindow::onCameraChooseBack);
        connect(ui->btnServoBack, &QPushButton::clicked, this, &MainWindow::onCameraChooseBack);

        if (ui->btnSiyiSave)
            connect(ui->btnSiyiSave, &QPushButton::clicked, this, &MainWindow::saveConfig);
        if (ui->btnServoSave)
            connect(ui->btnServoSave, &QPushButton::clicked, this, &MainWindow::saveConfig);
        if (ui->btnSiyiDefault) {
            connect(ui->btnSiyiDefault, &QPushButton::clicked, this, &MainWindow::onSiyiDefaultClicked);
        }
        if (ui->btnServoDefault) {
            connect(ui->btnServoDefault, &QPushButton::clicked, this, &MainWindow::onServoDefaultClicked);
        }
    }

    // Ensure port fields only accept numbers
    auto setPortValidator = [this](QLineEdit* le){
        if (!le) return;
        le->setValidator(new QIntValidator(1, 65535, this));
    };

    // SIYI ports
    setPortValidator(ui->siyi_lineEditPort);

    // Servo ports
    setPortValidator(ui->servo_lineEditPort);
    setPortValidator(ui->servo_lineEditServoPort);

    // When the user opens the Camera Configuration tab, populate fields
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index){
        // find tab index for Camera Configuration. If you know it's 2, check equality.
        // Here we simply check if the currently visible widget is the camera config page
        QWidget* current = ui->tabWidget->widget(index);
        if (current == ui->tabWidget /* replace with actual page widget pointer if present */) {
            populateConfigFields();
        } else {
            // Alternatively, call populateConfigFields when the user clicks the Camera Configuration button
        }
    });



    QStatusBar* statusBarr = new QStatusBar();
    statusBarr->setStyleSheet("background-color: #2d2d44; color: #aaaaaa;");
    setStatusBar(statusBarr);

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
            //ui->lineEditIP->setFocus();
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
        onSwitchToConfiguration();
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
    qDebug() << "[VideoReceiver] opening RTSP URI:" << rtspUri;


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
    connect(ui->pushButtonConfiguration, &QPushButton::clicked, this, &MainWindow::onSwitchToConfiguration);

    // in MainWindow::MainWindow(...)
    connect(ui->toolButtonUp,    &QToolButton::clicked, this, &MainWindow::onFullUp);
    connect(ui->toolButtonDown,  &QToolButton::clicked, this, &MainWindow::onFullDown);
    connect(ui->toolButtonLeft,  &QToolButton::clicked, this, &MainWindow::onFullLeft);
    connect(ui->toolButtonRight, &QToolButton::clicked, this, &MainWindow::onFullRight);
    connect(ui->toolButtonStop, &QToolButton::clicked, this, &MainWindow::onStop);

    // and your zoom buttons:
    connect(ui->toolButtonZoomPlus,  &QToolButton::clicked, this, &MainWindow::onZoomMaxIn);
    connect(ui->toolButtonZoomMinus, &QToolButton::clicked, this, &MainWindow::onZoomMaxOut);


    // connect(ui->pushButtonSaveConfig, &QPushButton::clicked, this, &MainWindow::saveConfig);
    // connect(ui->DefaultConfig, &QPushButton::clicked, this, &MainWindow::saveDefaultConfig);

    // Set focus policy so that key events arrive at the main window.
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    qApp->installEventFilter(this);


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
    //commandTimer->setInterval(50);
    commandTimer->start(50);
    qDebug() << "[MainWindow] gimbalTimer started (50ms)";


    // // Create the SIYI SDK instance.

    // std::string ip = "10.14.11.3";
    // int port = 37260;
    // sdk = new SIYI_SDK(ip.c_str(), 37260);
    // if (sdk->request_firmware_version()) {
    //     qDebug() << "Requested firmware version. Waiting for response...";
    //     std::this_thread::sleep_for(std::chrono::seconds(2));
    //     auto [code_version, gimbal_version, zoom_version] = sdk->get_firmware_version();
    //     qDebug() << "Code Board: " << code_version.c_str()
    //              << "  Gimbal: " << gimbal_version.c_str()
    //              << "  Zoom: " << zoom_version.c_str();
    // } else {
    //     qDebug() << "Failed to request firmware version.";
    // }
    // if (sdk->request_gimbal_center()){
    //     qDebug() << "Requested gimbal center . Waiting for response...";
    // }
    // if(sdk->request_autofocus()){
    //     qDebug() << "Requested autofocus. Waiting for response...";
    // }

    // receiveThread = std::thread([this]() {
    //     bool keepRunningLocal = keepRunning.load();
    //     sdk->receive_message_loop(keepRunningLocal);
    // });
    // #ifdef _DEBUG
    // qDebug() << "Camera control initialized";
    // #endif

    QTimer::singleShot(100, this, &MainWindow::initializeCameraController);
    createCameraControllerFromConfig();


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
    QString servoIp   = loadServoIp();
    int     servoPort = loadServoPort();

    // // 1) instantiate
    // _servo = std::make_unique<ServoControl::ServoClient>(
    //     servoIp.toStdString(),
    //     servoPort,
    //     /*timeout_ms=*/ 2000
    //     );

    // // 2) try to connect
    // if (!_servo->connect()) {
    //     statusBar()->showMessage(
    //         QString("Servo connect failed: %1")
    //             .arg(QString::fromStdString(_servo->getLastError())),
    //         5000
    //         );
    // } else {
    //     statusBar()->showMessage("Servo connected", 2000);
    // }

    // // 1) take ownership of your existing client and make a worker
    // auto client = std::move(_servo);
    // auto* thread = new QThread(this);
    // auto* worker = new ServoWorker(std::move(client));
    // worker->moveToThread(thread);
    // connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    // thread->start();

    // // 2) expose a signal so we can tell the worker "new position!"
    // connect(this, &MainWindow::servoPositionChanged,
    //         worker, &ServoWorker::setPosition,
    //         Qt::QueuedConnection);

    // // 3) initialize value (if you like)
    // emit servoPositionChanged(_servoPosition);
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
    //delete sdk;
    if (cameraController) {
        cameraController->stop();
        cameraController.reset();
    }
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
            //sdk->set_absolute_zoom(currentZoom, 1);
            if (cameraController) cameraController->setAbsoluteZoom(currentZoom, 1);
            //sdk->request_autofocus();

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

    if (!cameraController) {
        statusBar()->showMessage("No camera controller: cannot enter keyboard mode", 3000);
        qWarning() << "[onSwitchToKeyboard] no controller";
        return;
    }

    if (!cameraController->isRunning()) {
        qDebug() << "[onSwitchToKeyboard] controller not running; attempting start()";
        if (!cameraController->start()) {
            statusBar()->showMessage("Failed to start camera controller", 3000);
            qWarning() << "[onSwitchToKeyboard] failed to start controller";
            return;
        }
        // short, non-blocking wait for the controller to spin up
        QElapsedTimer t; t.start();
        while (t.elapsed() < 300) {
            QCoreApplication::processEvents();
            if (cameraController->isRunning()) break;
            QThread::msleep(10);
        }
        if (!cameraController->isRunning()) {
            statusBar()->showMessage("Controller did not become ready", 3000);
            qWarning() << "[onSwitchToKeyboard] controller not running after start attempt";
            return;
        }
    }
    inputMode = InputMode::Keyboard;
    currentYawSpeed = 0;
    currentPitchSpeed = 0;
    //sdk->set_gimbal_speed(0, 0);
    if (cameraController) cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
    this->setFocus(Qt::OtherFocusReason);
    if (ui->tabWidget) ui->tabWidget->setFocus(Qt::OtherFocusReason);
    this->grabKeyboard();
    statusBar()->showMessage("Keyboard mode active");
    qDebug() << "[MODE] keyboard";
    ui->switchtokeyboard->setStyleSheet("background-color: green;");
    ui->switchtojoystick->setStyleSheet("");
    ui->pushButtonConfiguration->setStyleSheet("");
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
    //sdk->set_gimbal_speed(0, 0);
    if (cameraController) cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
    this->releaseKeyboard();
    statusBar()->showMessage("Joystick mode active");
    qDebug() << "[MODE] joystick";
    ui->switchtojoystick->setStyleSheet("background-color: green;");
    ui->switchtokeyboard->setStyleSheet("");
    ui->pushButtonConfiguration->setStyleSheet("");
}

void MainWindow::onSwitchToConfiguration()
{
    QMutexLocker locker(&commandMutex);
    inputMode = InputMode::Configuration;
    this->releaseKeyboard();
    statusBar()->showMessage("Configuration mode active");
    qDebug() << "[MODE] Configuration";
    ui->pushButtonConfiguration->setStyleSheet("background-color: green;");
    ui->switchtojoystick->setStyleSheet("");
    ui->switchtokeyboard->setStyleSheet("");

}
void MainWindow::keyPressEvent(QKeyEvent* event) {
    qDebug() << "[keyPressEvent] Detailed debug -"
             << "key:" << event->key()
             << "text:" << event->text()
             << "autoRepeat:" << event->isAutoRepeat()
             << "inputMode:" << static_cast<int>(inputMode)
             << "cameraController:" << (cameraController ? "exists" : "null")
             << "controllerRunning:" << (cameraController ? cameraController->isRunning() : false)
             << "focusWidget:" << (qApp->focusWidget() ? qApp->focusWidget()->metaObject()->className() : "null");

    // Let QLineEdits handle their own typing
    if (qobject_cast<QLineEdit*>(qApp->focusWidget())) {
        qDebug() << "[keyPressEvent] Ignoring - QLineEdit has focus";
        return QMainWindow::keyPressEvent(event);
    }

    if (inputMode == InputMode::Keyboard) {
        int oldPos = _servoPosition;

        bool isServo = false;
        if (cameraController) {
            isServo = (dynamic_cast<ServoCameraController*>(cameraController.get()) != nullptr);
        }

        switch (event->key()) {
        case Qt::Key_Z:  // tilt up
            if (isServo) {
                // servo: change absolute position
                _servoPosition = qBound(0, _servoPosition - 5, 180);
                emit servoPositionChanged(_servoPosition);
                ui->toolButtonUp->setStyleSheet("background-color: green;");
            } else {
                // gimbal: set pitch speed (negative for up)
                QMutexLocker locker(&commandMutex);
                currentPitchSpeed = -MOVE_SPEED;
                if (cameraController) {
                    bool okImmediate = cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
                    qDebug() << "[keyPressEvent] immediate setGimbalSpeed returned:" << (okImmediate ? "OK" : "FAIL");
                }
                ui->toolButtonUp->setStyleSheet("background-color: green;");
            }
            break;
        case Qt::Key_S:  // tilt down
            if (isServo) {
                _servoPosition = qBound(0, _servoPosition + 5, 180);
                emit servoPositionChanged(_servoPosition);
                ui->toolButtonDown->setStyleSheet("background-color: green;");
            } else {
                QMutexLocker locker(&commandMutex);
                currentPitchSpeed = MOVE_SPEED;
                if (cameraController) {
                    bool okImmediate = cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
                    qDebug() << "[keyPressEvent] immediate setGimbalSpeed returned:" << (okImmediate ? "OK" : "FAIL");
                }
                ui->toolButtonDown->setStyleSheet("background-color: green;");
            }
            break;
        case Qt::Key_Q:  // pan left
        {
            QMutexLocker locker(&commandMutex);
            currentYawSpeed = -MOVE_SPEED;
            if (cameraController) {
                bool okImmediate = cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
                qDebug() << "[keyPressEvent] immediate setGimbalSpeed returned:" << (okImmediate ? "OK" : "FAIL");
            }
            ui->toolButtonLeft->setStyleSheet("background-color: green;");
        }
        break;
        case Qt::Key_D:  // pan right
        {
            QMutexLocker locker(&commandMutex);
            currentYawSpeed = MOVE_SPEED;
            if (cameraController) {
                bool okImmediate = cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
                qDebug() << "[keyPressEvent] immediate setGimbalSpeed returned:" << (okImmediate ? "OK" : "FAIL");
            }
            ui->toolButtonRight->setStyleSheet("background-color: green;");
        }
        break;
        case Qt::Key_Plus:
        case Qt::Key_Equal:
        {
            QMutexLocker locker(&commandMutex);
            currentZoom = std::min(MAX_ZOOM, currentZoom + ZOOM_SPEED);
            if(ui->toolButtonZoomPlus) ui->toolButtonZoomPlus->setStyleSheet("background-color: green;");
            cameraController->setAbsoluteZoom(currentZoom, 1);
        }
            break;
        case Qt::Key_Minus:
        case Qt::Key_Underscore:
        {
            QMutexLocker locker(&commandMutex);
            currentZoom = std::max(MIN_ZOOM, currentZoom - ZOOM_SPEED);
            if(ui->toolButtonZoomMinus) ui->toolButtonZoomMinus->setStyleSheet("background-color: green;");
            cameraController->setAbsoluteZoom(currentZoom, 1);
        }
            break;
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

    qDebug() << "[keyReleaseEvent]"
             << "key:" << event->key()
             << "text:" << event->text()
             << "autoRepeat:" << event->isAutoRepeat()
             << "inputMode:" << static_cast<int>(inputMode)
             << "focusWidget:" << (qApp->focusWidget()? qApp->focusWidget()->metaObject()->className() : "null");

    // same “let line‑edit” guard
    if (qobject_cast<QLineEdit*>(qApp->focusWidget())) {
        return QMainWindow::keyReleaseEvent(event);
    }

    if (inputMode == InputMode::Keyboard) {

        bool isServo = false;
        if (cameraController) {
            isServo = (dynamic_cast<ServoCameraController*>(cameraController.get()) != nullptr);
        }
        switch (event->key()) {
        case Qt::Key_Z:
        case Qt::Key_S:
            ui->toolButtonUp->setStyleSheet("");
            ui->toolButtonDown->setStyleSheet("");
            if (!isServo) {
                QMutexLocker locker(&commandMutex);
                if (!isServo) currentPitchSpeed = 0;
                if (cameraController) {
                    bool ok = cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
                    qDebug() << "[keyReleaseEvent] immediate stop setGimbalSpeed returned:" << (ok ? "OK" : "FAIL");
                }
            }
            break;
        case Qt::Key_Q:
        case Qt::Key_D:
        {
            QMutexLocker locker(&commandMutex);
            if (!isServo) currentYawSpeed = 0;
            if (cameraController) {
                bool ok = cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
                qDebug() << "[keyReleaseEvent] immediate stop setGimbalSpeed returned:" << (ok ? "OK" : "FAIL");
            }
        }
            currentYawSpeed = 0;
            ui->toolButtonLeft->setStyleSheet("");
            ui->toolButtonRight->setStyleSheet("");
            break;
        case Qt::Key_Plus:
        case Qt::Key_Equal:
        case Qt::Key_Minus:
        case Qt::Key_Underscore:
        {
            QMutexLocker locker(&commandMutex);
        }
            if(ui->toolButtonZoomPlus) ui->toolButtonZoomPlus->setStyleSheet("");
            if(ui->toolButtonZoomMinus) ui->toolButtonZoomMinus->setStyleSheet("");
            break;
        default:
            QMainWindow::keyReleaseEvent(event);
        }
        event->accept();
    } else {
        QMainWindow::keyReleaseEvent(event);
    }
}

void MainWindow::onFullUp() {
    currentYawSpeed = 0;
    currentPitchSpeed = -MOVE_SPEED;
    if (cameraController) cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
}

void MainWindow::onFullDown() {
    currentYawSpeed = 0;
    currentPitchSpeed = MOVE_SPEED;
    if (cameraController) cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
}

void MainWindow::onFullLeft() {
    currentYawSpeed = -MOVE_SPEED;
    currentPitchSpeed = 0;
    if (cameraController) cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
}

void MainWindow::onFullRight() {
    currentYawSpeed = MOVE_SPEED;
    currentPitchSpeed = 0;
    if (cameraController) cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
}

// Zoom - jump straight to min/max
void MainWindow::onZoomMaxIn() {
    currentZoom = MAX_ZOOM;
    // sdk->set_absolute_zoom(currentZoom, 1);
    // sdk->request_autofocus();
    if (cameraController) cameraController->setAbsoluteZoom(currentZoom, 1);
}

void MainWindow::onZoomMaxOut() {
    currentZoom = MIN_ZOOM;
    // sdk->set_absolute_zoom(currentZoom, 1);
    // sdk->request_autofocus();
    if (cameraController) cameraController->setAbsoluteZoom(currentZoom, 1);
}

void MainWindow::onStop() {
    currentYawSpeed = 0;
    currentPitchSpeed = 0;
    if (cameraController) cameraController->setGimbalSpeed(0, 0);
}

void MainWindow::sendGimbalCommands() {
    qDebug().nospace() << "[sendGimbalCommands] invoked; inputMode=" << static_cast<int>(inputMode);

    if ((inputMode == InputMode::None) || (inputMode == InputMode::Configuration)) {
        qDebug() << "[sendGimbalCommands] skipping — mode not active";
        return;
    }

    int yaw, pitch;
    {
        QMutexLocker locker(&commandMutex);
        yaw = currentYawSpeed;
        pitch = currentPitchSpeed;
    }

    if (!cameraController) {
        qDebug() << "[sendGimbalCommands] no cameraController";
        return;
    }

    static int lastYaw = INT_MIN;
    static int lastPitch = INT_MIN;

    // Always send the command, but log only when changed
    bool ok = cameraController->setGimbalSpeed(yaw, pitch);
    if (!ok) {
        qWarning() << "[sendGimbalCommands] controller rejected gimbal speed:" << yaw << pitch;
    } else {
        if (yaw != lastYaw || pitch != lastPitch) {
            qDebug() << "[sendGimbalCommands] cmd -> yaw:" << yaw << " pitch:" << pitch;
            lastYaw = yaw;
            lastPitch = pitch;
        }
    }

    if (inputMode != InputMode::Joystick && inputMode != InputMode::Keyboard) {
        QMutexLocker locker(&commandMutex);
        currentYawSpeed = 0;
        currentPitchSpeed = 0;
    }
}


// void MainWindow::sendGimbalCommands() {

//     if (inputMode == InputMode::None)
//         return;
//     QMutexLocker locker(&commandMutex);
//     // Always send commands regardless of speed values
//     bool success = cameraController->setGimbalSpeed(currentYawSpeed, currentPitchSpeed);
//     //sdk->request_autofocus();
//     //qDebug() << "Command sent - Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed << "Success:" << success;
//     //qDebug() << "Command:" << currentYawSpeed << "," << currentPitchSpeed
//     //         << (success ? "Succeeded" : "Failed");
//     //#ifdef _DEBUG
//     //qDebug() << "Command sent - Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed << "Success:" << success;
//     //#endif
//     if (inputMode != InputMode::Joystick) {
//         currentYawSpeed   = 0;
//         currentPitchSpeed = 0;
//     }
//     if (inputMode != InputMode::Keyboard) {
//         currentYawSpeed   = 0;
//         currentPitchSpeed = 0;
//     }
// }


void MainWindow::onSelectSiyiClicked()
{
    if (!ui->cameraTypeStack) return;
    ui->cameraTypeStack->setCurrentWidget(ui->page_siyi);
}

void MainWindow::onSelectServoClicked()
{
    if (!ui->cameraTypeStack) return;
    ui->cameraTypeStack->setCurrentWidget(ui->page_servo);
}

void MainWindow::onCameraChooseBack()
{
    if (!ui->cameraTypeStack) return;
    ui->cameraTypeStack->setCurrentWidget(ui->page_choose_type);
}

void MainWindow::populateConfigFields()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QDir dir(configDir);
    QString cfgFile = dir.filePath("Haxa5Camera/Hexa5CameraConfig.json");

    // defaults
    QString ipDefault("192.168.1.64");
    int portDefault = 554;
    QString pathDefault("/main.264");
    QString servoIpDefault("10.14.11.1");
    int servoPortDefault = 8000;

    QFile f(cfgFile);
    if (!f.open(QIODevice::ReadOnly)) {
        // set UI defaults
        if (ui->siyi_lineEditIP) ui->siyi_lineEditIP->setText(ipDefault);
        if (ui->siyi_lineEditPort) ui->siyi_lineEditPort->setText(QString::number(portDefault));
        if (ui->siyi_lineEditPath) ui->siyi_lineEditPath->setText(pathDefault);

        if (ui->servo_lineEditIP) ui->servo_lineEditIP->setText(ipDefault);
        if (ui->servo_lineEditPort) ui->servo_lineEditPort->setText(QString::number(portDefault));
        if (ui->servo_lineEditPath) ui->servo_lineEditPath->setText(pathDefault);
        if (ui->servo_lineEditServoIP) ui->servo_lineEditServoIP->setText(servoIpDefault);
        if (ui->servo_lineEditServoPort) ui->servo_lineEditServoPort->setText(QString::number(servoPortDefault));
        return;
    }

    QByteArray data = f.readAll();
    f.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QString cameraType = "siyi";
    QString ip = ipDefault;
    int port = portDefault;
    QString path = pathDefault;
    QString servoIp = servoIpDefault;
    int servoPort = servoPortDefault;

    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        cameraType = obj.value("cameraType").toString(cameraType).toLower();
        ip = obj.value("ip").toString(ip);
        port = obj.value("port").toInt(port);
        path = obj.value("path").toString(path);
        servoIp = obj.value("servoIP").toString(servoIp);
        servoPort = obj.value("servoPort").toInt(servoPort);
    }

    // Fill fields for SIYI page
    if (ui->siyi_lineEditIP)   ui->siyi_lineEditIP->setText(ip);
    if (ui->siyi_lineEditPort) ui->siyi_lineEditPort->setText(QString::number(port));
    if (ui->siyi_lineEditPath) ui->siyi_lineEditPath->setText(path);

    // Fill fields for Servo page
    if (ui->servo_lineEditIP)      ui->servo_lineEditIP->setText(ip);
    if (ui->servo_lineEditPort)    ui->servo_lineEditPort->setText(QString::number(port));
    if (ui->servo_lineEditPath)    ui->servo_lineEditPath->setText(path);
    if (ui->servo_lineEditServoIP) ui->servo_lineEditServoIP->setText(servoIp);
    if (ui->servo_lineEditServoPort) ui->servo_lineEditServoPort->setText(QString::number(servoPort));

    // Show the page matching the saved type so user sees the right form
    if (ui->cameraTypeStack) {
        if (cameraType == "servo") ui->cameraTypeStack->setCurrentWidget(ui->page_servo);
        else ui->cameraTypeStack->setCurrentWidget(ui->page_siyi);
    }
}


void MainWindow::saveConfig() {
    QString cameraType = "siyi";
    QString ip;
    int port = 0;
    QString path;
    QString servoIP;
    int servoPort = 0;

    // Decide which page is currently visible in the stack:
    int idx = ui->cameraTypeStack ? ui->cameraTypeStack->currentIndex() : 0;
    QWidget *current = ui->cameraTypeStack->currentWidget();

    // If current is SIYI page or we default to it
    if (current == ui->page_siyi) {
        cameraType = "siyi";
        ip = ui->siyi_lineEditIP->text().trimmed();
        port = ui->siyi_lineEditPort->text().toInt();
        path = ui->siyi_lineEditPath->text().trimmed();
    }
    // Servo page
    else if (current == ui->page_servo) {
        cameraType = "servo";
        ip = ui->servo_lineEditIP->text().trimmed();
        port = ui->servo_lineEditPort->text().toInt();
        path = ui->servo_lineEditPath->text().trimmed();
        servoIP = ui->servo_lineEditServoIP->text().trimmed();
        servoPort = ui->servo_lineEditServoPort->text().toInt();
    } else {
        // fallback to siyi fields if you had legacy fields
        ip = ui->siyi_lineEditIP->text().trimmed();
        port = ui->siyi_lineEditPort->text().toInt();
        path = ui->siyi_lineEditPath->text().trimmed();
    }

    // Basic validation
    if (ip.isEmpty() || port <= 0 || path.isEmpty()) {
        statusBar()->showMessage("Invalid camera configuration — fill in IP, Port and Path", 3000);
        return;
    }

    QJsonObject obj;
    obj["cameraType"] = cameraType;
    obj["ip"] = ip;
    obj["port"] = port;
    obj["path"] = path;
    if (cameraType == "servo") {
        obj["servoIP"] = servoIP;
        obj["servoPort"] = servoPort;
    }

    QJsonDocument doc(obj);
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString subFolder = "Haxa5Camera";
    QDir d(configDir);
    d.mkpath(subFolder);
    QString cfgFile = d.filePath(subFolder + "/Hexa5CameraConfig.json");

    QFile file(cfgFile);
    if (!file.open(QIODevice::WriteOnly)) {
        statusBar()->showMessage("Failed to open config file for writing", 3000);
        qWarning() << "Failed to write config to" << cfgFile;
        return;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    statusBar()->showMessage("Configuration saved", 3000);
    qDebug() << "Saved config to" << cfgFile;

    // Re-apply config (restart VideoReceiver and recreate camera controller)
    applyConfig();
    // Also re-create controller specifically (ensure createCameraControllerFromConfig reads cameraType)
    //createCameraControllerFromConfig();
    return;
}


void MainWindow::onSiyiDefaultClicked()
{
    // Default SIYI values:
    const QString defaultIP = QStringLiteral("192.168.144.25");
    const int     defaultPort = 8554;
    const QString defaultPath = QStringLiteral("/main.264");

    if (ui->siyi_lineEditIP)   ui->siyi_lineEditIP->setText(defaultIP);
    if (ui->siyi_lineEditPort) ui->siyi_lineEditPort->setText(QString::number(defaultPort));
    if (ui->siyi_lineEditPath) ui->siyi_lineEditPath->setText(defaultPath);

    // Ensure we show the SIYI page
    if (ui->cameraTypeStack) ui->cameraTypeStack->setCurrentWidget(ui->page_siyi);

    // Persist defaults and apply them
    saveDefaultConfig();
}

void MainWindow::onServoDefaultClicked()
{
    // Default Servo + Camera values:
    const QString defaultIP = QStringLiteral("192.168.144.25");
    const int     defaultPort = 8554;
    const QString defaultPath = QStringLiteral("/main.264");
    const QString defaultServoIP = QStringLiteral("10.14.11.1");
    const int     defaultServoPort = 8000;

    if (ui->servo_lineEditIP)      ui->servo_lineEditIP->setText(defaultIP);
    if (ui->servo_lineEditPort)    ui->servo_lineEditPort->setText(QString::number(defaultPort));
    if (ui->servo_lineEditPath)    ui->servo_lineEditPath->setText(defaultPath);
    if (ui->servo_lineEditServoIP) ui->servo_lineEditServoIP->setText(defaultServoIP);
    if (ui->servo_lineEditServoPort) ui->servo_lineEditServoPort->setText(QString::number(defaultServoPort));

    // Ensure we show the Servo page
    if (ui->cameraTypeStack) ui->cameraTypeStack->setCurrentWidget(ui->page_servo);

    // Persist defaults and apply them
    saveDefaultConfig();
}


void MainWindow::saveDefaultConfig() {
    // Default configuration values:
    const QString defaultIP = QStringLiteral("192.168.144.25");
    const int defaultPort = 8554;
    const QString defaultPath = QStringLiteral("/main.264");
    const QString defaultServoIP = QStringLiteral("10.14.11.1");
    const int defaultServoPort = 8000;

    // Determine which page is currently active so we can set cameraType properly
    QString cameraType = "siyi"; // default

    if (ui->cameraTypeStack) {
        QWidget* cur = ui->cameraTypeStack->currentWidget();
        if (cur == ui->page_servo) cameraType = "servo";
        else cameraType = "siyi";
    }

    // Build JSON object using values from the UI (if present) or the defaults
    QJsonObject obj;

    if (cameraType == "servo") {
        // Use servo page values if available, otherwise use defaults
        QString ip = (ui->servo_lineEditIP && !ui->servo_lineEditIP->text().isEmpty()) ? ui->servo_lineEditIP->text().trimmed() : defaultIP;
        int port = (ui->servo_lineEditPort && ui->servo_lineEditPort->text().toInt() > 0) ? ui->servo_lineEditPort->text().toInt() : defaultPort;
        QString path = (ui->servo_lineEditPath && !ui->servo_lineEditPath->text().isEmpty()) ? ui->servo_lineEditPath->text().trimmed() : defaultPath;
        QString sIP = (ui->servo_lineEditServoIP && !ui->servo_lineEditServoIP->text().isEmpty()) ? ui->servo_lineEditServoIP->text().trimmed() : defaultServoIP;
        int sPort = (ui->servo_lineEditServoPort && ui->servo_lineEditServoPort->text().toInt() > 0) ? ui->servo_lineEditServoPort->text().toInt() : defaultServoPort;

        obj["cameraType"] = cameraType;
        obj["ip"] = ip;
        obj["port"] = port;
        obj["path"] = path;
        obj["servoIP"] = sIP;
        obj["servoPort"] = sPort;
    } else {
        // SIYI defaults
        QString ip = (ui->siyi_lineEditIP && !ui->siyi_lineEditIP->text().isEmpty()) ? ui->siyi_lineEditIP->text().trimmed() : defaultIP;
        int port = (ui->siyi_lineEditPort && ui->siyi_lineEditPort->text().toInt() > 0) ? ui->siyi_lineEditPort->text().toInt() : defaultPort;
        QString path = (ui->siyi_lineEditPath && !ui->siyi_lineEditPath->text().isEmpty()) ? ui->siyi_lineEditPath->text().trimmed() : defaultPath;

        obj["cameraType"] = cameraType;
        obj["ip"] = ip;
        obj["port"] = port;
        obj["path"] = path;
        // ensure servo fields removed so config matches SIYI-only
    }

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

    // Apply config (video & controller will be restarted)
    applyConfig();
    // Recreate controller to pick up cameraType immediately
    //createCameraControllerFromConfig();
    return;
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

    // if (_servo && _servo->isConnected()) {
    //     _servo->disconnect();
    // }
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // Let focused QLineEdit handle typing
    if ((event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) &&
        qobject_cast<QLineEdit*>(qApp->focusWidget()))
    {
        return QMainWindow::eventFilter(watched, event);
    }

    // Only intercept keys when in keyboard mode
    if ((event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
        && inputMode == InputMode::Keyboard)
    {
        if (event->type() == QEvent::KeyPress) {
            keyPressEvent(static_cast<QKeyEvent*>(event));
            return true;
        } else {
            keyReleaseEvent(static_cast<QKeyEvent*>(event));
            return true;
        }
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




#include <QProcess>

QString MainWindow::loadControlIp() const
{
    // where VideoReceiver already looks:
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString subFolder = "Haxa5Camera";
    QDir dir(configDir);
    QString cfgFile = dir.filePath(subFolder + "/Hexa5CameraConfig.json");

    // defaults in case JSON is missing or invalid
    const QString defaultIp = QString::fromUtf8("192.168.144.25");

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

QString MainWindow::loadServoIp() const
{
    // same config file as camera
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QDir dir(configDir);
    QString cfgFile = dir.filePath(QStringLiteral("Haxa5Camera/Hexa5CameraConfig.json"));

    const QString defaultIp = QStringLiteral("10.14.11.1");
    QFile f(cfgFile);
    if (!f.open(QIODevice::ReadOnly)) return defaultIp;
    auto doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject())    return defaultIp;
    return doc.object().value(QStringLiteral("servoIP")).toString(defaultIp);
}

int MainWindow::loadServoPort() const
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QDir dir(configDir);
    QString cfgFile = dir.filePath(QStringLiteral("Haxa5Camera/Hexa5CameraConfig.json"));

    const int defaultPort = 8000;
    QFile f(cfgFile);
    if (!f.open(QIODevice::ReadOnly)) return defaultPort;
    auto doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject())    return defaultPort;
    return doc.object().value(QStringLiteral("servoPort")).toInt(defaultPort);
}


// Synchronous, safe apply: do everything on the GUI thread in a deterministic order.
void MainWindow::applyConfig()
{
    // Simple guard to avoid re-entrancy
    static bool applying = false;
    if (applying) {
        statusBar()->showMessage("Apply already in progress", 2000);
        return;
    }
    applying = true;

    // Disable UI save/default buttons while applying to prevent concurrent clicks
    if (ui->btnSiyiDefault)      ui->btnSiyiDefault->setEnabled(false);
    if (ui->btnServoDefault)     ui->btnServoDefault->setEnabled(false);
    if (ui->btnSiyiSave)         ui->btnSiyiSave->setEnabled(false);
    if (ui->btnServoSave)        ui->btnServoSave->setEnabled(false);

    statusBar()->showMessage("Applying configuration...", 2000);
    qDebug() << "[CONFIG] applyConfig: starting";

    if (commandTimer && commandTimer->isActive()) {
        commandTimer->stop();
        qDebug() << "[CONFIG] applyConfig: commandTimer stopped";
    }

    // 1) Stop VideoReceiver synchronously (safe on main thread)
    if (videoWidget) {
        if (auto *rcv = videoWidget->getReceiver()) {
            qDebug() << "[CONFIG] applyConfig: stopping VideoReceiver";
            // This is synchronous; it sets pipeline to NULL and unrefs elements.
            rcv->stop();

            // Give a short breathing room for GStreamer threads to tear down;
            // this reduces races when we immediately create a new pipeline.
            QThread::msleep(100);
        }
    }

    // 2) Stop and destroy existing camera controller (if any)
    if (cameraController) {
        qDebug() << "[CONFIG] applyConfig: stopping existing cameraController";
        try {
            cameraController->stop();
        } catch (...) {
            qWarning() << "[CONFIG] applyConfig: exception while stopping cameraController";
        }
        cameraController.reset();
    }

    // 3) Recreate the controller from persisted config
    qDebug() << "[CONFIG] applyConfig: creating new cameraController from config";
    try {
        createCameraControllerFromConfig();
    } catch (const std::exception &ex) {
        qWarning() << "[CONFIG] createCameraControllerFromConfig threw:" << ex.what();
        QMessageBox::warning(this, tr("Camera Error"),
                             tr("Failed to create camera controller:\n%1").arg(ex.what()));
    } catch (...) {
        qWarning() << "[CONFIG] createCameraControllerFromConfig unknown exception";
        QMessageBox::warning(this, tr("Camera Error"),
                             tr("Failed to create camera controller (unknown error)."));
    }

    // 4) Restart VideoReceiver synchronously with new RTSP URI
    if (videoWidget) {
        if (auto *rcv = videoWidget->getReceiver()) {
            QString newUri = rcv->getRtspUriFromConfig().trimmed();
            qDebug() << "[CONFIG] applyConfig: setting RTSP URI to" << newUri;
            // set the new URI and start pipeline
            rcv->setRtspUri(newUri);

            qDebug() << "[CONFIG] applyConfig: starting VideoReceiver";
            rcv->start();
        }
    }

    // Re-enable UI buttons
    if (ui->btnSiyiDefault)      ui->btnSiyiDefault->setEnabled(true);
    if (ui->btnServoDefault)     ui->btnServoDefault->setEnabled(true);
    if (ui->btnSiyiSave)         ui->btnSiyiSave->setEnabled(true);
    if (ui->btnServoSave)        ui->btnServoSave->setEnabled(true);

    // Restart command timer if controller present
    if (commandTimer && !commandTimer->isActive()) {
        commandTimer->start(50);
        qDebug() << "[CONFIG] applyConfig: commandTimer restarted";
    }


    statusBar()->showMessage("Configuration applied", 3000);
    qDebug() << "[CONFIG] applyConfig: finished";

    applying = false;
}



void MainWindow::createCameraControllerFromConfig()
{
    // config file path (same folder VideoReceiver used)
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QDir dir(configDir);
    dir.mkpath("Haxa5Camera");
    QString cfgFile = dir.filePath("Haxa5Camera/Hexa5CameraConfig.json");

    QString chosenType = "siyi"; // default
    QString ip = "10.14.11.3";
    int port = 8554;
    QString servoIP;
    int servoPort = 0;

    QFile f(cfgFile);
    if (f.open(QIODevice::ReadOnly)) {
        QByteArray data = f.readAll();
        f.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject obj = doc.object();

            // If there's an explicit cameraType key, prefer it
            if (obj.contains("cameraType")) {
                chosenType = obj.value("cameraType").toString("siyi").toLower();
            } else {
                // Detect by presence of servoIP key in your servo config
                if (obj.contains("servoIP") || obj.contains("servoPort")) {
                    chosenType = "servo";
                } else {
                    chosenType = "siyi";
                }
            }

            // read IP/port for chosen controller (config examples you gave)
            ip = obj.value("ip").toString(ip);
            port = obj.value("port").toInt(port);

            servoIP   = obj.value("servoIP").toString();
            servoPort = obj.value("servoPort").toInt();
        }
    } else {
        qWarning() << "Could not open config file:" << cfgFile << "; using defaults";
    }

    // Tear down existing controller
    if (cameraController) {
        cameraController->stop();
        cameraController.reset();
    }

    if (chosenType == "servo" && !servoIP.isEmpty()) {
        cameraController = std::make_unique<ServoCameraController>(servoIP.toStdString(), servoPort ? servoPort : port);
    } else {
        // default to SIYI
        cameraController = std::make_unique<SiyiCameraController>(ip.toStdString(), 37260);
    }

    if (!cameraController->start()) {
        qWarning() << "Failed to start cameraController";
        // keep it null to avoid using a half-started controller
        cameraController.reset();
        statusBar()->showMessage("Camera controller failed to start", 3000);
    }

    // wire callbacks into MainWindow slots
    cameraController->onStarted = [this]() {
        QMetaObject::invokeMethod(this, "onCameraStarted", Qt::QueuedConnection);
    };
    cameraController->onError = [this](const QString &msg) {
        QMetaObject::invokeMethod(this, [this, msg]() { onCameraError(msg); }, Qt::QueuedConnection);
    };

    // wire servoPositionChanged -> controller absolute position (if supported)
    connect(this, &MainWindow::servoPositionChanged, this, [this](int newPos) {
        if (cameraController && cameraController->supportsAbsolutePosition()) {
            cameraController->setGimbalPosition(0, newPos);
        }
    }, Qt::QueuedConnection);

}

void MainWindow::initializeCameraController()
{
    createCameraControllerFromConfig();

    // Verify controller started successfully
    if (cameraController && cameraController->isRunning()) {
        qDebug() << "Camera controller started successfully";
        statusBar()->showMessage("Camera controller ready", 3000);
    } else {
        qWarning() << "Failed to start camera controller";
        statusBar()->showMessage("Camera controller failed to start", 3000);
    }
}
