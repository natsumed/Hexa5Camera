/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QGroupBox *VideoRecorderSection;
    QMenuBar *menubar;
    QMenu *menuHexa_5;
    QStatusBar *statusbar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QPushButton *Rescan;
    QGroupBox *groupBox;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QListWidget *listWidget;
    QDockWidget *dockWidget_3;
    QWidget *dockWidgetContents_3;
    QGroupBox *groupBox_2;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QProgressBar *progressBar;
    QLabel *label_2;
    QProgressBar *progressBar_2;
    QLabel *label_3;
    QProgressBar *progressBar_3;
    QGroupBox *groupBox_3;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QToolButton *toolButton;
    QToolButton *toolButton_2;
    QGroupBox *gimbalControlGroupBox;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_2;
    QToolButton *toolButton_7;
    QToolButton *toolButtonLeft;
    QToolButton *toolButtonRight;
    QToolButton *toolButtonUp;
    QToolButton *toolButtonDown;
    QWidget *gridLayoutWidget_3;
    QGridLayout *gridLayout_3;
    QToolButton *toolButtonFocusPlus;
    QToolButton *toolButtonZoomPlus;
    QToolButton *toolButtonZoomMinus;
    QPushButton *switchtokeyboard;
    QLabel *focus;
    QToolButton *toolButtonFocusMinus;
    QLabel *zoom;
    QPushButton *switchtojoystick;
    QGroupBox *groupBox_4;
    QLineEdit *lineEditIP;
    QLineEdit *lineEditPort;
    QLineEdit *lineEditPath;
    QPushButton *pushButtonSaveConfig;
    QPushButton *DefaultConfig;
    QGroupBox *groupBox_5;
    QLineEdit *lineEditCameraStatus;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1240, 627);
        MainWindow->setMaximumSize(QSize(1240, 627));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        VideoRecorderSection = new QGroupBox(centralwidget);
        VideoRecorderSection->setObjectName("VideoRecorderSection");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(VideoRecorderSection->sizePolicy().hasHeightForWidth());
        VideoRecorderSection->setSizePolicy(sizePolicy);
        VideoRecorderSection->setMaximumSize(QSize(1230, 1200));

        horizontalLayout->addWidget(VideoRecorderSection);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1240, 22));
        menuHexa_5 = new QMenu(menubar);
        menuHexa_5->setObjectName("menuHexa_5");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName("dockWidget");
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName("dockWidgetContents");
        Rescan = new QPushButton(dockWidgetContents);
        Rescan->setObjectName("Rescan");
        Rescan->setGeometry(QRect(60, 480, 201, 25));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(60, 40, 211, 451));
        verticalLayoutWidget = new QWidget(groupBox);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(0, 29, 201, 411));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        listWidget = new QListWidget(verticalLayoutWidget);
        listWidget->setObjectName("listWidget");

        verticalLayout->addWidget(listWidget);

        dockWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWidget);
        dockWidget_3 = new QDockWidget(MainWindow);
        dockWidget_3->setObjectName("dockWidget_3");
        dockWidgetContents_3 = new QWidget();
        dockWidgetContents_3->setObjectName("dockWidgetContents_3");
        groupBox_2 = new QGroupBox(dockWidgetContents_3);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(60, 0, 141, 251));
        groupBox_2->setFlat(false);
        verticalLayoutWidget_2 = new QWidget(groupBox_2);
        verticalLayoutWidget_2->setObjectName("verticalLayoutWidget_2");
        verticalLayoutWidget_2->setGeometry(QRect(0, 30, 131, 211));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget_2);
        label->setObjectName("label");

        verticalLayout_2->addWidget(label);

        progressBar = new QProgressBar(verticalLayoutWidget_2);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(24);

        verticalLayout_2->addWidget(progressBar);

        label_2 = new QLabel(verticalLayoutWidget_2);
        label_2->setObjectName("label_2");

        verticalLayout_2->addWidget(label_2);

        progressBar_2 = new QProgressBar(verticalLayoutWidget_2);
        progressBar_2->setObjectName("progressBar_2");
        progressBar_2->setValue(24);

        verticalLayout_2->addWidget(progressBar_2);

        label_3 = new QLabel(verticalLayoutWidget_2);
        label_3->setObjectName("label_3");

        verticalLayout_2->addWidget(label_3);

        progressBar_3 = new QProgressBar(verticalLayoutWidget_2);
        progressBar_3->setObjectName("progressBar_3");
        progressBar_3->setMinimum(0);
        progressBar_3->setMaximum(3);
        progressBar_3->setValue(0);
        progressBar_3->setInvertedAppearance(false);

        verticalLayout_2->addWidget(progressBar_3);

        groupBox_3 = new QGroupBox(dockWidgetContents_3);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(60, 260, 141, 101));
        groupBox_3->setFlat(false);
        gridLayoutWidget = new QWidget(groupBox_3);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(7, 30, 131, 61));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        toolButton = new QToolButton(gridLayoutWidget);
        toolButton->setObjectName("toolButton");

        gridLayout->addWidget(toolButton, 0, 0, 1, 1);

        toolButton_2 = new QToolButton(gridLayoutWidget);
        toolButton_2->setObjectName("toolButton_2");

        gridLayout->addWidget(toolButton_2, 1, 0, 1, 1);

        gimbalControlGroupBox = new QGroupBox(dockWidgetContents_3);
        gimbalControlGroupBox->setObjectName("gimbalControlGroupBox");
        gimbalControlGroupBox->setGeometry(QRect(60, 360, 371, 151));
        gridLayoutWidget_2 = new QWidget(gimbalControlGroupBox);
        gridLayoutWidget_2->setObjectName("gridLayoutWidget_2");
        gridLayoutWidget_2->setGeometry(QRect(10, 20, 141, 121));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        toolButton_7 = new QToolButton(gridLayoutWidget_2);
        toolButton_7->setObjectName("toolButton_7");

        gridLayout_2->addWidget(toolButton_7, 1, 1, 1, 1);

        toolButtonLeft = new QToolButton(gridLayoutWidget_2);
        toolButtonLeft->setObjectName("toolButtonLeft");

        gridLayout_2->addWidget(toolButtonLeft, 1, 0, 1, 1);

        toolButtonRight = new QToolButton(gridLayoutWidget_2);
        toolButtonRight->setObjectName("toolButtonRight");

        gridLayout_2->addWidget(toolButtonRight, 1, 2, 1, 1);

        toolButtonUp = new QToolButton(gridLayoutWidget_2);
        toolButtonUp->setObjectName("toolButtonUp");

        gridLayout_2->addWidget(toolButtonUp, 0, 1, 1, 1);

        toolButtonDown = new QToolButton(gridLayoutWidget_2);
        toolButtonDown->setObjectName("toolButtonDown");

        gridLayout_2->addWidget(toolButtonDown, 2, 1, 1, 1);

        gridLayoutWidget_3 = new QWidget(gimbalControlGroupBox);
        gridLayoutWidget_3->setObjectName("gridLayoutWidget_3");
        gridLayoutWidget_3->setGeometry(QRect(170, 20, 182, 118));
        gridLayout_3 = new QGridLayout(gridLayoutWidget_3);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        toolButtonFocusPlus = new QToolButton(gridLayoutWidget_3);
        toolButtonFocusPlus->setObjectName("toolButtonFocusPlus");

        gridLayout_3->addWidget(toolButtonFocusPlus, 1, 0, 1, 1);

        toolButtonZoomPlus = new QToolButton(gridLayoutWidget_3);
        toolButtonZoomPlus->setObjectName("toolButtonZoomPlus");

        gridLayout_3->addWidget(toolButtonZoomPlus, 0, 0, 1, 1);

        toolButtonZoomMinus = new QToolButton(gridLayoutWidget_3);
        toolButtonZoomMinus->setObjectName("toolButtonZoomMinus");

        gridLayout_3->addWidget(toolButtonZoomMinus, 0, 1, 1, 1);

        switchtokeyboard = new QPushButton(gridLayoutWidget_3);
        switchtokeyboard->setObjectName("switchtokeyboard");

        gridLayout_3->addWidget(switchtokeyboard, 2, 0, 1, 3);

        focus = new QLabel(gridLayoutWidget_3);
        focus->setObjectName("focus");

        gridLayout_3->addWidget(focus, 1, 2, 1, 1);

        toolButtonFocusMinus = new QToolButton(gridLayoutWidget_3);
        toolButtonFocusMinus->setObjectName("toolButtonFocusMinus");

        gridLayout_3->addWidget(toolButtonFocusMinus, 1, 1, 1, 1);

        zoom = new QLabel(gridLayoutWidget_3);
        zoom->setObjectName("zoom");

        gridLayout_3->addWidget(zoom, 0, 2, 1, 1);

        switchtojoystick = new QPushButton(gridLayoutWidget_3);
        switchtojoystick->setObjectName("switchtojoystick");

        gridLayout_3->addWidget(switchtojoystick, 3, 0, 1, 3);

        groupBox_4 = new QGroupBox(dockWidgetContents_3);
        groupBox_4->setObjectName("groupBox_4");
        groupBox_4->setGeometry(QRect(220, 0, 201, 341));
        lineEditIP = new QLineEdit(groupBox_4);
        lineEditIP->setObjectName("lineEditIP");
        lineEditIP->setGeometry(QRect(10, 50, 181, 25));
        lineEditPort = new QLineEdit(groupBox_4);
        lineEditPort->setObjectName("lineEditPort");
        lineEditPort->setGeometry(QRect(10, 110, 171, 25));
        lineEditPath = new QLineEdit(groupBox_4);
        lineEditPath->setObjectName("lineEditPath");
        lineEditPath->setGeometry(QRect(10, 170, 181, 25));
        pushButtonSaveConfig = new QPushButton(groupBox_4);
        pushButtonSaveConfig->setObjectName("pushButtonSaveConfig");
        pushButtonSaveConfig->setGeometry(QRect(10, 230, 181, 25));
        DefaultConfig = new QPushButton(groupBox_4);
        DefaultConfig->setObjectName("DefaultConfig");
        DefaultConfig->setGeometry(QRect(10, 270, 181, 25));
        groupBox_5 = new QGroupBox(dockWidgetContents_3);
        groupBox_5->setObjectName("groupBox_5");
        groupBox_5->setGeometry(QRect(60, 510, 341, 80));
        lineEditCameraStatus = new QLineEdit(groupBox_5);
        lineEditCameraStatus->setObjectName("lineEditCameraStatus");
        lineEditCameraStatus->setGeometry(QRect(0, 20, 341, 25));
        dockWidget_3->setWidget(dockWidgetContents_3);
        MainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dockWidget_3);

        menubar->addAction(menuHexa_5->menuAction());
        menuHexa_5->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        VideoRecorderSection->setTitle(QCoreApplication::translate("MainWindow", "Video Recorder", nullptr));
        menuHexa_5->setTitle(QCoreApplication::translate("MainWindow", "Hexa 5", nullptr));
        Rescan->setText(QCoreApplication::translate("MainWindow", "Rescan", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Joysticks List", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "Axes", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Turn Right/Left", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Move Up/Down", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Zoom", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "Buttons", nullptr));
        toolButton->setText(QCoreApplication::translate("MainWindow", "Released", nullptr));
        toolButton_2->setText(QCoreApplication::translate("MainWindow", "Released", nullptr));
        gimbalControlGroupBox->setTitle(QCoreApplication::translate("MainWindow", "Gimbal Control", nullptr));
        toolButton_7->setText(QCoreApplication::translate("MainWindow", "O", nullptr));
        toolButtonLeft->setText(QCoreApplication::translate("MainWindow", "\342\227\200", nullptr));
        toolButtonRight->setText(QCoreApplication::translate("MainWindow", "\342\226\266", nullptr));
        toolButtonUp->setText(QCoreApplication::translate("MainWindow", "\342\226\262", nullptr));
        toolButtonDown->setText(QCoreApplication::translate("MainWindow", "\342\226\274", nullptr));
        toolButtonFocusPlus->setText(QCoreApplication::translate("MainWindow", "*", nullptr));
        toolButtonZoomPlus->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        toolButtonZoomMinus->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        switchtokeyboard->setText(QCoreApplication::translate("MainWindow", "Switch to Keyboard", nullptr));
        focus->setText(QCoreApplication::translate("MainWindow", "Focus", nullptr));
        toolButtonFocusMinus->setText(QCoreApplication::translate("MainWindow", "/", nullptr));
        zoom->setText(QCoreApplication::translate("MainWindow", "Zoom", nullptr));
        switchtojoystick->setText(QCoreApplication::translate("MainWindow", "Switch to Joystick", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "Change Camera Flow", nullptr));
        lineEditIP->setText(QString());
        lineEditIP->setPlaceholderText(QCoreApplication::translate("MainWindow", "Input Camera IP Address", nullptr));
        lineEditPort->setText(QString());
        lineEditPort->setPlaceholderText(QCoreApplication::translate("MainWindow", "Input Camera Port", nullptr));
        lineEditPath->setText(QString());
        lineEditPath->setPlaceholderText(QCoreApplication::translate("MainWindow", "Input Camera Path", nullptr));
        pushButtonSaveConfig->setText(QCoreApplication::translate("MainWindow", "Save Configuration", nullptr));
        DefaultConfig->setText(QCoreApplication::translate("MainWindow", "Default Configuration", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "Camera Status", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
