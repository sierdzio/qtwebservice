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
    ../../QWebService_main/QWebService/sources/qwebserviceabstract.cpp \
    ../../QWebService_main/QWebService/sources/qwebservicereaderabstract.cpp \
    ../../QWebService_main/QWebService/sources/qwebservicemethodabstract.cpp \
    ../../QWebService_main/QWebService/sources/qwsdl.cpp \
    ../../QWebService_main/QWebService/sources/qsoapmessage.cpp

HEADERS += headers/wsdlconverter.h \
    headers/standardpath.h \
    headers/flags.h \
    ../../QWebService_main/QWebService/headers/qwebserviceabstract.h \
    ../../QWebService_main/QWebService/headers/qwebservicereaderabstract.h \
    ../../QWebService_main/QWebService/headers/qwebservicemethodabstract.h \
    ../../QWebService_main/QWebService/headers/qwsdl.h \
    ../../QWebService_main/QWebService/headers/qsoapmessage.h

OTHER_FILES += \
    examples/band_ws.asmx \
    examples/stockquote.asmx \
    examples/example_URLs_and_commands.txt \
    doc/futureFeatures.txt
