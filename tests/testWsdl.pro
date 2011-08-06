#-------------------------------------------------
#
# Project created by QtCreator 2011-05-17T10:04:00
# Tomasz 'sierdzio' Siekierda
# sierdzio@gmail.com
#-------------------------------------------------

QT += core gui network xml

TARGET = testWsdl

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -lQWebService
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/ -lQWebServiced
else:symbian: LIBS += -lQWebService
else:unix: LIBS += -L$$PWD/../lib/ -lQWebService

INCLUDEPATH += $$PWD/../QWebService/headers
DEPENDPATH += $$PWD/../QWebService/headers

SOURCES += \
    main.cpp \
    testCase1.cpp \
    testcase2_wsdl.cpp \
    testcase3_webservice.cpp

HEADERS += \
    testCase1.h \
    testcase2_wsdl.h \
    testcase3_webservice.h




