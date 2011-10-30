include(../../buildInfo.pri)

QT += qtestlib
CONFIG += qtestlib

include(../../libraryIncludes.pri)

DESTDIR = $${TESTS_DIRECTORY}/qtwsdlconvert
OBJECTS_DIR = $${TESTS_DIRECTORY}/qtwsdlconvert
MOC_DIR = $${TESTS_DIRECTORY}/qtwsdlconvert

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

