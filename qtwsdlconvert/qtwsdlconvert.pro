#-------------------------------------------------
#
# Project created by QtCreator 2011-06-10T14:59:44
# Tomasz 'sierdzio' Siekierda
# sierdzio@gmail.com
#-------------------------------------------------
include(../buildInfo.pri)

TARGET   = qtwsdlconvert
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../libraryIncludes.pri)

DESTDIR = $${BUILD_DIRECTORY}/qtwsdlconvert
OBJECTS_DIR = $${BUILD_DIRECTORY}/qtwsdlconvert
MOC_DIR = $${BUILD_DIRECTORY}/qtwsdlconvert

SOURCES += sources/main.cpp \
    sources/wsdlconverter.cpp \
    sources/flags.cpp \
    sources/codegenerator.cpp \
    sources/templatelogic.cpp \
    sources/methodgenerator.cpp

HEADERS += headers/wsdlconverter.h \
    headers/flags.h \
    headers/codegenerator.h \
    headers/templatelogic.h \
    headers/methodgenerator.h
