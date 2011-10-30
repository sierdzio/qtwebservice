include(../../buildInfo.pri)

QT += qtestlib
CONFIG += qtestlib

include(../../libraryIncludes.pri)

DESTDIR = $${TESTS_DIRECTORY}/QWsdl
OBJECTS_DIR = $${TESTS_DIRECTORY}/QWsdl
MOC_DIR = $${TESTS_DIRECTORY}/QWsdl

SOURCES += tst_qwsdl.cpp
