include(../../buildInfo.pri)

QT += qtestlib
CONFIG += qtestlib

include(../../libraryIncludes.pri)

DESTDIR = $${BUILD_DIRECTORY}/tests/QWebService
OBJECTS_DIR = $${BUILD_DIRECTORY}/tests/QWebService
MOC_DIR = $${BUILD_DIRECTORY}/tests/QWebService

SOURCES += tst_qwebservice.cpp
