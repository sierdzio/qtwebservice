/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QtDeveloperNEtwork example.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
    ui->lineEditPassword->setEchoMode(QLineEdit::Password);

    webMethodProfile = new QWebMethod(QWebMethod::Json, QWebMethod::Get, this);
    webMethodProfile->setHost(QUrl::fromUserInput(
                              QString("http://developer.qt.nokia.com/qtapi/1/member/profile")));
    connect(webMethodProfile, SIGNAL(replyReady(QByteArray)), this, SLOT(profileReply()));

    webMethodPosts = new QWebMethod(
                QUrl::fromUserInput(
                    QString("http://developer.qt.nokia.com/qtapi/1/forums/posts/unread")),
                QWebMethod::Json, QWebMethod::Get, this);
    connect(webMethodPosts, SIGNAL(replyReady(QByteArray)), this, SLOT(postsReply()));
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
    webMethodProfile->authenticate(ui->lineEditLogin->text(),
                                   ui->lineEditPassword->text());
    webMethodProfile->sendMessage();

    QUrl url;
    url.addEncodedQueryItem("ACT", QUrl::toPercentEncoding("11"));
    url.addEncodedQueryItem("RET", QUrl::toPercentEncoding("/"));
    url.addEncodedQueryItem("site_id", QUrl::toPercentEncoding("1"));
    url.addEncodedQueryItem("username",
                            QUrl::toPercentEncoding(ui->lineEditLogin->text()));
    url.addEncodedQueryItem("password",
                            QUrl::toPercentEncoding(ui->lineEditPassword->text()));
    webMethodPosts->authenticate(url);
    webMethodPosts->sendMessage();
}

void QdnMain::profileReply()
{
    QString reply = webMethodProfile->replyRead();
    ui->labelUsernameResult->setText(reply);

    { // Points
        int pointsIndex = reply.indexOf("\"points\":") + 10;
        QString points = reply.mid(pointsIndex, 8);
        points.chop(points.length() - points.indexOf("\","));
        ui->labelPointsResult->setText(points);
    }
    { // Title
        int titleIndex = reply.indexOf("\"title\":") + 9;
        QString title = reply.mid(titleIndex, 30);
        title.chop(title.length() - title.indexOf("\","));
        ui->labelTitleResult->setText(title);
    }
    { // Level
        int levelIndex = reply.indexOf("\"level\":") + 8;
        QString level = reply.mid(levelIndex, 30);
        level.chop(level.length() - level.indexOf(","));
        ui->labelLevelResult->setText(level);
    }
    { // ID
        int idIndex = reply.indexOf("\"member_id\":") + 12;
        QString id = reply.mid(idIndex, 30);
        id.chop(id.length() - id.indexOf(","));
        ui->labelUserIdResult->setText(id);
    }
    { // Gravatar
        int gravatarIndex = reply.indexOf("\"gravatar_hash\":") + 17;
        QString gravatar = reply.mid(gravatarIndex, 50);
        gravatar.chop(gravatar.length() - gravatar.indexOf("\"}"));
        QString result = "http://www.gravatar.com/avatar/" + gravatar + "?s=512";
        ui->labelGravatarResult->setText(result);
//        ui->webViewGravatar->setUrl(QUrl::fromUserInput(result));
    }
}

void QdnMain::postsReply()
{
    QString reply = webMethodPosts->replyRead();
    ui->labelPostsResult->setText(reply);
}
