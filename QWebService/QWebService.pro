#-------------------------------------------------
#
# Project created by QtCreator 2011-05-11T11:13:39
# Tomasz 'sierdzio' Siekierda
# sierdzio@gmail.com
#-------------------------------------------------
include(../buildInfo.pri)

TARGET   = QWebService

TEMPLATE = lib

CONFIG   += dll

DEFINES  += QWEBSERVICE_LIBRARY

VERSION  = 0.6.0

DESTDIR  = $$_PRO_FILE_PWD_/../lib
OBJECTS_DIR = $${BUILD_DIRECTORY}/QWebService
MOC_DIR = $${BUILD_DIRECTORY}/QWebService

SOURCES  += \
    sources/qwebmethod.cpp \
    sources/qwebservicemethod.cpp \
    sources/qwsdl.cpp \
    sources/qwebservice.cpp \

HEADERS  += headers/QWebService_global.h \
    headers/QWebService \
    headers/qwebmethod.h \
    headers/qwebservicemethod.h \
    headers/qwsdl.h \
    headers/qwebservice.h \
    headers/qwebmethod_p.h \
    headers/qwebservicemethod_p.h \
    headers/qwebservice_p.h \
    headers/qwsdl_p.h \
    headers/QtWebServiceQml.h

INSTALLS += target









