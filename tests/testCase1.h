#ifndef TESTCASE1_H
#define TESTCASE1_H

#include <QtGui>
#include <QWebService>

class testCase1 : public QPushButton
{
    Q_OBJECT

public:
    explicit testCase1(QWidget *parent = 0);
    ~testCase1();

public slots:
    void messageResponse(QVariant msg);

private:
    void soapMessageTest();

    QSoapMessage *message;
};

#endif
