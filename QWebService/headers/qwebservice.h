/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWebService library.
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.txt included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#ifndef QWEBSERVICE_H
#define QWEBSERVICE_H

#include <QtCore/qurl.h>
#include "QWebService_global.h"
#include "qwebmethod.h"
#include "qwsdl.h"

class QWebServicePrivate;

class QWEBSERVICESHARED_EXPORT QWebService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(QUrl hostUrl READ hostUrl WRITE setHost NOTIFY hostUrlChanged)
    Q_PROPERTY(QStringList methodNames READ methodNames NOTIFY methodNamesChanged)

public:
    QWebService(QObject *parent = 0);
    QWebService(QWsdl *wsdl, QObject *parent = 0);
    QWebService(const QString &host, QObject *parent = 0);
    ~QWebService();

    QString name() const;
    void setName(const QString &newWebServiceName = QString());

    QMap<QString, QWebMethod *> *methods();
    QWebMethod *method(const QString &methodName);
    QStringList methodNames() const;
    QStringList methodParameters(const QString &methodName) const;
    QStringList methodReturnValue(const QString &methodName) const;
    QMap<QString, QVariant> parameterNamesTypes(const QString &methodName) const;
    QMap<QString, QVariant> returnValueNameType(const QString &methodName) const;
    void addMethod(QWebMethod *newMethod);
    void addMethod(const QString &methodName, QWebMethod *newMethod);
    void removeMethod(const QString &methodName);
    Q_INVOKABLE bool invokeMethod(const QString &methodName, const QByteArray &data = 0);
    Q_INVOKABLE QString replyRead(const QString &methodName);

    QUrl hostUrl() const;
    QString host() const;
    void setHost(const QString &host);
    void setHost(const QUrl &hostUrl);

//    QString wsdl();
    Q_INVOKABLE void setWsdl(QWsdl *newWsdl);
    void resetWsdl(QWsdl *newWsdl = 0);

    bool isErrorState();
    QString errorInfo() const;

signals:
    void errorEncountered(const QString &errMessage);
    void replyReady(const QByteArray &reply, const QString &methodName);

    // For QObject properties:
    void hostChanged();
    void hostUrlChanged();
    void nameChanged();
    void methodNamesChanged();

protected:
    QWebService(QWebServicePrivate &d, QObject *parent = 0);
    QWebServicePrivate *d_ptr;

protected slots:
    void receiveReply(const QByteArray &reply);

private:
    Q_DECLARE_PRIVATE(QWebService)
};

#endif // QWEBSERVICE_H
