#-------------------------------------------------
#
# Project created by QtCreator 2013-04-27T22:05:45
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = RfidTap
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    connectionhandler.cpp

include(qextserialport/qextserialport.pri)

HEADERS += \
    connectionhandler.h
