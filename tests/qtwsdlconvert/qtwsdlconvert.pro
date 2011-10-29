include(../../buildInfo.pri)

QT += qtestlib
CONFIG += qtestlib

include(../../libraryIncludes.pri)

DESTDIR = $${BUILD_DIRECTORY}/tests/qtwsdlconvert
OBJECTS_DIR = $${BUILD_DIRECTORY}/tests/qtwsdlconvert
MOC_DIR = $${BUILD_DIRECTORY}/tests/qtwsdlconvert

SOURCES += tst_qtwsdlconvert.cpp \
    ../../qtwsdlconvert/sources/flags.cpp \
    ../../qtwsdlconvert/sources/templatelogic.cpp \
    ../../qtwsdlconvert/sources/wsdlconverter.cpp \
    ../../qtwsdlconvert/sources/methodgenerator.cpp \
    ../../qtwsdlconvert/sources/codegenerator.cpp

HEADERS += ../../qtwsdlconvert/headers/flags.h \
    ../../qtwsdlconvert/headers/templatelogic.h \
    ../../qtwsdlconvert/headers/wsdlconverter.h \
    ../../qtwsdlconvert/headers/methodgenerator.h \
    ../../qtwsdlconvert/headers/codegenerator.h

