/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ui/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[18];
    char stringdata[458];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 24),
QT_MOC_LITERAL(2, 36, 0),
QT_MOC_LITERAL(3, 37, 35),
QT_MOC_LITERAL(4, 73, 26),
QT_MOC_LITERAL(5, 100, 29),
QT_MOC_LITERAL(6, 130, 24),
QT_MOC_LITERAL(7, 155, 23),
QT_MOC_LITERAL(8, 179, 23),
QT_MOC_LITERAL(9, 203, 30),
QT_MOC_LITERAL(10, 234, 31),
QT_MOC_LITERAL(11, 266, 27),
QT_MOC_LITERAL(12, 294, 25),
QT_MOC_LITERAL(13, 320, 31),
QT_MOC_LITERAL(14, 352, 27),
QT_MOC_LITERAL(15, 380, 29),
QT_MOC_LITERAL(16, 410, 37),
QT_MOC_LITERAL(17, 448, 8)
    },
    "MainWindow\0sensorUpdateTimerTimeout\0"
    "\0pushButton_removeRedObjects_clicked\0"
    "pushButton_Connect_clicked\0"
    "pushButton_Disconnect_clicked\0"
    "pushButton_Clean_clicked\0"
    "pushButton_Safe_clicked\0pushButton_Full_clicked\0"
    "pushButton_allMotorsOn_clicked\0"
    "pushButton_allMotorsOff_clicked\0"
    "pushButton_playSong_clicked\0"
    "pushButton_simMov_clicked\0"
    "pushButton_unshowTraces_clicked\0"
    "pushButton_mapWidth_clicked\0"
    "pushButton_resetAngle_clicked\0"
    "velocity_horizontalSlider_sliderMoved\0"
    "position\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x0a,
       3,    0,   90,    2, 0x08,
       4,    0,   91,    2, 0x08,
       5,    0,   92,    2, 0x08,
       6,    0,   93,    2, 0x08,
       7,    0,   94,    2, 0x08,
       8,    0,   95,    2, 0x08,
       9,    0,   96,    2, 0x08,
      10,    0,   97,    2, 0x08,
      11,    0,   98,    2, 0x08,
      12,    0,   99,    2, 0x08,
      13,    0,  100,    2, 0x08,
      14,    0,  101,    2, 0x08,
      15,    0,  102,    2, 0x08,
      16,    1,  103,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   17,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->sensorUpdateTimerTimeout(); break;
        case 1: _t->pushButton_removeRedObjects_clicked(); break;
        case 2: _t->pushButton_Connect_clicked(); break;
        case 3: _t->pushButton_Disconnect_clicked(); break;
        case 4: _t->pushButton_Clean_clicked(); break;
        case 5: _t->pushButton_Safe_clicked(); break;
        case 6: _t->pushButton_Full_clicked(); break;
        case 7: _t->pushButton_allMotorsOn_clicked(); break;
        case 8: _t->pushButton_allMotorsOff_clicked(); break;
        case 9: _t->pushButton_playSong_clicked(); break;
        case 10: _t->pushButton_simMov_clicked(); break;
        case 11: _t->pushButton_unshowTraces_clicked(); break;
        case 12: _t->pushButton_mapWidth_clicked(); break;
        case 13: _t->pushButton_resetAngle_clicked(); break;
        case 14: _t->velocity_horizontalSlider_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
