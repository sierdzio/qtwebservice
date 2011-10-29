include(../../buildInfo.pri)

QT += qtestlib
CONFIG += qtestlib

include(../../libraryIncludes.pri)

DESTDIR = $${BUILD_DIRECTORY}/tests/QWsdl
OBJECTS_DIR = $${BUILD_DIRECTORY}/tests/QWsdl
MOC_DIR = $${BUILD_DIRECTORY}/tests/QWsdl

SOURCES += tst_qwsdl.cpp
