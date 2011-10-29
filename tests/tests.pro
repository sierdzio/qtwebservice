include(../buildInfo.pri)

TEMPLATE = subdirs

DESTDIR = $${BUILD_DIRECTORY}/tests
OBJECTS_DIR = $${BUILD_DIRECTORY}/tests
MOC_DIR = $${BUILD_DIRECTORY}/tests

SUBDIRS += \
    QWebService \
    QWebMethod \
    QWebServiceMethod \
    QWsdl \
    qtwsdlconvert

