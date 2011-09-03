#include "qdnmain.h"
#include "ui_qdnmain.h"

QdnMain::QdnMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QdnMain)
{
    ui->setupUi(this);
    centralWidget()->setLayout(ui->layoutMain);
    ui->tabProfile->setLayout(ui->layoutProfile);
    ui->tabPosts->setLayout(ui->layoutPosts);

    webMethodProfile = new QWebMethod(this, QWebMethod::json, QWebMethod::GET);
    webMethodProfile->setHost(QUrl::fromUserInput("http://developer.qt.nokia.com/qtapi/1/member/profile"));

    webMethodPosts = new QWebMethod(this, QWebMethod::json, QWebMethod::GET);
    webMethodPosts->setHost(QUrl::fromUserInput("http://developer.qt.nokia.com/qtapi/1/forums/posts/unread"));
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
//    webMethodProfile->setCredentials(ui->lineEditLogin->text(), ui->lineEditPassword->text());
    webMethodProfile->authenticate(ui->lineEditLogin->text(), ui->lineEditPassword->text());
    webMethodProfile->sendMessage();

//    webMethodPosts->setCredentials(ui->lineEditLogin->text(), ui->lineEditPassword->text());
    webMethodPosts->authenticate(ui->lineEditLogin->text(), ui->lineEditPassword->text());
    webMethodPosts->sendMessage();

    forever {
        if (webMethodProfile->isErrorState()) {
            ui->labelUsernameResult->setText(webMethodProfile->replyRead().toString());
            return;
        }

        if (webMethodProfile->isReplyReady()) {
            ui->labelUsernameResult->setText(webMethodProfile->replyRead().toString());
            break;
        }
        else {
            qApp->processEvents();
        }
    }

    forever {
        if (webMethodPosts->isErrorState()) {
            ui->labelPostsResult->setText(webMethodPosts->replyRead().toString());
            return;
        }

        if (webMethodPosts->isReplyReady()) {
            ui->labelPostsResult->setText(webMethodPosts->replyRead().toString());
            break;
        }
        else {
            qApp->processEvents();
        }
    }
}
