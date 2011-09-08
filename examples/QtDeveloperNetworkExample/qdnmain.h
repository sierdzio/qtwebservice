#ifndef QDNMAIN_H
#define QDNMAIN_H

#include <QMainWindow>
#include <qwebmethod.h>

namespace Ui {
    class QdnMain;
}

class QdnMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit QdnMain(QWidget *parent = 0);
    ~QdnMain();

private slots:
    void on_actionQuit_triggered();
    void on_buttonLogin_clicked();
    void profileReply();
    void postsReply();

private:
    Ui::QdnMain *ui;
    QWebMethod *webMethodProfile;
    QWebMethod *webMethodPosts;
};

#endif // QDNMAIN_H
