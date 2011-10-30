include(../../buildInfo.pri)

QT += qtestlib
CONFIG += qtestlib

include(../../libraryIncludes.pri)

DESTDIR = $${TESTS_DIRECTORY}/QWebServiceMethod
OBJECTS_DIR = $${TESTS_DIRECTORY}/QWebServiceMethod
MOC_DIR = $${TESTS_DIRECTORY}/QWebServiceMethod

SOURCES += tst_qwebservicemethod.cpp
