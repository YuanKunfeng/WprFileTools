#-------------------------------------------------
#
# Project created by QtCreator 2015-11-10T14:13:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileSaveAs
TEMPLATE = app

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    BIN_ROOT = $$PWD/../../bin/debug
    BUILD_ROOT = $$PWD/../../tmp/fsa/debug
}else{
    BIN_ROOT = $$PWD/../../bin/release
    BUILD_ROOT = $$PWD/../../tmp/fsa/release
}

win32:
   LIBS += $$BIN_ROOT/resources.dll
DESTDIR = $$BIN_ROOT
OBJECTS_DIR = $$BUILD_ROOT/.obj
MOC_DIR = $$BUILD_ROOT/.moc
RCC_DIR = $$BUILD_ROOT/.rcc
UI_DIR =  $$BUILD_ROOT/.ui
CONFIG += C++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    monitorthread.cpp \
    managers/configdepository.cpp \
    managers/logmanager.cpp \
    utilities/iconhelper.cpp \
    utilities/utilities.cpp \
    dialogs/aboutdialog.cpp \
    dialogs/messagedialog.cpp \
    dialogs/shadowdialog.cpp \
    widgets/customctrls.cpp

HEADERS  += \
    mainwindow.h \
    basicdefines.h \
    monitorthread.h \
    managers/configdepository.h \
    managers/logmanager.h \
    utilities/debugtools.h \
    utilities/iconhelper.h \
    utilities/utilities.h \
    dialogs/aboutdialog.h \
    dialogs/messagedialog.h \
    dialogs/shadowdialog.h \
    widgets/customctrls.h

FORMS    +=

OTHER_FILES +=
