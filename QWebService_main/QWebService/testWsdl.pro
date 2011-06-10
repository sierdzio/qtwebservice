#-------------------------------------------------
#
# Project created by QtCreator 2011-05-17T10:04:00
# Tomasz 'sierdzio' Siekierda
# sierdzio@gmail.com
#-------------------------------------------------

QT += core gui network xml

TARGET = testWsdl

TEMPLATE = app


SOURCES += sources/qwebserviceabstract.cpp \
    sources/qwebservicereaderabstract.cpp \
    sources/qwebservicemethodabstract.cpp \
    tests/main.cpp \
    sources/qwsdl.cpp \
    sources/qsoapmessage.cpp \
    tests/testCase1.cpp \
    tests/testcase2_wsdl.cpp \
    tests/testcase3_webservice.cpp

HEADERS += headers/qwebserviceabstract.h \
    headers/qwebservicereaderabstract.h \
    headers/qwebservicemethodabstract.h \
    headers/qwsdl.h \
    headers/qsoapmessage.h \
    tests/testCase1.h \
    tests/testcase2_wsdl.h \
    tests/testcase3_webservice.h
