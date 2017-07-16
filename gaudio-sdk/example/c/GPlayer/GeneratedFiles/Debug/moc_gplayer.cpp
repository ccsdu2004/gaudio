/****************************************************************************
** Meta object code from reading C++ file 'gplayer.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../gplayer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gplayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GPlayer[] = {

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
      14,    9,    8,    8, 0x08,
      28,    8,    8,    8, 0x08,
      38,    8,    8,    8, 0x08,
      48,    8,    8,    8, 0x08,
      62,    8,    8,    8, 0x08,
      73,    8,    8,    8, 0x08,
      89,    8,    8,    8, 0x08,
     106,    8,    8,    8, 0x08,
     127,    8,    8,    8, 0x08,
     148,    8,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GPlayer[] = {
    "GPlayer\0\0file\0open(QString)\0onAbout()\0"
    "onPause()\0onVolume(int)\0onPan(int)\0"
    "onSurround(int)\0onUseBassboost()\0"
    "onBassboostGain(int)\0onBassboostFreq(int)\0"
    "onSpeedChanger(int)\0"
};

void GPlayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GPlayer *_t = static_cast<GPlayer *>(_o);
        switch (_id) {
        case 0: _t->open((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->onAbout(); break;
        case 2: _t->onPause(); break;
        case 3: _t->onVolume((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onPan((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->onSurround((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->onUseBassboost(); break;
        case 7: _t->onBassboostGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->onBassboostFreq((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->onSpeedChanger((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GPlayer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GPlayer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GPlayer,
      qt_meta_data_GPlayer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GPlayer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GPlayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GPlayer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GPlayer))
        return static_cast<void*>(const_cast< GPlayer*>(this));
    return QWidget::qt_metacast(_clname);
}

int GPlayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
