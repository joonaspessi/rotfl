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
    QByteArrayData data[26];
    char stringdata[605];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 11),
QT_MOC_LITERAL(2, 23, 0),
QT_MOC_LITERAL(3, 24, 35),
QT_MOC_LITERAL(4, 60, 26),
QT_MOC_LITERAL(5, 87, 29),
QT_MOC_LITERAL(6, 117, 24),
QT_MOC_LITERAL(7, 142, 23),
QT_MOC_LITERAL(8, 166, 23),
QT_MOC_LITERAL(9, 190, 30),
QT_MOC_LITERAL(10, 221, 31),
QT_MOC_LITERAL(11, 253, 27),
QT_MOC_LITERAL(12, 281, 31),
QT_MOC_LITERAL(13, 313, 27),
QT_MOC_LITERAL(14, 341, 29),
QT_MOC_LITERAL(15, 371, 37),
QT_MOC_LITERAL(16, 409, 8),
QT_MOC_LITERAL(17, 418, 25),
QT_MOC_LITERAL(18, 444, 21),
QT_MOC_LITERAL(19, 466, 12),
QT_MOC_LITERAL(20, 479, 19),
QT_MOC_LITERAL(21, 499, 18),
QT_MOC_LITERAL(22, 518, 20),
QT_MOC_LITERAL(23, 539, 20),
QT_MOC_LITERAL(24, 560, 22),
QT_MOC_LITERAL(25, 583, 20)
    },
    "MainWindow\0mapModified\0\0"
    "pushButton_removeRedObjects_clicked\0"
    "pushButton_Connect_clicked\0"
    "pushButton_Disconnect_clicked\0"
    "pushButton_Clean_clicked\0"
    "pushButton_Safe_clicked\0pushButton_Full_clicked\0"
    "pushButton_allMotorsOn_clicked\0"
    "pushButton_allMotorsOff_clicked\0"
    "pushButton_playSong_clicked\0"
    "pushButton_unshowTraces_clicked\0"
    "pushButton_mapWidth_clicked\0"
    "pushButton_resetAngle_clicked\0"
    "velocity_horizontalSlider_sliderMoved\0"
    "position\0pushButton_Go2POI_clicked\0"
    "action_Cursor_toggled\0toggleStatus\0"
    "action_Wall_toggled\0action_Poi_toggled\0"
    "action_Start_toggled\0actionSave_triggered\0"
    "actionSaveAs_triggered\0actionOpen_triggered\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  124,    2, 0x0a,
       3,    0,  125,    2, 0x08,
       4,    0,  126,    2, 0x08,
       5,    0,  127,    2, 0x08,
       6,    0,  128,    2, 0x08,
       7,    0,  129,    2, 0x08,
       8,    0,  130,    2, 0x08,
       9,    0,  131,    2, 0x08,
      10,    0,  132,    2, 0x08,
      11,    0,  133,    2, 0x08,
      12,    0,  134,    2, 0x08,
      13,    0,  135,    2, 0x08,
      14,    0,  136,    2, 0x08,
      15,    1,  137,    2, 0x08,
      17,    0,  140,    2, 0x08,
      18,    1,  141,    2, 0x08,
      20,    1,  144,    2, 0x08,
      21,    1,  147,    2, 0x08,
      22,    1,  150,    2, 0x08,
      23,    0,  153,    2, 0x08,
      24,    0,  154,    2, 0x08,
      25,    0,  155,    2, 0x08,

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
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->mapModified(); break;
        case 1: _t->pushButton_removeRedObjects_clicked(); break;
        case 2: _t->pushButton_Connect_clicked(); break;
        case 3: _t->pushButton_Disconnect_clicked(); break;
        case 4: _t->pushButton_Clean_clicked(); break;
        case 5: _t->pushButton_Safe_clicked(); break;
        case 6: _t->pushButton_Full_clicked(); break;
        case 7: _t->pushButton_allMotorsOn_clicked(); break;
        case 8: _t->pushButton_allMotorsOff_clicked(); break;
        case 9: _t->pushButton_playSong_clicked(); break;
        case 10: _t->pushButton_unshowTraces_clicked(); break;
        case 11: _t->pushButton_mapWidth_clicked(); break;
        case 12: _t->pushButton_resetAngle_clicked(); break;
        case 13: _t->velocity_horizontalSlider_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->pushButton_Go2POI_clicked(); break;
        case 15: _t->action_Cursor_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: _t->action_Wall_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->action_Poi_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->action_Start_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: _t->actionSave_triggered(); break;
        case 20: _t->actionSaveAs_triggered(); break;
        case 21: _t->actionOpen_triggered(); break;
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
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
