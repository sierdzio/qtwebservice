include(../../buildInfo.pri)

QT += qtestlib
CONFIG += qtestlib

include(../../libraryIncludes.pri)

DESTDIR = $${TESTS_DIRECTORY}/QWebService
OBJECTS_DIR = $${TESTS_DIRECTORY}/QWebService
MOC_DIR = $${TESTS_DIRECTORY}/QWebService

SOURCES += tst_qwebservice.cpp
