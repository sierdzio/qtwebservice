#ifndef QWSDL_H
#define QWSDL_H

#include <QXmlStreamReader>
#include <QFile>
#include <QtCore>
#include "qwebmethod.h"
#include "QWebService_global.h"

class QWEBSERVICESHARED_EXPORT QWsdl : public QObject
{
    Q_OBJECT

public:
    explicit QWsdl(QObject *parent = 0);
    QWsdl(QString wsdlFile, QObject *parent = 0);
    ~QWsdl();

    void setWsdlFile(QString wsdlFile); // == resetWsdl()
    QStringList getMethodNames();
    QMap<QString, QWebMethod *> *getMethods();
    QString getWebServiceName();
    QString getHost();
    QUrl getHostUrl();
    QString getTargetNamespace();
    //QFile getWsdl(); Rethink that. Maybe QString? Or saving to file specified in parameter?

    QString getErrorInfo();
    bool isErrorState();
    void resetWsdl(QString newWsdl);

signals:

public slots:
    void fileReplyFinished(QNetworkReply *rply);

private:
    bool parse();
    void prepareFile();
    void prepareMethods();
    void readDefinitions();
    void readTypes();
    void readTypeSchemaElement();
    void readPorts();
    void readMessages();
    void readBindings();
    void readService();
    void readDocumentation();
    QString convertReplyToUtf(QString textToConvert);

    bool errorState;
    bool replyReceived;
//    QFile wsdlFile;
    QString errorMessage;
    QString wsdlFilePath;
    QString webServiceName;
    QString host, hostUrl, targetNamespace;
    QXmlStreamReader xmlReader;

    QStringList *workMethodList;
    QMap<int, QMap<QString, QVariant> > *workMethodParameters; // Param if one, QList if many.
    QMap<QString, QWebMethod *> *methods;
};

#endif // QWSDL_H
