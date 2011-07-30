#-------------------------------------------------
#
# Project created by QtCreator 2011-06-10T14:59:44
# Tomasz 'sierdzio' Siekierda
# sierdzio@gmail.com
#-------------------------------------------------

QT       += core network xml

QT       -= gui

TARGET = qtWsdlConverter
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += sources/main.cpp \
    sources/wsdlconverter.cpp \
    sources/standardpath.cpp \
    sources/flags.cpp \
    ../QWebService/sources/qwebserviceabstract.cpp \
    ../QWebService/sources/qwebservicereaderabstract.cpp \
    ../QWebService/sources/qwebservicemethodabstract.cpp \
    ../QWebService/sources/qwsdl.cpp \
    ../QWebService/sources/qsoapmessage.cpp

HEADERS += headers/wsdlconverter.h \
    headers/standardpath.h \
    headers/flags.h \
    ../QWebService/headers/qwebserviceabstract.h \
    ../QWebService/headers/qwebservicereaderabstract.h \
    ../QWebService/headers/qwebservicemethodabstract.h \
    ../QWebService/headers/qwsdl.h \
    ../QWebService/headers/qsoapmessage.h

OTHER_FILES += \
    examples/band_ws.asmx \
    examples/stockquote.asmx \
    examples/example_URLs_and_commands.txt \
    doc/futureFeatures.txt
