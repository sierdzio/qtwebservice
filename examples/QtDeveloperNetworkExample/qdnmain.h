/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QtDeveloperNetwork example.
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.txt included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

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
