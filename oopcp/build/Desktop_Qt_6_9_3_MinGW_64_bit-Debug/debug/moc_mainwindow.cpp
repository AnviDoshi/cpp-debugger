/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
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
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "onProcessOutput",
        "",
        "onProcessFinished",
        "exitCode",
        "onAnalyzeClicked",
        "onAnalysisComplete",
        "QList<ErrorInfo>",
        "errors",
        "onAnalysisError",
        "errorMessage",
        "toggleTheme",
        "updateStatusBar",
        "newFile",
        "openFile",
        "saveFile",
        "onRunClicked",
        "updateCompilerStatus",
        "status",
        "updateVariablePanel",
        "QList<VariableInfo>",
        "variables",
        "onStepForward",
        "onStepBackward",
        "onStepExecution",
        "step",
        "onBreakpointToggled",
        "line",
        "loadExecutionTimeline",
        "QList<ExecutionStep>",
        "timeline",
        "applyTheme",
        "index",
        "runUserCode",
        "exePath",
        "onAnalyzeComplexityClicked",
        "onAIComplexityResponse",
        "response",
        "onGdbOutput",
        "onGdbInputEntered"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onProcessOutput'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onProcessFinished'
        QtMocHelpers::SlotData<void(int)>(3, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Slot 'onAnalyzeClicked'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAnalysisComplete'
        QtMocHelpers::SlotData<void(const QList<ErrorInfo> &)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Slot 'onAnalysisError'
        QtMocHelpers::SlotData<void(const QString &)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Slot 'toggleTheme'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateStatusBar'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'newFile'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'openFile'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'saveFile'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRunClicked'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateCompilerStatus'
        QtMocHelpers::SlotData<void(const QString &)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Slot 'updateVariablePanel'
        QtMocHelpers::SlotData<void(const QList<VariableInfo> &)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 20, 21 },
        }}),
        // Slot 'onStepForward'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStepBackward'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStepExecution'
        QtMocHelpers::SlotData<void(int)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 25 },
        }}),
        // Slot 'onBreakpointToggled'
        QtMocHelpers::SlotData<void(int)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 27 },
        }}),
        // Slot 'loadExecutionTimeline'
        QtMocHelpers::SlotData<void(const QList<ExecutionStep> &)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 29, 30 },
        }}),
        // Slot 'applyTheme'
        QtMocHelpers::SlotData<void(int)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 32 },
        }}),
        // Slot 'runUserCode'
        QtMocHelpers::SlotData<void(const QString &)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 34 },
        }}),
        // Slot 'onAnalyzeComplexityClicked'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAIComplexityResponse'
        QtMocHelpers::SlotData<void(const QString &)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 37 },
        }}),
        // Slot 'onGdbOutput'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGdbInputEntered'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onProcessOutput(); break;
        case 1: _t->onProcessFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->onAnalyzeClicked(); break;
        case 3: _t->onAnalysisComplete((*reinterpret_cast< std::add_pointer_t<QList<ErrorInfo>>>(_a[1]))); break;
        case 4: _t->onAnalysisError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->toggleTheme(); break;
        case 6: _t->updateStatusBar(); break;
        case 7: _t->newFile(); break;
        case 8: _t->openFile(); break;
        case 9: _t->saveFile(); break;
        case 10: _t->onRunClicked(); break;
        case 11: _t->updateCompilerStatus((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->updateVariablePanel((*reinterpret_cast< std::add_pointer_t<QList<VariableInfo>>>(_a[1]))); break;
        case 13: _t->onStepForward(); break;
        case 14: _t->onStepBackward(); break;
        case 15: _t->onStepExecution((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->onBreakpointToggled((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->loadExecutionTimeline((*reinterpret_cast< std::add_pointer_t<QList<ExecutionStep>>>(_a[1]))); break;
        case 18: _t->applyTheme((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 19: _t->runUserCode((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->onAnalyzeComplexityClicked(); break;
        case 21: _t->onAIComplexityResponse((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->onGdbOutput(); break;
        case 23: _t->onGdbInputEntered(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 24;
    }
    return _id;
}
QT_WARNING_POP
