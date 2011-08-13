#ifndef QWSDL_H
#define QWSDL_H

#include <QXmlStreamReader>
#include <QFile>
#include <QtCore>
#include "qwebmethod.h"
#include "QWebService_global.h"

/**
  Reads web service data (message names, parameters, return values, ws name etc)
  from a WSDL file or URL.
  */
class QWEBSERVICESHARED_EXPORT QWsdl : public QObject
{
    Q_OBJECT

public:
    // Requires subsequent use of setWsdleFile()
    explicit QWsdl(QObject *parent = 0);
    // When this constructor returns, the file is already set and parsed
    QWsdl(QString wsdlFile, QObject *parent = 0);
//    QWsdl(QWsdl wsdlObject);
    ~QWsdl();
    // Sets the WSDL file (or URL)
    void setWsdlFile(QString wsdlFile); // == resetWsdl()
    // Returns a list of methods' names (as declared in WSDL)
    QStringList getMethodNames();
    // Returns methods themselves (as QWebMethods)
    QMap<QString, QWebMethod *> *getMethods();
    // Returns web service's name
    QString getWebServiceName();
    // Returns hostname (URL) - this has to be refactorised, as the name is not intuitive!
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
    //QMap<int, QVariant> *workMethodReturnParmeters;
    QMap<QString, QWebMethod *> *methods;
};

#endif // QWSDL_H
