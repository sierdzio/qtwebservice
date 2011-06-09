#ifndef QSOAPMESSAGE_H
#define QSOAPMESSAGE_H

#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class QSoapMessage : public QObject
{
    Q_OBJECT

public:
    explicit QSoapMessage(QObject *parent = 0);
    QSoapMessage(QUrl hostUrl, QString messageName, QObject *parent = 0);
    QSoapMessage(QString hostname, QString messageName, QObject *parent = 0);
    QSoapMessage(QString hostname, QString messageName, QMap<QString, QVariant> params,
                 QMap<QString, QVariant> returnValue, QObject *parent = 0);
    ~QSoapMessage();
    // ADD TARGET NAMESPACE!
    void setParams(QMap<QString, QVariant> params, QMap<QString, QVariant> returnValue);
    void setTargetNamespace(QString tNamespace);
    bool sendMessage();
    bool sendMessage(QMap<QString, QVariant> params);
    QVariant static sendMessage(QObject *parent, QUrl url, QString _messageName,
                                QMap<QString, QVariant> params, QMap<QString, QVariant> returnVal);
    QVariant replyRead();
    QStringList getParameterNames();
    QStringList getReturnValueName();
    QMap<QString, QVariant> getParameterNamesTypes();
    QMap<QString, QVariant> getReturnValueNameType();
    QString getTargetNamespace();

    ///TEMP:
    bool isReplyReceived();
    ///ENDOF TEMP

    enum Role {outboundRole, inboundRole, staticRole, noRole};
    enum Version {soap10, soap12};

signals:
    void replyReady(QVariant rply);

public slots:
    void replyFinished(QNetworkReply *reply);

private:
    void init();
    void prepareRequestData();
    QString convertReplyToUtf(QString textToConvert);

    bool replyReceived;
    Role role;
    Version version;
    QUrl hostUrl;
    QString hostname;
    QString messageName;
    QString targetNamespace;
    QVariant reply;
//    QVariant returnValue;
    QMap<QString, QVariant> parameters;
    QMap<QString, QVariant> returnValue;
    QNetworkAccessManager *manager;
    QNetworkReply *networkReply;
    QByteArray data;
};
#endif // QSOAPMESSAGE_H
