#ifndef MESSAGEGENERATOR_H
#define MESSAGEGENERATOR_H

#include <QObject>
#include "flags.h"

class MessageGenerator : public QObject
{
    Q_OBJECT
public:
    explicit MessageGenerator(QMap<QString, QWebMethod *> *messages,
                              QDir workingDir, Flags *flgs, QObject *parent = 0);
    QString errorMessage();
    bool createMessages();

private:
    bool createSubclassedMessageHeader(QWebMethod *msg);
    bool createSubclassedMessageSource(QWebMethod *msg);
    bool createMessageHeader(QWebMethod *msg);
    bool createMessageSource(QWebMethod *msg);

    QMap<QString, QWebMethod *> *messages;
    QDir workingDir;
    Flags *flags;
    bool errorState;
    QString m_errorMessage;
};

#endif // MESSAGEGENERATOR_H
