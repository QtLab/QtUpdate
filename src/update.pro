#-------------------------------------------------
#
# Project created by QtCreator 2016-08-24T15:56:44
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = update
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    src/update.cpp \
    src/httpfile.cpp

HEADERS  += mainwindow.h \
    inc/httpfile.h \
    inc/update.h

FORMS    += mainwindow.ui
