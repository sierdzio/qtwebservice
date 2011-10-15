#ifndef TEMPLATELOGIC_H
#define TEMPLATELOGIC_H

#include <QtCore/qstring.h>
#include <QtCore/qfile.h>
#include "flags.h"

class TemplateLogic
{
public:
    TemplateLogic(Flags *flgs = 0);

    enum FileType {
        Header        = 0,
        PrivateHeader = 1,
        Source        = 2
    };

    QString readFile(QString path);
    int removeTag(QString &templateCode, QString tag, int beginIndex = 0);
    QString mergeHeaders(QString headerPath, QString privateHeaderPath);
    QString stripFile(QString fileData, FileType type = Header);

private:
    Flags *flags;
};

#endif // TEMPLATELOGIC_H
