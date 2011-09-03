#include "qdnlogin.h"
#include "ui_qdnlogin.h"

QdnLogin::QdnLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QdnLogin)
{
    ui->setupUi(this);
}

QdnLogin::~QdnLogin()
{
    delete ui;
}
