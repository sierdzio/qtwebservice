#ifndef QDNLOGIN_H
#define QDNLOGIN_H

#include <QDialog>

namespace Ui {
    class QdnLogin;
}

class QdnLogin : public QDialog
{
    Q_OBJECT

public:
    explicit QdnLogin(QWidget *parent = 0);
    ~QdnLogin();

private:
    Ui::QdnLogin *ui;
};

#endif // QDNLOGIN_H
