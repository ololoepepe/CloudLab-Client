/****************************************************************************
**
** Copyright (C) 2013-2014 Andrey Bogdanov
**
** This file is part of CloudLab Client.
**
** CloudLab Client is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** CloudLab Client is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with CloudLab Client.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef TEXSAMPLECORE_H
#define TEXSAMPLECORE_H

class Cache;
class LabModel;

class TBinaryFile;
class TGroupModel;
class TInviteModel;
class TNetworkClient;
class TUserModel;

class BNetworkConnection;
class BNetworkOperation;

class QString;
class QWidget;

#include "application.h"

#include <TBinaryFileList>

#include <BVersion>

#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QUrl>
#include <QWidget>

#if defined(tSmp)
#   undef tSmp
#endif
#define tSmp (static_cast<Application *>(BApplication::instance())->texsampleCore())

/*============================================================================
================================ TexsampleCore ===============================
============================================================================*/

class TexsampleCore : public QObject
{
    Q_OBJECT
private:
    Cache *mcache;
    TNetworkClient *mclient;
    bool mdestructorCalled;
    QMap< quint64, QPointer<QWidget> > meditLabDialogs;
    QList<QObject *> mfutureWatchers;
    QPointer<QWidget> mgroupManagementDialog;
    TGroupModel *mgroupModel;
    QPointer<QWidget> minviteManagementDialog;
    TInviteModel *minviteModel;
    QMap< quint64, QPointer<QWidget> > mlabInfoDialogs;
    LabModel *mlabModel;
    QPointer<QWidget> msendLabDialog;
    QMap< quint64, QPointer<QWidget> > muserInfoDialogs;
    QPointer<QWidget> muserManagementDialog;
    TUserModel *muserModel;
public:
    explicit TexsampleCore(QObject *parent = 0);
    ~TexsampleCore();
public:
    Cache *cache() const;
    TNetworkClient *client() const;
    TGroupModel *groupModel() const;
    TInviteModel *inviteModel() const;
    LabModel *labModel() const;
    void updateCacheSettings();
    void updateClientSettings();
    TUserModel *userModel() const;
public slots:
    bool checkForNewVersion(bool persistent = false);
    bool checkForNewVersionPersistent();
    void connectToServer();
    bool deleteLab(quint64 labId, QWidget *parent = 0);
    void disconnectFromServer();
    void editLab(quint64 labId);
    bool getLab(quint64 labId, QWidget *parent = 0);
    void sendLab();
    bool showAccountManagementDialog(QWidget *parent = 0);
    bool showConfirmEmailChangeDialog(QWidget *parent = 0);
    bool showConfirmRegistrationDialog(QWidget *parent = 0);
    void showGroupManagementWidget();
    void showInviteManagementWidget();
    bool showRecoverDialog(QWidget *parent = 0);
    bool showRegisterDialog(QWidget *parent = 0);
    void showLabInfo(quint64 labId);
    bool showTexsampleSettings(QWidget *parent = 0);
    void showUserInfo(quint64 userId);
    void showUserManagementWidget();
    void updateLabList();
signals:
    void stopWaiting();
private:
    struct CheckForNewVersionResult
    {
    public:
        QString message;
        bool persistent;
        bool success;
        QUrl url;
        BVersion version;
    public:
        explicit CheckForNewVersionResult(bool persistent = false);
    };
private:
    typedef QFuture<CheckForNewVersionResult> Future;
    typedef QFutureWatcher<CheckForNewVersionResult> Watcher;
private:
    static CheckForNewVersionResult checkForNewVersionFunction(bool persistent);
    static void runExecutable(const QString &url, const QString &path);
    static void showMessageFunction(const QString &text, const QString &informativeText, bool error,
                                    QWidget *parentWidget);
    static bool waitForConnectedFunction(BNetworkConnection *connection, int timeout, QWidget *parentWidget,
                                         QString *msg);
    static void waitForDestroyed(QWidget *wgt, const QString &path);
    static bool waitForFinishedFunction(BNetworkOperation *op, int timeout, QWidget *parentWidget, QString *msg);
private slots:
    void checkingForNewVersionFinished();
    void checkTexsample();
    void clientAuthorizedChanged(bool authorized);
    void editLabDialogFinished(int result);
    void sendLabDialogFinished(int result);
};

#endif // TEXSAMPLECORE_H
