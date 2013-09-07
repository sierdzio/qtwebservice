#-------------------------------------------------
#
# Project created by QtCreator 2011-09-03T14:28:37
#
#-------------------------------------------------
include(../../buildInfo.pri)

QT       += gui widgets

TARGET = QtDeveloperNetworkExample
TEMPLATE = app

include(../../libraryIncludes.pri)

DESTDIR = $${EXAMPLES_DIRECTORY}/QtDeveloperNetworkExample
OBJECTS_DIR = $${EXAMPLES_DIRECTORY}/QtDeveloperNetworkExample
MOC_DIR = $${EXAMPLES_DIRECTORY}/QtDeveloperNetworkExample

SOURCES += main.cpp qdnmain.cpp

HEADERS  += qdnmain.h

FORMS    += qdnmain.ui






