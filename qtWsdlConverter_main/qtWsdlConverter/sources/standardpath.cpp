#include "../headers/standardpath.h"

StandardPath::StandardPath(QObject *parent) :
    QObject(parent)
{
}

void StandardPath::prepare()
{
    workingDir.mkdir("headers");
    workingDir.mkdir("sources");

    messages = wsdl->getMethods();
}

bool StandardPath::create(QWsdl *w, QDir wrkDir, QObject *parent)
{
    StandardPath obj(parent);
    obj.workingDir = wrkDir;
    obj.wsdl = w;

    obj.prepare();

    if (!obj.createMessages())
        return false;
}

bool StandardPath::createMessages()
{
    workingDir.cd("headers");

    foreach (QString s, messages->keys())
    {
        QSoapMessage *m = messages->value(s);
        createMessageHeader(m);
    }

    workingDir.cdUp();
    return true;
}

bool StandardPath::createMessageHeader(QSoapMessage *msg)
{
    QFile messageFile(msg->getMessageName());

    messageFile.open(QFile::WriteOnly | QFile::Text); // This means endline will be \r\n on Windows. Might be a bad idea.
}
