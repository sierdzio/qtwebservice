include(../../buildInfo.pri)

QT += testlib

include(../../libraryIncludes.pri)

DESTDIR = $${TESTS_DIRECTORY}/QWsdl
OBJECTS_DIR = $${TESTS_DIRECTORY}/QWsdl
MOC_DIR = $${TESTS_DIRECTORY}/QWsdl

SOURCES += tst_qwsdl.cpp
