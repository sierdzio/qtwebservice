#-------------------------------------------------
#
# Project created by QtCreator 2011-09-03T14:28:37
#
#-------------------------------------------------

QT       += core gui

TARGET = QtDeveloperNetworkExample
TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../lib/ -lQWebService
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../lib/ -lQWebServiced
else:symbian: LIBS += -lQWebService
else:unix: LIBS += -L$$PWD/../../lib/ -lQWebService

INCLUDEPATH += $$PWD/../../QWebService/headers
DEPENDPATH += $$PWD/../../QWebService/headers

SOURCES += main.cpp\
        qdnmain.cpp \
    qdnlogin.cpp

HEADERS  += qdnmain.h \
    qdnlogin.h

FORMS    += qdnmain.ui \
    qdnlogin.ui



