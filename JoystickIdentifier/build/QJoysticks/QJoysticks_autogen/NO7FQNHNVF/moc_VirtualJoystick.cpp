/****************************************************************************
** Meta object code from reading C++ file 'VirtualJoystick.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../QJoysticks/src/QJoysticks/VirtualJoystick.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VirtualJoystick.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN15VirtualJoystickE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN15VirtualJoystickE = QtMocHelpers::stringData(
    "VirtualJoystick",
    "enabledChanged",
    "",
    "povEvent",
    "QJoystickPOVEvent",
    "event",
    "axisEvent",
    "QJoystickAxisEvent",
    "buttonEvent",
    "QJoystickButtonEvent",
    "setJoystickID",
    "id",
    "setAxisRange",
    "range",
    "setJoystickEnabled",
    "enabled",
    "setAxisSensibility",
    "sensibility",
    "readAxes",
    "key",
    "pressed",
    "updateAxis",
    "readPOVs",
    "readButtons",
    "processKeyEvent",
    "QKeyEvent*"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN15VirtualJoystickE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x06,    1 /* Public */,
       3,    1,   93,    2, 0x06,    2 /* Public */,
       6,    1,   96,    2, 0x06,    4 /* Public */,
       8,    1,   99,    2, 0x06,    6 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      10,    1,  102,    2, 0x0a,    8 /* Public */,
      12,    1,  105,    2, 0x0a,   10 /* Public */,
      14,    1,  108,    2, 0x0a,   12 /* Public */,
      16,    1,  111,    2, 0x0a,   14 /* Public */,
      18,    2,  114,    2, 0x08,   16 /* Private */,
      21,    0,  119,    2, 0x08,   19 /* Private */,
      22,    2,  120,    2, 0x08,   20 /* Private */,
      23,    2,  125,    2, 0x08,   23 /* Private */,
      24,    2,  130,    2, 0x08,   26 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 7,    5,
    QMetaType::Void, 0x80000000 | 9,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::QReal,   13,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::QReal,   17,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   19,   20,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   19,   20,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   19,   20,
    QMetaType::Void, 0x80000000 | 25, QMetaType::Bool,    5,   20,

       0        // eod
};

Q_CONSTINIT const QMetaObject VirtualJoystick::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN15VirtualJoystickE.offsetsAndSizes,
    qt_meta_data_ZN15VirtualJoystickE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN15VirtualJoystickE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<VirtualJoystick, std::true_type>,
        // method 'enabledChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'povEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJoystickPOVEvent &, std::false_type>,
        // method 'axisEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJoystickAxisEvent &, std::false_type>,
        // method 'buttonEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJoystickButtonEvent &, std::false_type>,
        // method 'setJoystickID'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setAxisRange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        // method 'setJoystickEnabled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setAxisSensibility'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        // method 'readAxes'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'updateAxis'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readPOVs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'readButtons'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'processKeyEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QKeyEvent *, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void VirtualJoystick::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<VirtualJoystick *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->enabledChanged(); break;
        case 1: _t->povEvent((*reinterpret_cast< std::add_pointer_t<QJoystickPOVEvent>>(_a[1]))); break;
        case 2: _t->axisEvent((*reinterpret_cast< std::add_pointer_t<QJoystickAxisEvent>>(_a[1]))); break;
        case 3: _t->buttonEvent((*reinterpret_cast< std::add_pointer_t<QJoystickButtonEvent>>(_a[1]))); break;
        case 4: _t->setJoystickID((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->setAxisRange((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1]))); break;
        case 6: _t->setJoystickEnabled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 7: _t->setAxisSensibility((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1]))); break;
        case 8: _t->readAxes((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 9: _t->updateAxis(); break;
        case 10: _t->readPOVs((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 11: _t->readButtons((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 12: _t->processKeyEvent((*reinterpret_cast< std::add_pointer_t<QKeyEvent*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (VirtualJoystick::*)();
            if (_q_method_type _q_method = &VirtualJoystick::enabledChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (VirtualJoystick::*)(const QJoystickPOVEvent & );
            if (_q_method_type _q_method = &VirtualJoystick::povEvent; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (VirtualJoystick::*)(const QJoystickAxisEvent & );
            if (_q_method_type _q_method = &VirtualJoystick::axisEvent; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (VirtualJoystick::*)(const QJoystickButtonEvent & );
            if (_q_method_type _q_method = &VirtualJoystick::buttonEvent; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *VirtualJoystick::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VirtualJoystick::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN15VirtualJoystickE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int VirtualJoystick::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void VirtualJoystick::enabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void VirtualJoystick::povEvent(const QJoystickPOVEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VirtualJoystick::axisEvent(const QJoystickAxisEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void VirtualJoystick::buttonEvent(const QJoystickButtonEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
