#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDebug>
#include <QTimer>
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





static const char *CONTROL_IP = "10.14.11.3";
static const int CONTROL_PORT = 37260;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      keepRunning(true),
      sdk(nullptr),
      useKeyboard(false),
      currentZoom(1.0f)
{
    ui->setupUi(this);
    QApplication::instance()->installEventFilter(this);
    videoWidget = new VideoRecorderWidget(this);
    videoWidget->setFocusPolicy(Qt::NoFocus);
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
    pollTimer->start(10);

    // Set up a timer to send gimbal commands every 100ms.
    commandTimer = new QTimer(this);
    connect(commandTimer, &QTimer::timeout, this, &MainWindow::sendGimbalCommands);
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
}


MainWindow::~MainWindow() {
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
    if (!useKeyboard) {
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
        int zoomValue = static_cast<int>(value * 3);  // maps [-1,1] to roughly [-3,3]
        ui->progressBar_3->setRange(-3, 3);
        ui->progressBar_3->setTextVisible(true);
        ui->progressBar_3->setFormat("%v");
        ui->progressBar_3->setValue(zoomValue);
    } else {
        qDebug() << "Unknown axis index:" << axis;
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

void MainWindow::onSwitchToKeyboard() {
    QMutexLocker locker(&commandMutex);
    currentYawSpeed = 0;
    currentPitchSpeed = 0;
    useKeyboard = true;
    statusBar()->showMessage("Keyboard mode active");
    #ifdef _DEBUG
    qDebug() << "Switched to keyboard mode";
    #endif
    ui->switchtokeyboard->setStyleSheet("background-color: green;");
    ui->switchtojoystick->setStyleSheet("");
}

void MainWindow::onSwitchToJoystick() {
    QMutexLocker locker(&commandMutex);
    currentYawSpeed = 0;
    currentPitchSpeed = 0;
    useKeyboard = false;
    statusBar()->showMessage("Joystick mode active");
    qDebug() << "Switched to joystick mode";
    ui->switchtojoystick->setStyleSheet("background-color: green;");
    ui->switchtokeyboard->setStyleSheet("");
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (!useKeyboard) {
        QMainWindow::keyPressEvent(event);
        return;
    }
    event->accept();
    {

        switch (event->key()) {
        case Qt::Key_Z:  // pitch up
            //currentPitchSpeed = PITCH_SPEED_CONSTANT;
            currentPitchSpeed = MOVE_SPEED;
            if(ui->toolButtonUp) ui->toolButtonUp->setStyleSheet("background-color: green;");
            #ifdef _DEBUG
            qDebug() << "Key pressed:" << event->key() << "Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed;
            #endif
            break;
        case Qt::Key_S:  // pitch down
            //currentPitchSpeed = -PITCH_SPEED_CONSTANT;
            currentPitchSpeed = -MOVE_SPEED;
            if(ui->toolButtonDown) ui->toolButtonDown->setStyleSheet("background-color: green;");
            #ifdef _DEBUG
            qDebug() << "Key pressed:" << event->key() << "Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed;
            #endif
            break;
        case Qt::Key_Q:  // yaw left
            //currentYawSpeed = -YAW_SPEED_CONSTANT;
            currentYawSpeed = -MOVE_SPEED;
            if(ui->toolButtonLeft) ui->toolButtonLeft->setStyleSheet("background-color: green;");
            #ifdef _DEBUG
            qDebug() << "Key pressed:" << event->key() << "Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed;
             #endif
            break;
        case Qt::Key_D:  // yaw right
            currentYawSpeed = MOVE_SPEED;
            if(ui->toolButtonRight) ui->toolButtonRight->setStyleSheet("background-color: green;");
            #ifdef _DEBUG
            qDebug() << "Key pressed:" << event->key() << "Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed;
             #endif
            break;
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            currentZoom = std::min(MAX_ZOOM, currentZoom + ZOOM_SPEED);
            if(ui->toolButtonZoomPlus) ui->toolButtonZoomPlus->setStyleSheet("background-color: green;");
            sdk->set_absolute_zoom(currentZoom, 1);
            #ifdef _DEBUG
            qDebug() << "Key pressed:" << event->key() << "Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed;
             #endif
            break;
        case Qt::Key_Minus:
        case Qt::Key_Underscore:
            currentZoom = std::max(MIN_ZOOM, currentZoom - ZOOM_SPEED);
            sdk->set_absolute_zoom(currentZoom, 1);
            if(ui->toolButtonZoomMinus) ui->toolButtonZoomMinus->setStyleSheet("background-color: green;");
            #ifdef _DEBUG
            qDebug() << "Key pressed:" << event->key() << "Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed;
             #endif
            break;
        default:
            QMainWindow::keyPressEvent(event);
            break;
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (!useKeyboard) {
        QMainWindow::keyReleaseEvent(event);
        return;
    }
    event->accept();
    {

        switch (event->key()) {
        case Qt::Key_Z:
        case Qt::Key_S:
            //currentPitchAccel = 0.0f;
            currentPitchSpeed = 0;
            if(ui->toolButtonUp) ui->toolButtonUp->setStyleSheet("");
            if(ui->toolButtonDown) ui->toolButtonDown->setStyleSheet("");
            break;
        case Qt::Key_Q:
        case Qt::Key_D:
            //currentYawAccel = 0.0f;
            currentYawSpeed = 0;
            if(ui->toolButtonLeft) ui->toolButtonLeft->setStyleSheet("");
            if(ui->toolButtonRight) ui->toolButtonRight->setStyleSheet("");
            break;
        case Qt::Key_Plus:
        case Qt::Key_Equal:
        case Qt::Key_Minus:
        case Qt::Key_Underscore:
            if(ui->toolButtonZoomPlus) ui->toolButtonZoomPlus->setStyleSheet("");
            if(ui->toolButtonZoomMinus) ui->toolButtonZoomMinus->setStyleSheet("");
            break;
        default:
            QMainWindow::keyReleaseEvent(event);
            break;
        }
    }
}

void MainWindow::sendGimbalCommands() {

    QMutexLocker locker(&commandMutex); 
    // Always send commands regardless of speed values
    bool success = sdk->set_gimbal_speed(currentYawSpeed, currentPitchSpeed);
    //qDebug() << "Command sent - Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed << "Success:" << success;
    //qDebug() << "Command:" << currentYawSpeed << "," << currentPitchSpeed 
    //         << (success ? "Succeeded" : "Failed");
    //#ifdef _DEBUG
    //qDebug() << "Command sent - Yaw:" << currentYawSpeed << "Pitch:" << currentPitchSpeed << "Success:" << success;
    //#endif
    currentYawSpeed = 0;
    currentPitchSpeed = 0;
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

#include <csignal>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

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
    QMainWindow::showEvent(event);
    grabKeyboard();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    releaseKeyboard();
    QCoreApplication::quit();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {

    if (useKeyboard) {
        if (event->type() == QEvent::KeyPress) {
            auto *ke = static_cast<QKeyEvent*>(event);
            keyPressEvent(ke);
            return true; 
        }
        if (event->type() == QEvent::KeyRelease) {
            auto *ke = static_cast<QKeyEvent*>(event);
            keyReleaseEvent(ke);
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

