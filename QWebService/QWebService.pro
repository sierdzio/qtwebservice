#-------------------------------------------------
#
# Project created by QtCreator 2011-05-11T11:13:39
# Tomasz 'sierdzio' Siekierda
# sierdzio@gmail.com
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = QWebService

TEMPLATE = lib

CONFIG += dll

DEFINES += QWEBSERVICE_LIBRARY

VERSION = 0.3.6

DESTDIR = ../lib
OBJECTS_DIR = ../build-QWebService

SOURCES += \
    sources/qwsdl.cpp \
    sources/qwebservice.cpp \
    sources/qwebservicemethod.cpp \
    sources/qwebservicereader.cpp \
    sources/qwebmethod.cpp

HEADERS += headers/QWebService_global.h \
    headers/qwsdl.h \
    headers/QWebService \
    headers/qwebservice.h \
    headers/qwebservicemethod.h \
    headers/qwebservicereader.h \
    headers/qwebmethod.h

symbian {
    #Symbian specific definitions
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE45E3457
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = QWebService.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/local/lib
    }
    INSTALLS += target
}
