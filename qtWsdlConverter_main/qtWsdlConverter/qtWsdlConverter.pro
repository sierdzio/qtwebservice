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
    sources/wsdlconverter.cpp

HEADERS += \
    headers/wsdlconverter.h
