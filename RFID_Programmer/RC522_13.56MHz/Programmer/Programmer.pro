#-------------------------------------------------
#
# Project created by QtCreator 2018-03-10T14:23:17
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Programmer
TEMPLATE = app

CONFIG += c++11

OBJECTS_DIR         = ../build
MOC_DIR             = ../build
RCC_DIR             = ../build
UI_DIR				= ../build
DESTDIR             = ../bin

QMAKE_CXXFLAGS += "-std=c++11"

win32|win64{
    RC_FILE=  index.rc
    OTHER_FILES+= index.rc
    DISTFILES += index.rc
}

TRANSLATIONS = lang/ru_RU.ts

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
