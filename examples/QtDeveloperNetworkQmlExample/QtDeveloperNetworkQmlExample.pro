include(../../buildInfo.pri)
QT += gui quick qml

include(../../libraryIncludes.pri)

OUTDIR = $${EXAMPLES_DIRECTORY}/QtDeveloperNetworkQmlExample

DESTDIR = $${OUTDIR}
OBJECTS_DIR = $${OUTDIR}
MOC_DIR = $${OUTDIR}

OTHER_FILES = main.qml

SOURCES += main.cpp

unix {
    QMAKE_POST_LINK = cp $$_PRO_FILE_PWD_/main.qml $${OUTDIR}
}

win32 {
    QMAKE_POST_LINK = copy $$_PRO_FILE_PWD_/main.qml $${OUTDIR}
}
