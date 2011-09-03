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

VERSION = 0.4.2

DESTDIR = ../lib
OBJECTS_DIR = ../build-QWebService

SOURCES += \
    sources/qwebmethod.cpp \
    sources/qwebservicemethod.cpp \
    sources/qwsdl.cpp \
    sources/qwebservice.cpp \
    sources/qwebservicereader.cpp \


HEADERS += headers/QWebService_global.h \
    headers/QWebService \
    headers/qwebmethod.h \
    headers/qwebservicemethod.h \
    headers/qwsdl.h \
    headers/qwebservice.h \
    headers/qwebservicereader.h


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
