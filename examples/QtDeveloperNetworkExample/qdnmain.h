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

#ifndef QDNMAIN_H
#define QDNMAIN_H

#if QT_VERSION < 0x050000
    #include <QMainWindow>
#else
    #include <QtWidgets/QMainWindow>
#endif
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
