/****************************************************************************
** Meta object code from reading C++ file 'WebKit.hpp'
**
** Created: Sat Feb 2 14:17:07 2013
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/client/WebKit.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WebKit.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WebKit[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,    8,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WebKit[] = {
    "WebKit\0\0url\0setUrl(QString)\0"
};

const QMetaObject WebKit::staticMetaObject = {
    { &QWebView::staticMetaObject, qt_meta_stringdata_WebKit,
      qt_meta_data_WebKit, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WebKit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WebKit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WebKit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WebKit))
        return static_cast<void*>(const_cast< WebKit*>(this));
    return QWebView::qt_metacast(_clname);
}

int WebKit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setUrl((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
