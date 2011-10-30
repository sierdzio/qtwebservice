include(../../buildInfo.pri)

QT += qtestlib
CONFIG += qtestlib

include(../../libraryIncludes.pri)

DESTDIR = $${TESTS_DIRECTORY}/QWebMethod
OBJECTS_DIR = $${TESTS_DIRECTORY}/QWebMethod
MOC_DIR = $${TESTS_DIRECTORY}/QWebMethod

SOURCES += tst_qwebmethod.cpp
