#include <QUrl>
#include <QGuiApplication>
#include <QQuickView>
#include <QWebService>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // This method can be used if the whole QWebService was included.
    registerQmlTypes();
    // When including just QWebMethod, this line should be used.
//    qmlRegisterType<QWebMethod>("QtWebService", 1, 0, "WebMethod");

    QQuickView view(QUrl::fromUserInput("main.qml"));
    view.show();

    return app.exec();
}
