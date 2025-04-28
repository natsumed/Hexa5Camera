/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    const uint offsetsAndSize[52];
    char stringdata0[332];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 10), // "MainWindow"
QT_MOC_LITERAL(11, 16), // "updateDeviceList"
QT_MOC_LITERAL(28, 0), // ""
QT_MOC_LITERAL(29, 14), // "pollAxisValues"
QT_MOC_LITERAL(44, 16), // "updateAxisValues"
QT_MOC_LITERAL(61, 2), // "js"
QT_MOC_LITERAL(64, 4), // "axis"
QT_MOC_LITERAL(69, 5), // "value"
QT_MOC_LITERAL(75, 17), // "updateButtonState"
QT_MOC_LITERAL(93, 6), // "button"
QT_MOC_LITERAL(100, 7), // "pressed"
QT_MOC_LITERAL(108, 21), // "onJoystickItemClicked"
QT_MOC_LITERAL(130, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(147, 4), // "item"
QT_MOC_LITERAL(152, 18), // "onSwitchToKeyboard"
QT_MOC_LITERAL(171, 18), // "onSwitchToJoystick"
QT_MOC_LITERAL(190, 18), // "sendGimbalCommands"
QT_MOC_LITERAL(209, 10), // "saveConfig"
QT_MOC_LITERAL(220, 17), // "saveDefaultConfig"
QT_MOC_LITERAL(238, 21), // "handleCommandFeedback"
QT_MOC_LITERAL(260, 9), // "commandId"
QT_MOC_LITERAL(270, 7), // "success"
QT_MOC_LITERAL(278, 15), // "onCameraStarted"
QT_MOC_LITERAL(294, 13), // "onCameraError"
QT_MOC_LITERAL(308, 3), // "msg"
QT_MOC_LITERAL(312, 19) // "refreshCameraStatus"

    },
    "MainWindow\0updateDeviceList\0\0"
    "pollAxisValues\0updateAxisValues\0js\0"
    "axis\0value\0updateButtonState\0button\0"
    "pressed\0onJoystickItemClicked\0"
    "QListWidgetItem*\0item\0onSwitchToKeyboard\0"
    "onSwitchToJoystick\0sendGimbalCommands\0"
    "saveConfig\0saveDefaultConfig\0"
    "handleCommandFeedback\0commandId\0success\0"
    "onCameraStarted\0onCameraError\0msg\0"
    "refreshCameraStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   98,    2, 0x08,    1 /* Private */,
       3,    0,   99,    2, 0x08,    2 /* Private */,
       4,    3,  100,    2, 0x08,    3 /* Private */,
       8,    3,  107,    2, 0x08,    7 /* Private */,
      11,    1,  114,    2, 0x08,   11 /* Private */,
      14,    0,  117,    2, 0x08,   13 /* Private */,
      15,    0,  118,    2, 0x08,   14 /* Private */,
      16,    0,  119,    2, 0x08,   15 /* Private */,
      17,    0,  120,    2, 0x08,   16 /* Private */,
      18,    0,  121,    2, 0x08,   17 /* Private */,
      19,    2,  122,    2, 0x08,   18 /* Private */,
      22,    0,  127,    2, 0x08,   21 /* Private */,
      23,    1,  128,    2, 0x08,   22 /* Private */,
      25,    0,  131,    2, 0x08,   24 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QReal,    5,    6,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool,    5,    9,   10,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   20,   21,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   24,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateDeviceList(); break;
        case 1: _t->pollAxisValues(); break;
        case 2: _t->updateAxisValues((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<qreal>>(_a[3]))); break;
        case 3: _t->updateButtonState((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 4: _t->onJoystickItemClicked((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 5: _t->onSwitchToKeyboard(); break;
        case 6: _t->onSwitchToJoystick(); break;
        case 7: _t->sendGimbalCommands(); break;
        case 8: _t->saveConfig(); break;
        case 9: _t->saveDefaultConfig(); break;
        case 10: _t->handleCommandFeedback((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 11: _t->onCameraStarted(); break;
        case 12: _t->onCameraError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->refreshCameraStatus(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.offsetsAndSize,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MainWindow_t
, QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<qreal, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
