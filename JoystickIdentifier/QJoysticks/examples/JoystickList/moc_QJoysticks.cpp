/****************************************************************************
** Meta object code from reading C++ file 'QJoysticks.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/QJoysticks.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QJoysticks.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10QJoysticksE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN10QJoysticksE = QtMocHelpers::stringData(
    "QJoysticks",
    "countChanged",
    "",
    "enabledChanged",
    "enabled",
    "POVEvent",
    "QJoystickPOVEvent",
    "event",
    "axisEvent",
    "QJoystickAxisEvent",
    "buttonEvent",
    "QJoystickButtonEvent",
    "povChanged",
    "js",
    "pov",
    "angle",
    "axisChanged",
    "axis",
    "value",
    "buttonChanged",
    "button",
    "pressed",
    "updateInterfaces",
    "setVirtualJoystickRange",
    "range",
    "setVirtualJoystickEnabled",
    "setVirtualJoystickAxisSensibility",
    "sensibility",
    "setSortJoysticksByBlacklistState",
    "sort",
    "setBlacklisted",
    "index",
    "blacklisted",
    "resetJoysticks",
    "addInputDevice",
    "QJoystickDevice*",
    "device",
    "onPOVEvent",
    "e",
    "onAxisEvent",
    "onButtonEvent",
    "getPOV",
    "getAxis",
    "getButton",
    "getNumAxes",
    "getNumPOVs",
    "getNumButtons",
    "isBlacklisted",
    "joystickExists",
    "getName",
    "count",
    "nonBlacklistedCount",
    "deviceNames"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN10QJoysticksE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   14, // methods
       3,  280, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  182,    2, 0x06,    4 /* Public */,
       3,    1,  183,    2, 0x06,    5 /* Public */,
       5,    1,  186,    2, 0x06,    7 /* Public */,
       8,    1,  189,    2, 0x06,    9 /* Public */,
      10,    1,  192,    2, 0x06,   11 /* Public */,
      12,    3,  195,    2, 0x06,   13 /* Public */,
      16,    3,  202,    2, 0x06,   17 /* Public */,
      19,    3,  209,    2, 0x06,   21 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      22,    0,  216,    2, 0x0a,   25 /* Public */,
      23,    1,  217,    2, 0x0a,   26 /* Public */,
      25,    1,  220,    2, 0x0a,   28 /* Public */,
      26,    1,  223,    2, 0x0a,   30 /* Public */,
      28,    1,  226,    2, 0x0a,   32 /* Public */,
      30,    2,  229,    2, 0x0a,   34 /* Public */,
      33,    0,  234,    2, 0x08,   37 /* Private */,
      34,    1,  235,    2, 0x08,   38 /* Private */,
      37,    1,  238,    2, 0x08,   40 /* Private */,
      39,    1,  241,    2, 0x08,   42 /* Private */,
      40,    1,  244,    2, 0x08,   44 /* Private */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      41,    2,  247,    2, 0x02,   46 /* Public */,
      42,    2,  252,    2, 0x02,   49 /* Public */,
      43,    2,  257,    2, 0x02,   52 /* Public */,
      44,    1,  262,    2, 0x02,   55 /* Public */,
      45,    1,  265,    2, 0x02,   57 /* Public */,
      46,    1,  268,    2, 0x02,   59 /* Public */,
      47,    1,  271,    2, 0x02,   61 /* Public */,
      48,    1,  274,    2, 0x02,   63 /* Public */,
      49,    1,  277,    2, 0x02,   65 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9,    7,
    QMetaType::Void, 0x80000000 | 11,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   13,   14,   15,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QReal,   13,   17,   18,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool,   13,   20,   21,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QReal,   24,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::QReal,   27,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   31,   32,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 35,   36,
    QMetaType::Void, 0x80000000 | 6,   38,
    QMetaType::Void, 0x80000000 | 9,   38,
    QMetaType::Void, 0x80000000 | 11,   38,

 // methods: parameters
    QMetaType::Int, QMetaType::Int, QMetaType::Int,   31,   14,
    QMetaType::Double, QMetaType::Int, QMetaType::Int,   31,   17,
    QMetaType::Bool, QMetaType::Int, QMetaType::Int,   31,   20,
    QMetaType::Int, QMetaType::Int,   31,
    QMetaType::Int, QMetaType::Int,   31,
    QMetaType::Int, QMetaType::Int,   31,
    QMetaType::Bool, QMetaType::Int,   31,
    QMetaType::Bool, QMetaType::Int,   31,
    QMetaType::QString, QMetaType::Int,   31,

 // properties: name, type, flags, notifyId, revision
      50, QMetaType::Int, 0x00015001, uint(0), 0,
      51, QMetaType::Int, 0x00015001, uint(0), 0,
      52, QMetaType::QStringList, 0x00015001, uint(0), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject QJoysticks::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN10QJoysticksE.offsetsAndSizes,
    qt_meta_data_ZN10QJoysticksE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN10QJoysticksE_t,
        // property 'count'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'nonBlacklistedCount'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'deviceNames'
        QtPrivate::TypeAndForceComplete<QStringList, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<QJoysticks, std::true_type>,
        // method 'countChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'enabledChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'POVEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJoystickPOVEvent &, std::false_type>,
        // method 'axisEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJoystickAxisEvent &, std::false_type>,
        // method 'buttonEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJoystickButtonEvent &, std::false_type>,
        // method 'povChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        // method 'axisChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const qreal, std::false_type>,
        // method 'buttonChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'updateInterfaces'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setVirtualJoystickRange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        // method 'setVirtualJoystickEnabled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setVirtualJoystickAxisSensibility'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        // method 'setSortJoysticksByBlacklistState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setBlacklisted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'resetJoysticks'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addInputDevice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QJoystickDevice *, std::false_type>,
        // method 'onPOVEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJoystickPOVEvent &, std::false_type>,
        // method 'onAxisEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJoystickAxisEvent &, std::false_type>,
        // method 'onButtonEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJoystickButtonEvent &, std::false_type>,
        // method 'getPOV'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        // method 'getAxis'
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        // method 'getButton'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        // method 'getNumAxes'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        // method 'getNumPOVs'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        // method 'getNumButtons'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        // method 'isBlacklisted'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        // method 'joystickExists'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>,
        // method 'getName'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int, std::false_type>
    >,
    nullptr
} };

void QJoysticks::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QJoysticks *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->countChanged(); break;
        case 1: _t->enabledChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->POVEvent((*reinterpret_cast< std::add_pointer_t<QJoystickPOVEvent>>(_a[1]))); break;
        case 3: _t->axisEvent((*reinterpret_cast< std::add_pointer_t<QJoystickAxisEvent>>(_a[1]))); break;
        case 4: _t->buttonEvent((*reinterpret_cast< std::add_pointer_t<QJoystickButtonEvent>>(_a[1]))); break;
        case 5: _t->povChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 6: _t->axisChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<qreal>>(_a[3]))); break;
        case 7: _t->buttonChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 8: _t->updateInterfaces(); break;
        case 9: _t->setVirtualJoystickRange((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1]))); break;
        case 10: _t->setVirtualJoystickEnabled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->setVirtualJoystickAxisSensibility((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1]))); break;
        case 12: _t->setSortJoysticksByBlacklistState((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: _t->setBlacklisted((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 14: _t->resetJoysticks(); break;
        case 15: _t->addInputDevice((*reinterpret_cast< std::add_pointer_t<QJoystickDevice*>>(_a[1]))); break;
        case 16: _t->onPOVEvent((*reinterpret_cast< std::add_pointer_t<QJoystickPOVEvent>>(_a[1]))); break;
        case 17: _t->onAxisEvent((*reinterpret_cast< std::add_pointer_t<QJoystickAxisEvent>>(_a[1]))); break;
        case 18: _t->onButtonEvent((*reinterpret_cast< std::add_pointer_t<QJoystickButtonEvent>>(_a[1]))); break;
        case 19: { int _r = _t->getPOV((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 20: { double _r = _t->getAxis((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 21: { bool _r = _t->getButton((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 22: { int _r = _t->getNumAxes((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 23: { int _r = _t->getNumPOVs((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 24: { int _r = _t->getNumButtons((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 25: { bool _r = _t->isBlacklisted((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 26: { bool _r = _t->joystickExists((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 27: { QString _r = _t->getName((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (QJoysticks::*)();
            if (_q_method_type _q_method = &QJoysticks::countChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (QJoysticks::*)(const bool );
            if (_q_method_type _q_method = &QJoysticks::enabledChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (QJoysticks::*)(const QJoystickPOVEvent & );
            if (_q_method_type _q_method = &QJoysticks::POVEvent; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (QJoysticks::*)(const QJoystickAxisEvent & );
            if (_q_method_type _q_method = &QJoysticks::axisEvent; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (QJoysticks::*)(const QJoystickButtonEvent & );
            if (_q_method_type _q_method = &QJoysticks::buttonEvent; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _q_method_type = void (QJoysticks::*)(const int , const int , const int );
            if (_q_method_type _q_method = &QJoysticks::povChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _q_method_type = void (QJoysticks::*)(const int , const int , const qreal );
            if (_q_method_type _q_method = &QJoysticks::axisChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _q_method_type = void (QJoysticks::*)(const int , const int , const bool );
            if (_q_method_type _q_method = &QJoysticks::buttonChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->count(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->nonBlacklistedCount(); break;
        case 2: *reinterpret_cast< QStringList*>(_v) = _t->deviceNames(); break;
        default: break;
        }
    }
}

const QMetaObject *QJoysticks::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QJoysticks::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN10QJoysticksE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QJoysticks::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 28)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 28;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void QJoysticks::countChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void QJoysticks::enabledChanged(const bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QJoysticks::POVEvent(const QJoystickPOVEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QJoysticks::axisEvent(const QJoystickAxisEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QJoysticks::buttonEvent(const QJoystickButtonEvent & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QJoysticks::povChanged(const int _t1, const int _t2, const int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QJoysticks::axisChanged(const int _t1, const int _t2, const qreal _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QJoysticks::buttonChanged(const int _t1, const int _t2, const bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
