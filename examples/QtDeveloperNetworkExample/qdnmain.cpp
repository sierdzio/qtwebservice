#include "qdnmain.h"
#include "ui_qdnmain.h"

QdnMain::QdnMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QdnMain)
{
    ui->setupUi(this);
    setCentralWidget(ui->tabWidget);

    webMethod = new QWebMethod(this, QWebMethod::json, QWebMethod::GET);
    webMethod->setHost(QUrl::fromUserInput("http://developer.qt.nokia.com/qtapi/1/member/profile"));
//    webMethod->setMessageName("");
}

QdnMain::~QdnMain()
{
    delete ui;
}

void QdnMain::on_actionQuit_triggered()
{
    this->close();
}

void QdnMain::on_buttonLogin_clicked()
{
    webMethod->setCredentials(ui->lineEditLogin->text(), ui->lineEditPassword->text());

    qDebug() << webMethod->host() << " " << webMethod->httpMethodString() << " "
             << webMethod->protocolString() << " " << webMethod->username();

    webMethod->sendMessage();

    forever {
        if (webMethod->isErrorState()) {
            ui->labelUsernameResult->setText(webMethod->replyRead().toString());
            return;
        }

        if (webMethod->isReplyReady()) {
            ui->labelUsernameResult->setText(webMethod->replyRead().toString());
            break;
        }
        else {
            qApp->processEvents();
        }
    }
}
