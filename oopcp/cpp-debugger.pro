QT += core gui widgets network

CONFIG += c++17
CONFIG -= app_bundle

TARGET = cpp-debugger
TEMPLATE = app

INCLUDEPATH += include

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/codeeditor.cpp \
    src/geminiapi.cpp \
    src/complexityanalyzer.cpp

HEADERS += \
    include/mainwindow.h \
    include/codeeditor.h \
    include/geminiapi.h \
    include/complexityanalyzer.h

# Windows specific
win32 {
    CONFIG += console
}