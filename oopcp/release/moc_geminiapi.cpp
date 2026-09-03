/****************************************************************************
** Meta object code from reading C++ file 'geminiapi.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/geminiapi.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'geminiapi.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.0. It"
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
struct qt_meta_tag_ZN9GeminiAPIE_t {};
} // unnamed namespace

template <> constexpr inline auto GeminiAPI::qt_create_metaobjectdata<qt_meta_tag_ZN9GeminiAPIE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "GeminiAPI",
        "analysisComplete",
        "",
        "QList<ErrorInfo>",
        "errors",
        "analysisError",
        "errorMessage",
        "explanationComplete",
        "explanation",
        "complexityAnalysisComplete",
        "response",
        "debugExplanationReady",
        "refactorComplete",
        "refactoredCode",
        "analyzeCode",
        "code",
        "explainCode",
        "analyzeComplexity",
        "prompt",
        "explainDebugStep",
        "requestRefactor",
        "onReplyFinished"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'analysisComplete'
        QtMocHelpers::SignalData<void(const QList<ErrorInfo> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'analysisError'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'explanationComplete'
        QtMocHelpers::SignalData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'complexityAnalysisComplete'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'debugExplanationReady'
        QtMocHelpers::SignalData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'refactorComplete'
        QtMocHelpers::SignalData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'analyzeCode'
        QtMocHelpers::SlotData<void(const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Slot 'explainCode'
        QtMocHelpers::SlotData<void(const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Slot 'analyzeComplexity'
        QtMocHelpers::SlotData<void(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Slot 'explainDebugStep'
        QtMocHelpers::SlotData<void(const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Slot 'requestRefactor'
        QtMocHelpers::SlotData<void(const QString &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Slot 'onReplyFinished'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<GeminiAPI, qt_meta_tag_ZN9GeminiAPIE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject GeminiAPI::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9GeminiAPIE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9GeminiAPIE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9GeminiAPIE_t>.metaTypes,
    nullptr
} };

void GeminiAPI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<GeminiAPI *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->analysisComplete((*reinterpret_cast<std::add_pointer_t<QList<ErrorInfo>>>(_a[1]))); break;
        case 1: _t->analysisError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->explanationComplete((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->complexityAnalysisComplete((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->debugExplanationReady((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->refactorComplete((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->analyzeCode((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->explainCode((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->analyzeComplexity((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->explainDebugStep((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->requestRefactor((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onReplyFinished(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (GeminiAPI::*)(const QList<ErrorInfo> & )>(_a, &GeminiAPI::analysisComplete, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (GeminiAPI::*)(const QString & )>(_a, &GeminiAPI::analysisError, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (GeminiAPI::*)(const QString & )>(_a, &GeminiAPI::explanationComplete, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (GeminiAPI::*)(const QString & )>(_a, &GeminiAPI::complexityAnalysisComplete, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (GeminiAPI::*)(const QString & )>(_a, &GeminiAPI::debugExplanationReady, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (GeminiAPI::*)(const QString & )>(_a, &GeminiAPI::refactorComplete, 5))
            return;
    }
}

const QMetaObject *GeminiAPI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GeminiAPI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9GeminiAPIE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GeminiAPI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void GeminiAPI::analysisComplete(const QList<ErrorInfo> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void GeminiAPI::analysisError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void GeminiAPI::explanationComplete(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void GeminiAPI::complexityAnalysisComplete(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void GeminiAPI::debugExplanationReady(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void GeminiAPI::refactorComplete(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
