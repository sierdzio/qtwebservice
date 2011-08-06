#-------------------------------------------------
#
# Project created by QtCreator 2011-05-17T10:04:00
# Tomasz 'sierdzio' Siekierda
# sierdzio@gmail.com
#-------------------------------------------------

QT += core gui network xml

TARGET = testWsdl

TEMPLATE = app

SOURCES += ../QWebService/sources/qwebserviceabstract.cpp \
    ../QWebService/sources/qwebservicereaderabstract.cpp \
    ../QWebService/sources/qwebservicemethodabstract.cpp \
    ../QWebService/sources/qwsdl.cpp \
    ../QWebService/sources/qsoapmessage.cpp \
    main.cpp \
    testCase1.cpp \
    testcase2_wsdl.cpp \
    testcase3_webservice.cpp

HEADERS += ../QWebService/headers/qwebserviceabstract.h \
    ../QWebService/headers/qwebservicereaderabstract.h \
    ../QWebService/headers/qwebservicemethodabstract.h \
    ../QWebService/headers/qwsdl.h \
    ../QWebService/headers/qsoapmessage.h \
    ../QWebService/headers/QWebService.h \
    testCase1.h \
    testcase2_wsdl.h \
    testcase3_webservice.h
