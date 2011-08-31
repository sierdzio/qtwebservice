#ifndef QWSDL_H
#define QWSDL_H

#include <QtCore/QXmlStreamReader>
#include <QtCore/qfile.h>
#include <QtCore/qmap.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdatetime.h>
#include "QWebService_global.h"
#include "qwebservicemethod.h"

class QWEBSERVICESHARED_EXPORT QWsdl : public QObject
{
    Q_OBJECT

public:
    explicit QWsdl(QObject *parent = 0);
    QWsdl(QString wsdlFile, QObject *parent = 0);
    ~QWsdl();

    void setWsdlFile(QString wsdlFile); // == resetWsdl()
    void resetWsdl(QString newWsdl);

    QStringList methodNames() const;
    QMap<QString, QWebServiceMethod *> *methods();
    QString webServiceName() const;
    QString host() const;
    QUrl hostUrl() const;
    QString wsdlFile() const;
    QString targetNamespace() const;
    //QFile getWsdl(); Rethink that. Maybe QString? Or saving to file specified in parameter?
    QString errorInfo() const;
    bool isErrorState() const;


signals:
    void errorEncountered(QString errMessage);

public slots:
    void fileReplyFinished(QNetworkReply *rply);

private:
    void init();
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
    bool enterErrorState(QString errMessage = "");

    bool errorState;
    bool replyReceived;
//    QFile wsdlFile;
    QUrl m_hostUrl;
    QString errorMessage;
    QString wsdlFilePath;
    QString m_webServiceName;
    QString m_targetNamespace;
    QXmlStreamReader xmlReader;

    QStringList *workMethodList;
    QMap<int, QMap<QString, QVariant> > *workMethodParameters; // Param if one, QList if many.
    QMap<QString, QWebServiceMethod *> *methodsMap;
};

#endif // QWSDL_H
