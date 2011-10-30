#if QT_VERSION < 0x050000
    #include <QtGui/QApplication>
    #include <QtGui/QGraphicsView>
#else
    #include <QtWidgets/QApplication>
#endif
#include <QtDeclarative>
#include "qmlapplicationviewer.h"
#include <qwebmethod.h>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    qmlRegisterType<QWebMethod>("QtWebService", 1, 0, "WebMethod");

    QScopedPointer<QmlApplicationViewer> viewer(QmlApplicationViewer::create());

    viewer->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer->setMainQmlFile(QLatin1String("qml/QtDeveloperNetworkQmlExample/main.qml"));
    viewer->showExpanded();

    return app->exec();
}
