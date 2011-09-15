#include <QtCore>
#if QT_VERSION < 0x050000
    #include <QtGui/QMainWindow>
#else
    #include <QtWidgets/QApplication>
#endif
#include "qdnmain.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QdnMain w;
    w.show();

    return a.exec();
}
