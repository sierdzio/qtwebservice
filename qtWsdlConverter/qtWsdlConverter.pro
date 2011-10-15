#-------------------------------------------------
#
# Project created by QtCreator 2011-06-10T14:59:44
# Tomasz 'sierdzio' Siekierda
# sierdzio@gmail.com
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET   = qtwsdlconvert
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -lQWebService
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/ -lQWebServiced
else:symbian: LIBS += -lQWebService
else:unix: LIBS += -L$$PWD/../lib/ -lQWebService

INCLUDEPATH += $$PWD/../QWebService/headers
DEPENDPATH += $$PWD/../QWebService/headers

OBJECTS_DIR = ../build-qtWsdlConverter

SOURCES += sources/main.cpp \
    sources/wsdlconverter.cpp \
    sources/flags.cpp \
    sources/codegenerator.cpp \
    sources/messagegenerator.cpp \
    sources/templatelogic.cpp

HEADERS += headers/wsdlconverter.h \
    headers/flags.h \
    headers/codegenerator.h \
    headers/messagegenerator.h \
    headers/templatelogic.h

OTHER_FILES += \
    examples/band_ws.asmx \
    examples/stockquote.asmx \
    examples/example_URLs_and_commands.txt \
    doc/futureFeatures.txt


