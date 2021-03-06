#include "../headers/templatelogic.h"

/*!
    \class TemplateLogic
    \brief Used to prepare QWebMethod code for template processing.

    Provides some basic methods used in preparing QWebMethod code, as well as
    process template tags, etc.

    Generation is based on templates located in ./qtwsdlconverter/templates (for
    subclassed methods) and on real QWebMethod code (for non-subclassed methods).
  */
/*!
    \enum TemplateLogic::FileType

    This enum type specifies what type of file is being processed.

    \value Header
           The file is a header.
    \value PrivateHeader
           The file is a private header.
    \value Source
           The file is a source file.
  */

/*!
    Constructor, takes in the Flags object (\a flgs).
  */
TemplateLogic::TemplateLogic(Flags *flgs) :
    flags(flgs)
{
}

/*!
    Reads file contents and puts it into a QString. A crude, but
    effective way of helping in method creation. Needs \a path to operate on.
  */
QString TemplateLogic::readFile(QString path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    }

    return file.readAll();
}

/*!
    Removes a given tag and returns it's begin index,
    which can be used to insert new data.

    Takes in \a templateCode string, which is the text of the file,
    \a tag, which is the tag to look for, and optionally \a beginIndex, which
    can be used to optimise the search a bit (by specifying where to start).
  */
int TemplateLogic::removeTag(QString &templateCode, QString tag, int beginIndex)
{
    int endIndex = 0;

    beginIndex = templateCode.indexOf(tag, beginIndex);
    endIndex = templateCode.indexOf("%", beginIndex + 1) + 1;
    templateCode.remove(beginIndex, endIndex - beginIndex);

    return beginIndex;
}


/*!
    Merges a private and public header into one file.
    Both \a headerPath and \a privateHeaderPath are needed.
  */
QString TemplateLogic::mergeHeaders(QString headerPath, QString privateHeaderPath)
{
    QString header = stripFile(readFile(headerPath), Header);
    QString privateHeader = stripFile(readFile(privateHeaderPath), PrivateHeader);

    int beginIndex = 0;

    beginIndex = header.indexOf("private:") + 8;
    header.insert(beginIndex, flags->endLine() + flags->tab() + privateHeader);

    return header;
}

/*!
    Strips file data from license headers, includes etc.
    Takes in \a fileData in for of a QString, and \a type (it
    tells the algorithm what to do with the data).
  */
QString TemplateLogic::stripFile(QString fileData, FileType type)
{
    int beginIndex = 0;
    int endIndex = 0;

    if (type == PrivateHeader)
        endIndex = fileData.indexOf("class");
    else if (type == Header)
        endIndex = fileData.indexOf("#include");
    else if (type == Source)
        endIndex = fileData.indexOf("QWebMethod::QWebMethod");

    fileData.remove(beginIndex, endIndex - beginIndex);

    if (type == PrivateHeader || type == Header) {
        beginIndex = fileData.indexOf("class");

        if (type == Header)
            endIndex = fileData.indexOf("class", beginIndex + 2);
        else
            endIndex = fileData.indexOf("void init();");

        fileData.remove(beginIndex, endIndex - beginIndex);

        if (type == Header) {
            fileData.remove("QWEBSERVICESHARED_EXPORT");

            beginIndex = fileData.indexOf("QWebMethod(QWebMethodPrivate");
            endIndex = fileData.indexOf("private:");
        } else {
            beginIndex = fileData.indexOf("};");
            endIndex = fileData.length() - 1;
        }

        fileData.remove(beginIndex, endIndex - beginIndex);
        fileData.insert(beginIndex, flags->endLine());

        if (type == Header) {
            beginIndex = fileData.indexOf("#include \"QWebService_global.h\"");
            endIndex = fileData.indexOf("h\"", beginIndex) + 3;

            fileData.remove(beginIndex, endIndex - beginIndex);

            beginIndex = fileData.indexOf("Q_DECLARE_P");
            endIndex = fileData.indexOf("};", beginIndex);

            fileData.remove(beginIndex, endIndex - beginIndex);

            beginIndex = fileData.indexOf("#endif");
            endIndex = fileData.length() - 1;
            fileData.remove(beginIndex, endIndex - beginIndex);
        }
    } else {
        beginIndex = 0;
        endIndex = 0;

        // Remove QDoc.
        for (int i = 0; ; i++) {
            // Remember to break!
            beginIndex = fileData.indexOf("/*!");
            if (beginIndex == -1)
                break;

            endIndex = fileData.indexOf("*/", beginIndex);
            if (endIndex == -1)
                break;

            endIndex += 2;
            fileData.remove(beginIndex, endIndex - beginIndex);
        }
    }

    return fileData;
}
