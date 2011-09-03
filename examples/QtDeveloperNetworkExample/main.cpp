#include <QtCore>
#include <QtGui>
#include "qdnmain.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QdnMain w;
    w.show();

    return a.exec();
}
