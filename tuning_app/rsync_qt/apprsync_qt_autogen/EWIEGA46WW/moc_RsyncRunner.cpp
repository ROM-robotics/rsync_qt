/****************************************************************************
** Meta object code from reading C++ file 'RsyncRunner.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../RsyncRunner.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RsyncRunner.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.3. It"
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
struct qt_meta_tag_ZN11RsyncRunnerE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN11RsyncRunnerE = QtMocHelpers::stringData(
    "RsyncRunner",
    "logsChanged",
    "",
    "remoteDestPathChanged",
    "defaultUserChanged",
    "sourceRootChanged",
    "statusChanged",
    "finished",
    "exitCode",
    "clearLogs",
    "runRemoteCommand",
    "host",
    "password",
    "remotePath",
    "command",
    "openTerminalSsh",
    "openLocalTerminal",
    "run",
    "items",
    "excludes",
    "logs",
    "remoteDestPath",
    "defaultUser",
    "sourceRoot",
    "status",
    "statusColor"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN11RsyncRunnerE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       6,  113, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x06,    7 /* Public */,
       3,    0,   81,    2, 0x06,    8 /* Public */,
       4,    0,   82,    2, 0x06,    9 /* Public */,
       5,    0,   83,    2, 0x06,   10 /* Public */,
       6,    0,   84,    2, 0x06,   11 /* Public */,
       7,    1,   85,    2, 0x06,   12 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,   88,    2, 0x02,   14 /* Public */,
      10,    4,   89,    2, 0x02,   15 /* Public */,
      15,    2,   98,    2, 0x02,   20 /* Public */,
      16,    0,  103,    2, 0x02,   23 /* Public */,
      17,    4,  104,    2, 0x02,   24 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   11,   12,   13,   14,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   11,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QStringList, QMetaType::QStringList,   11,   12,   18,   19,

 // properties: name, type, flags, notifyId, revision
      20, QMetaType::QString, 0x00015001, uint(0), 0,
      21, QMetaType::QString, 0x00015103, uint(1), 0,
      22, QMetaType::QString, 0x00015103, uint(2), 0,
      23, QMetaType::QString, 0x00015103, uint(3), 0,
      24, QMetaType::QString, 0x00015001, uint(4), 0,
      25, QMetaType::QString, 0x00015001, uint(4), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject RsyncRunner::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN11RsyncRunnerE.offsetsAndSizes,
    qt_meta_data_ZN11RsyncRunnerE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN11RsyncRunnerE_t,
        // property 'logs'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'remoteDestPath'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'defaultUser'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'sourceRoot'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'status'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'statusColor'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RsyncRunner, std::true_type>,
        // method 'logsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remoteDestPathChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'defaultUserChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sourceRootChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'statusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'clearLogs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'runRemoteCommand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'openTerminalSsh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'openLocalTerminal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'run'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>
    >,
    nullptr
} };

void RsyncRunner::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RsyncRunner *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->logsChanged(); break;
        case 1: _t->remoteDestPathChanged(); break;
        case 2: _t->defaultUserChanged(); break;
        case 3: _t->sourceRootChanged(); break;
        case 4: _t->statusChanged(); break;
        case 5: _t->finished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->clearLogs(); break;
        case 7: _t->runRemoteCommand((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4]))); break;
        case 8: _t->openTerminalSsh((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->openLocalTerminal(); break;
        case 10: _t->run((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[4]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (RsyncRunner::*)();
            if (_q_method_type _q_method = &RsyncRunner::logsChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (RsyncRunner::*)();
            if (_q_method_type _q_method = &RsyncRunner::remoteDestPathChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (RsyncRunner::*)();
            if (_q_method_type _q_method = &RsyncRunner::defaultUserChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (RsyncRunner::*)();
            if (_q_method_type _q_method = &RsyncRunner::sourceRootChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (RsyncRunner::*)();
            if (_q_method_type _q_method = &RsyncRunner::statusChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _q_method_type = void (RsyncRunner::*)(int );
            if (_q_method_type _q_method = &RsyncRunner::finished; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->logs(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->remoteDestPath(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->defaultUser(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->sourceRoot(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->status(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->statusColor(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setRemoteDestPath(*reinterpret_cast< QString*>(_v)); break;
        case 2: _t->setDefaultUser(*reinterpret_cast< QString*>(_v)); break;
        case 3: _t->setSourceRoot(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *RsyncRunner::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RsyncRunner::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN11RsyncRunnerE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RsyncRunner::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void RsyncRunner::logsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RsyncRunner::remoteDestPathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RsyncRunner::defaultUserChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void RsyncRunner::sourceRootChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void RsyncRunner::statusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void RsyncRunner::finished(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
