/****************************************************************************
** Meta object code from reading C++ file 'roowifi.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ext_lib/RooWifi_11b/roowifi.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'roowifi.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RooWifi_t {
    QByteArrayData data[10];
    char stringdata[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_RooWifi_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_RooWifi_t qt_meta_stringdata_RooWifi = {
    {
QT_MOC_LITERAL(0, 0, 7),
QT_MOC_LITERAL(1, 8, 18),
QT_MOC_LITERAL(2, 27, 0),
QT_MOC_LITERAL(3, 28, 15),
QT_MOC_LITERAL(4, 44, 28),
QT_MOC_LITERAL(5, 73, 11),
QT_MOC_LITERAL(6, 85, 9),
QT_MOC_LITERAL(7, 95, 15),
QT_MOC_LITERAL(8, 111, 18),
QT_MOC_LITERAL(9, 130, 15)
    },
    "RooWifi\0AutoCaptureUpdated\0\0ErrorConnection\0"
    "QAbstractSocket::SocketError\0socketError\0"
    "DataReady\0RooWifiResponse\0AutoCaptureProcess\0"
    "ConnectionError\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RooWifi[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x05,
       3,    1,   45,    2, 0x05,
       6,    0,   48,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       7,    0,   49,    2, 0x08,
       8,    0,   50,    2, 0x08,
       9,    1,   51,    2, 0x08,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,

       0        // eod
};

void RooWifi::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RooWifi *_t = static_cast<RooWifi *>(_o);
        switch (_id) {
        case 0: _t->AutoCaptureUpdated(); break;
        case 1: _t->ErrorConnection((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 2: _t->DataReady(); break;
        case 3: _t->RooWifiResponse(); break;
        case 4: _t->AutoCaptureProcess(); break;
        case 5: _t->ConnectionError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RooWifi::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RooWifi::AutoCaptureUpdated)) {
                *result = 0;
            }
        }
        {
            typedef void (RooWifi::*_t)(QAbstractSocket::SocketError );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RooWifi::ErrorConnection)) {
                *result = 1;
            }
        }
        {
            typedef void (RooWifi::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RooWifi::DataReady)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject RooWifi::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RooWifi.data,
      qt_meta_data_RooWifi,  qt_static_metacall, 0, 0}
};


const QMetaObject *RooWifi::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RooWifi::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RooWifi.stringdata))
        return static_cast<void*>(const_cast< RooWifi*>(this));
    return QObject::qt_metacast(_clname);
}

int RooWifi::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void RooWifi::AutoCaptureUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void RooWifi::ErrorConnection(QAbstractSocket::SocketError _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RooWifi::DataReady()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
