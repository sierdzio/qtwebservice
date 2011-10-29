include(../../buildInfo.pri)

QT += qtestlib
CONFIG += qtestlib

include(../../libraryIncludes.pri)

DESTDIR = $${BUILD_DIRECTORY}/tests/QWebMethod
OBJECTS_DIR = $${BUILD_DIRECTORY}/tests/QWebMethod
MOC_DIR = $${BUILD_DIRECTORY}/tests/QWebMethod

SOURCES += tst_qwebmethod.cpp
