include(buildInfo.pri)

TEMPLATE = subdirs

DESTDIR = $${BUILD_DIRECTORY}
OBJECTS_DIR = $${BUILD_DIRECTORY}
MOC_DIR = $${BUILD_DIRECTORY}

SUBDIRS += \
    QWebService \
    qtwsdlconvert \
    tests \
    examples
