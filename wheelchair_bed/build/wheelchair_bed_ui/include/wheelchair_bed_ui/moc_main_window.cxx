/****************************************************************************
** Meta object code from reading C++ file 'main_window.hpp'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/wheelchair_bed_ui/include/wheelchair_bed_ui/main_window.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_window.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_wheelchair_bed_ui__MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      37,   31,   30,   30, 0x0a,
      69,   31,   30,   30, 0x0a,
     103,   98,   30,   30, 0x0a,
     144,  134,   30,   30, 0x0a,
     179,   30,   30,   30, 0x0a,
     191,   30,   30,   30, 0x0a,
     202,   30,   30,   30, 0x0a,
     211,   30,   30,   30, 0x0a,
     222,   30,   30,   30, 0x0a,
     233,   30,   30,   30, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_wheelchair_bed_ui__MainWindow[] = {
    "wheelchair_bed_ui::MainWindow\0\0check\0"
    "on_button_connect_clicked(bool)\0"
    "on_quit_button_clicked(bool)\0data\0"
    "updateLoggingView_pub(QString)\0temp,humi\0"
    "updateLoggingView_sub(float,float)\0"
    "start_cmd()\0stop_cmd()\0up_cmd()\0"
    "down_cmd()\0left_cmd()\0right_cmd()\0"
};

void wheelchair_bed_ui::MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->on_button_connect_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->on_quit_button_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->updateLoggingView_pub((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->updateLoggingView_sub((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 4: _t->start_cmd(); break;
        case 5: _t->stop_cmd(); break;
        case 6: _t->up_cmd(); break;
        case 7: _t->down_cmd(); break;
        case 8: _t->left_cmd(); break;
        case 9: _t->right_cmd(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData wheelchair_bed_ui::MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject wheelchair_bed_ui::MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_wheelchair_bed_ui__MainWindow,
      qt_meta_data_wheelchair_bed_ui__MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &wheelchair_bed_ui::MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *wheelchair_bed_ui::MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *wheelchair_bed_ui::MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_wheelchair_bed_ui__MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int wheelchair_bed_ui::MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
