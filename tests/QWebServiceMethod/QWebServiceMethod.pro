include(../../buildInfo.pri)

QT += qtestlib
CONFIG += qtestlib

include(../../libraryIncludes.pri)

DESTDIR = $${BUILD_DIRECTORY}/tests/QWebServiceMethod
OBJECTS_DIR = $${BUILD_DIRECTORY}/tests/QWebServiceMethod
MOC_DIR = $${BUILD_DIRECTORY}/tests/QWebServiceMethod

SOURCES += tst_qwebservicemethod.cpp
