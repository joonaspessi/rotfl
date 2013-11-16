/****************************************************************************
** Meta object code from reading C++ file 'iRoomba.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../croi/iRoomba.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'iRoomba.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Croi__IRoomba_t {
    QByteArrayData data[3];
    char stringdata[41];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_Croi__IRoomba_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_Croi__IRoomba_t qt_meta_stringdata_Croi__IRoomba = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 24),
QT_MOC_LITERAL(2, 39, 0)
    },
    "Croi::IRoomba\0sensorUpdateTimerTimeout\0"
    "\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Croi__IRoomba[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Croi::IRoomba::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        IRoomba *_t = static_cast<IRoomba *>(_o);
        switch (_id) {
        case 0: _t->sensorUpdateTimerTimeout(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Croi::IRoomba::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Croi__IRoomba.data,
      qt_meta_data_Croi__IRoomba,  qt_static_metacall, 0, 0}
};


const QMetaObject *Croi::IRoomba::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Croi::IRoomba::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Croi__IRoomba.stringdata))
        return static_cast<void*>(const_cast< IRoomba*>(this));
    return QObject::qt_metacast(_clname);
}

int Croi::IRoomba::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
