#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QObject>
#include <QWebService>
#include "flags.h"

class CodeGenerator : public QObject
{
    Q_OBJECT
public:
    explicit CodeGenerator(QObject *parent = 0);
    static bool create(QWsdl *wsdl, QDir workingDir, Flags *flgs, QString baseClassName = 0, QObject *parent = 0);
    bool isErrorState();

signals:
    void errorEncountered(QString errMessage);

public slots:

private:
    bool enterErrorState(QString errMessage = "");
    void prepare();
    bool createMessages();
    bool createMessageHeader(QWebMethod *msg);
    bool createMessageSource(QWebMethod *msg);
    bool createMainCpp();
    bool createService();
    bool createServiceHeader();
    bool createServiceSource();
    bool createBuildSystemFile();
    bool createQMakeProject();
    bool createCMakeProject();
    bool createSconsProject();

    QMap<QString, QWebMethod *> *messages;
    QDir workingDir;
    QWsdl *wsdl;
    Flags *flags;
    bool errorState;
    QString errorMessage;
    QString baseClassName;
};

#endif // CODEGENERATOR_H
