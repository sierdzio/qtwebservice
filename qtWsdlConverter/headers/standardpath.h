#ifndef STANDARDPATH_H
#define STANDARDPATH_H

#include <QObject>
#include <QWebService>
#include "flags.h"

/**
  Used to generate files in the standard path (with --standard-structure switch on).

  In the future, this might be modified to support all switches.
  */
class StandardPath : public QObject
{
    Q_OBJECT
public:
    explicit StandardPath(QObject *parent = 0);
    static bool create(QWsdl *wsdl, QDir workingDir, Flags flgs, QString baseClassName = 0, QObject *parent = 0);
    bool isErrorState();

signals:
    void errorEncountered(QString errMessage);

public slots:

private:
    bool enterErrorState(QString errMessage = "");
    void prepare();
    bool createMessages();
    bool createMessageHeader(QSoapMessage *msg);
    bool createMessageSource(QSoapMessage *msg);
    bool createMainCpp();
    bool createService();
    bool createServiceHeader();
    bool createServiceSource();
    bool createBuildSystemFile();
    bool createQMakeProject();

    QMap<QString, QSoapMessage *> *messages;
    QDir workingDir;
    QWsdl *wsdl;
    Flags flags;
    bool errorState;
    QString errorMessage;
    QString baseClassName;
};

#endif // STANDARDPATH_H
