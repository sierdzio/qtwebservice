#ifndef WSDLCONVERTER_H
#define WSDLCONVERTER_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qmap.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvariant.h>
//#include <QWebService>
#include <qwsdl.h>
#include "flags.h"
#include "codegenerator.h"

class WsdlConverter : public QObject
{
    Q_OBJECT
    Q_ENUMS(ArgumentDescriptionFlag)
    Q_FLAGS(ArgumentDescription)

public:
    enum ArgumentDescriptionFlag
    {
        AppName     = 0x01,
        Path        = 0x02,
        Dir         = 0x04,
        ClassName   = 0x08,
        FlagHelp    = 0x0F
    };
    Q_DECLARE_FLAGS(ArgumentDescription, ArgumentDescriptionFlag)

    explicit WsdlConverter(QStringList appArguments, QObject *parent = 0);
    ~WsdlConverter();

    void setFlags(Flags flgs);
    bool isErrorState();
    QString errorInfo();
    void resetError();
    void convert();
    QString webServiceName();

signals:
    void errorEncountered(QString errMessage);

public slots:

private:
    bool parseArguments(QStringList arguments);
    void displayHelp();
    void displayIntro();
    void displayOutro();

    bool enterErrorState(QString errMessage = "");
    bool removeDir(QString path);

    bool errorState;
    QString errorMessage;
    QString baseClassName;
    QString outputDir;
    QWsdl *wsdl;
    QMap<int, QVariant> *argList;
    Flags *flags;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(WsdlConverter::ArgumentDescription)

#endif // WSDLCONVERTER_H
