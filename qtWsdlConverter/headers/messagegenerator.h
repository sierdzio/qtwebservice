#ifndef MESSAGEGENERATOR_H
#define MESSAGEGENERATOR_H

#include <QObject>
#include <QWebService>
#include "flags.h"

class MessageGenerator : public QObject
{
    Q_OBJECT
public:
    explicit MessageGenerator(QMap<QString, QWebServiceMethod *> *messages,
                              QDir workingDir, Flags *flgs, QObject *parent = 0);
    QString errorMessage();
    bool createMessages();

private:
    bool enterErrorState(QString errMessage = "");
    bool createSubclassedMessageHeader(QWebServiceMethod *msg);
    bool createSubclassedMessageSource(QWebServiceMethod *msg);
    bool createMessageHeader(QWebServiceMethod *msg);
    bool createMessageSource(QWebServiceMethod *msg);
    bool createMainCpp();

    void assignAllParameters(QWebServiceMethod *msg, QTextStream &out);

    QMap<QString, QWebServiceMethod *> *messages;
    QDir workingDir;
    Flags *flags;
    bool errorState;
    QString m_errorMessage;
};

#endif // MESSAGEGENERATOR_H
