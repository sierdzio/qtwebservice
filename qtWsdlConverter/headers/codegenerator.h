#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qmap.h>
#include <QtCore/qdir.h>
//#include <QWebService>
#include <qwebservicemethod.h>
#include <qwsdl.h>
#include "flags.h"
#include "messagegenerator.h"

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
    bool createService();
    bool createServiceHeader();
    bool createServiceSource();
    bool createBuildSystemFile();
    bool createQMakeProject();
    bool createCMakeProject();
    bool createSconsProject();

    QMap<QString, QWebServiceMethod *> *messages;
    QDir workingDir;
    QWsdl *wsdl;
    Flags *flags;
    bool errorState;
    QString errorMessage;
    QString baseClassName;
};

#endif // CODEGENERATOR_H
