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

#include "texsamplecore.h"

#include "application.h"
#include "cache.h"
#include "dialog.h"
#include "mainwindow.h"
#include "labinfowidget.h"
#include "labmodel.h"
#include "settings.h"
#include "usermodel.h"

#include <TeXSample/TeXSampleCore>
#include <TeXSample/TeXSampleNetwork>
#include <TeXSample/TeXSampleNetworkWidgets>
#include <TeXSample/TeXSampleWidgets>

#include <BDialog>
#include <BDirTools>
#include <BDynamicTranslator>
#include <BeQt>
#include <BFileDialog>
#include <BLocationProvider>
#include <BNetworkConnection>
#include <BNetworkOperation>
#include <BOperationProgressDialog>
#include <BPassword>
#include <BTextTools>
#include <BTranslation>
#include <BUuid>
#include <BVersion>

#include <QAbstractSocket>
#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QIODevice>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QObject>
#include <QPointer>
#include <QProgressDialog>
#include <QPushButton>
#include <QString>
#include <QtConcurrentRun>
#include <QTextCodec>
#include <QThread>
#include <QTimer>
#include <QToolBar>
#include <QUrl>
#include <QVariant>
#include <QVBoxLayout>
#include <QWebPage>
#include <QWebView>
#include <QWidget>

#undef bApp
#define bApp (static_cast<Application *>(BApplication::instance()))

/*============================================================================
================================ TexsampleCore::CheckForNewVersionResult =====
============================================================================*/

/*============================== Public constructors =======================*/

TexsampleCore::CheckForNewVersionResult::CheckForNewVersionResult(bool persistent)
{
    this->persistent = persistent;
    success = false;
}

/*============================================================================
================================ TexsampleCore ===============================
============================================================================*/

/*============================== Public constructors =======================*/

TexsampleCore::TexsampleCore(QObject *parent) :
    QObject(parent)
{
    mdestructorCalled = false;
    BLocationProvider *provider = new BLocationProvider;
    provider->addLocation("texsample");
    provider->addLocation("texsample/labs");
    provider->addLocation("texsample/files");
    provider->createLocationPath("texsample", Application::UserResource);
    provider->createLocationPath("texsample/labs", Application::UserResource);
    provider->createLocationPath("texsample/files", Application::UserResource);
    Application::installLocationProvider(provider);
    QString texsampleLocation = BDirTools::findResource("texsample", BDirTools::UserOnly);
    mcache = new Cache(texsampleLocation);
    mclient = new TNetworkClient;
    mclient->setShowMessageFunction(&showMessageFunction);
    mclient->setWaitForConnectedFunction(&waitForConnectedFunction);
    mclient->setWaitForFinishedFunction(&waitForFinishedFunction);
    mclient->setWaitForConnectedDelay(BeQt::Second / 2);
    mclient->setWaitForFinishedDelay(BeQt::Second / 2);
    mclient->setWaitForConnectedTimeout(20 * BeQt::Second);
    mclient->setWaitForFinishedTimeout(30 * BeQt::Second);
    mclient->setPingInterval(5 * BeQt::Minute);
    connect(mclient, SIGNAL(authorizedChanged(bool)), this, SLOT(clientAuthorizedChanged(bool)));
    Settings::Texsample::loadPassword();
    updateCacheSettings();
    mgroupModel = new TGroupModel;
    minviteModel = new TInviteModel;
    mlabModel = new LabModel;
    muserModel = new UserModel(texsampleLocation);
    updateClientSettings();
    if (mcache->isEnabled()) {
        mgroupModel->update(mcache->groupInfoList(), mcache->lastRequestDateTime(Cache::GroupListRequest));
        minviteModel->update(mcache->inviteInfoList(), mcache->lastRequestDateTime(Cache::InviteListRequest));
        mlabModel->update(mcache->labInfoList(), mcache->lastRequestDateTime(Cache::LabListRequest));
        muserModel->update(mcache->userInfoList(), mcache->lastRequestDateTime(Cache::UserListRequest));
    }
    QTimer::singleShot(0, this,SLOT(checkTexsample()));
}

TexsampleCore::~TexsampleCore()
{
    mdestructorCalled = true;
    delete mclient;
    delete mgroupModel;
    delete minviteModel;
    delete mlabModel;
    delete muserModel;
    emit stopWaiting();
    while (!mfutureWatchers.isEmpty()) {
        Watcher *w = dynamic_cast<Watcher *>(mfutureWatchers.takeLast());
        if (!w)
            continue;
        w->waitForFinished();
    }
    BDirTools::rmdir(Application::location("texsample/labs", Application::UserResource));
    BDirTools::rmdir(Application::location("texsample/files", Application::UserResource));
}

/*============================== Public methods ============================*/

Cache *TexsampleCore::cache() const
{
    return mcache;
}

void TexsampleCore::clearCache()
{
    mcache->clear();
    mgroupModel->clear();
    minviteModel->clear();
    mlabModel->clear();
    muserModel->clear();
    updateLabList();
}

TNetworkClient *TexsampleCore::client() const
{
    return mclient;
}

TGroupModel *TexsampleCore::groupModel() const
{
    return mgroupModel;
}

TInviteModel *TexsampleCore::inviteModel() const
{
    return minviteModel;
}

LabModel *TexsampleCore::labModel() const
{
    return mlabModel;
}

void TexsampleCore::updateCacheSettings()
{
    mcache->setEnabled(Settings::Texsample::cachingEnabled());
    mclient->setCachingEnabled(mcache->isEnabled());
}

void TexsampleCore::updateClientSettings()
{
    if (mclient->hostName() != Settings::Texsample::host(true)) {
        mgroupModel->clear();
        minviteModel->clear();
        mlabModel->clear();
        muserModel->clear();
        if (!mclient->hostName().isEmpty())
            mcache->clear();
    }
    mclient->setHostName(Settings::Texsample::host(true));
    mclient->setLogin(Settings::Texsample::login());
    mclient->setPassword(Settings::Texsample::password().encryptedPassword());
    mclient->reconnect();
}

TUserModel *TexsampleCore::userModel() const
{
    return muserModel;
}

/*============================== Public slots ==============================*/

bool TexsampleCore::checkForNewVersion(bool persistent)
{
    if (!mclient->isValid(true)) {
        if (!bApp->showSettings(Application::TexsampleSettings))
            return false;
        updateClientSettings();
    }
    if (!mclient->isValid(true))
        return false;
    Future f = QtConcurrent::run(&checkForNewVersionFunction, persistent);
    Watcher *w = new Watcher;
    w->setFuture(f);
    connect(w, SIGNAL(finished()), this, SLOT(checkingForNewVersionFinished()));
    mfutureWatchers << w;
    return true;
}

bool TexsampleCore::checkForNewVersionPersistent()
{
    return checkForNewVersion(true);
}

void TexsampleCore::connectToServer()
{
    if (!mclient->isValid())
        showTexsampleSettings();
    mclient->connectToServer();
}

bool TexsampleCore::deleteLab(quint64 labId, QWidget *parent)
{
    if (!labId)
        return false;
    if (!mclient->isAuthorized())
        return false;
    if (!parent)
        parent = bApp->mostSuitableWindow();
    QMessageBox msg(parent);
    msg.setWindowTitle(tr("Deleting lab", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Question);
    msg.setText(tr("You are going to delete a lab. Do you want to continue?", "msgbox text"));
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Ok);
    if (msg.exec() != QMessageBox::Ok)
        return true;
    TDeleteLabRequestData requestData;
    requestData.setId(labId);
    TReply reply = mclient->performOperation(TOperation::DeleteLab, requestData, parent);
    if (!reply.success()) {
        QMessageBox msg(parent);
        msg.setWindowTitle(tr("Deleting lab error", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Failed to delete lab due to the following error:", "msgbox text"));
        msg.setInformativeText(reply.message());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return true;
    }
    mlabModel->removeLab(labId);
    mcache->removeData(TOperation::DeleteLab, labId);
    bApp->showStatusBarMessage(tr("Lab was successfully deleted", "message"));
    return true;
}

void TexsampleCore::disconnectFromServer()
{
    mclient->disconnectFromServer();
}

void TexsampleCore::editLab(quint64 labId)
{
    if (!labId)
        return;
    if (meditLabDialogs.contains(labId)) {
        QPointer<QWidget> wgt = meditLabDialogs.value(labId);
        if (!wgt.isNull())
            return wgt->activateWindow();
        meditLabDialogs.remove(labId);
    }
    if (mlabInfoDialogs.contains(labId)) {
        QPointer<QWidget> wgt = mlabInfoDialogs.value(labId);
        if (!wgt.isNull())
            return wgt->activateWindow();
        else
            mlabInfoDialogs.remove(labId);
    }
    if (!mclient->isAuthorized())
        return;
    if (mclient->userInfo().accessLevel().level() < TAccessLevel::SuperuserLevel
            && mlabModel->labInfo(labId).senderId() != mclient->userInfo().id()) {
        return;
    }
    BDialog *dlg = new BDialog;
    dlg->setProperty("lab_id", labId);
    LabInfoWidget *swgt = new LabInfoWidget(LabInfoWidget::EditMode);
    swgt->setModel(mlabModel);
    swgt->setClient(mclient);
    swgt->setCache(mcache);
    if (!swgt->setLab(labId))
        return dlg->deleteLater();
    BTranslation t = BTranslation::translate("TexsampleCore", "Editing lab: %1", "wgt windowTitle");
    t.setArgument(swgt->title());
    dlg->setWindowTitle(t.translate());
    new BDynamicTranslator(dlg, "windowTitle", t);
    dlg->setWidget(swgt);
    dlg->addButton(QDialogButtonBox::Cancel, SLOT(reject()));
    QPushButton *btnAccept = dlg->addButton(QDialogButtonBox::Ok, SLOT(accept()));
    btnAccept->setEnabled(swgt->hasValidInput());
    connect(swgt, SIGNAL(inputValidityChanged(bool)), btnAccept, SLOT(setEnabled(bool)));
    dlg->restoreGeometry(Settings::TexsampleCore::sendLabDialogGeometry());
    connect(dlg, SIGNAL(finished(int)), this, SLOT(editLabDialogFinished(int)));
    meditLabDialogs.insert(labId, dlg);
    dlg->show();
}

bool TexsampleCore::getExtraFile(quint64 labId, const QString &fileName, QWidget *parent)
{
    if (!labId || fileName.isEmpty())
        return false;
    if (!mclient->isAuthorized())
        return false;
    if (!parent)
        parent = bApp->mostSuitableWindow();
    TGetLabExtraFileRequestData requestData;
    requestData.setLabId(labId);
    requestData.setFileName(fileName);
    TReply reply = mclient->performOperation(TOperation::GetLabExtraFile, requestData, 5 * BeQt::Minute, parent);
    if (!reply.success()) {
        QMessageBox msg(bApp->mostSuitableWindow());
        msg.setWindowTitle(tr("Getting lab extra file error", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Failed to get lab extra file due to the following error:", "msgbox text"));
        msg.setInformativeText(reply.message());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return false;
    }
    TBinaryFile file = reply.data().value<TGetLabExtraFileReplyData>().file();
    QString path = Application::location("texsample/files", Application::UserResource)
            + "/" + BUuid::createUuid().toString(true);
    if (!file.save(path))
        return false;
    return bApp->openLocalFile(path + "/" + file.fileName());
}

bool TexsampleCore::getLab(quint64 labId, QWidget *parent)
{
    if (!labId)
        return false;
    if (!mclient->isAuthorized())
        return false;
    if (!parent)
        parent = bApp->mostSuitableWindow();
    TGetLabDataRequestData requestData;
    requestData.setLabId(labId);
    requestData.setOs(BeQt::osType());
    TReply reply = mclient->performOperation(TOperation::GetLabData, requestData, 5 * BeQt::Minute, parent);
    if (!reply.success()) {
        QMessageBox msg(bApp->mostSuitableWindow());
        msg.setWindowTitle(tr("Getting lab error", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Failed to get lab due to the following error:", "msgbox text"));
        msg.setInformativeText(reply.message());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return false;
    }
    TLabData data = reply.data().value<TGetLabDataReplyData>().data();
    bApp->showStatusBarMessage(tr("Lab was successfully downloaded", "message"));
    QString path = Application::location("texsample/labs", Application::UserResource)
            + "/" + BUuid::createUuid().toString(true);
    int type = data.type();
    const TLabApplication &app = data.application();
    QString url = data.url();
    if (TLabType::DesktopApplication == type || TLabType::WebApplication == type) {
        if (!app.save(path))
            return false;
        url = path + "/" + app.mainFile().fileName();
    }
    if (TLabType::DesktopApplication == type) {
        QtConcurrent::run(&runExecutable, url, path);
    } else {
        QWidget *wgt = new QWidget;
        wgt->setAttribute(Qt::WA_DeleteOnClose, true);
        BTranslation t = BTranslation::translate("TexsampleCore", "Lab", "wgt windowTitle");
        new BDynamicTranslator(wgt, "windowTitle", t);
        wgt->setWindowTitle(t.translate());
        QVBoxLayout *vlt = new QVBoxLayout(wgt);
          QToolBar *tbar = new QToolBar;
          vlt->addWidget(tbar);
          QWebView *wvw = new QWebView;
            wvw->setUrl(TLabType::Url == type ? QUrl::fromUserInput(url) : QUrl::fromLocalFile(url));
            QAction *act = wvw->page()->action(QWebPage::Back);
            act->setIcon(Application::icon("back"));
            act->setToolTip(tr("To the previous page", "tbtn toolTip"));
            tbar->addAction(act);
            act = wvw->page()->action(QWebPage::Forward);
            act->setIcon(Application::icon("forward"));
            act->setToolTip(tr("To the next page", "tbtn toolTip"));
            tbar->addAction(act);
          vlt->addWidget(wvw);
          QDialogButtonBox *dlgbbox = new QDialogButtonBox;
            dlgbbox->addButton(QDialogButtonBox::Close);
            connect(dlgbbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), wgt, SLOT(close()));
          vlt->addWidget(dlgbbox);
        wgt->show();
        if (TLabType::WebApplication == type)
            QtConcurrent::run(&waitForDestroyed, wgt, path);
    }
    return true;
}

void TexsampleCore::sendLab()
{
    if (!msendLabDialog.isNull())
        return msendLabDialog->activateWindow();
    if (!mclient->isAuthorized())
        return;
    BDialog *dlg = new BDialog;
    BTranslation t = BTranslation::translate("TexsampleCore", "Sending lab", "wgt windowTitle");
    dlg->setWindowTitle(t.translate());
    new BDynamicTranslator(dlg, "windowTitle", t);
    LabInfoWidget *swgt = new LabInfoWidget(LabInfoWidget::AddMode);
    swgt->setClient(mclient);
    swgt->setCache(mcache);
    swgt->restoreState(Settings::TexsampleCore::sendLabWidgetState());
    dlg->setWidget(swgt);
    dlg->addButton(QDialogButtonBox::Cancel, SLOT(reject()));
    QPushButton *btnAccept = dlg->addButton(QDialogButtonBox::Ok, SLOT(accept()));
    btnAccept->setEnabled(swgt->hasValidInput());
    connect(swgt, SIGNAL(inputValidityChanged(bool)), btnAccept, SLOT(setEnabled(bool)));
    dlg->restoreGeometry(Settings::TexsampleCore::sendLabDialogGeometry());
    connect(dlg, SIGNAL(finished(int)), this, SLOT(sendLabDialogFinished(int)));
    msendLabDialog = dlg;
    dlg->show();
}

bool TexsampleCore::showAccountManagementDialog(QWidget *parent)
{
    if (!mclient->isAuthorized())
        return false;
    TUserInfoWidget *uwgt = new TUserInfoWidget(TUserInfoWidget::EditSelfMode);
    uwgt->setClient(mclient);
    uwgt->setCache(mcache);
    uwgt->setModel(muserModel);
    quint64 userId = mclient->userInfo().id();
    if (!uwgt->setUser(userId))
        return false;
    if (!parent)
        parent = bApp->mostSuitableWindow();
    BDialog dlg(parent);
    dlg.setWindowTitle(tr("Account management", "dlg windowTitle"));
    dlg.setWidget(uwgt);
    dlg.addButton(QDialogButtonBox::Cancel, SLOT(reject()));
    QPushButton *btnAccept = dlg.addButton(QDialogButtonBox::Ok, SLOT(accept()));
    btnAccept->setEnabled(uwgt->hasValidInput());
    connect(uwgt, SIGNAL(inputValidityChanged(bool)), btnAccept, SLOT(setEnabled(bool)));
    dlg.restoreGeometry(Settings::TexsampleCore::accountManagementDialogGeometry());
    bool b = (dlg.exec() == BDialog::Accepted);
    Settings::TexsampleCore::setAccountManagementDialogGeometry(dlg.saveGeometry());
    if (!b)
        return false;
    TReply reply = mclient->performOperation(TOperation::EditSelf, uwgt->createRequestData(), parent);
    if (!reply.success()) {
        QMessageBox msg(parent);
        msg.setWindowTitle(tr("Editing account error", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Failed to edit account due to the following error:", "msgbox text"));
        msg.setInformativeText(reply.message());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return false;
    }
    muserModel->updateUser(userId, reply.data().value<TEditSelfReplyData>().userInfo());
    mcache->setData(TOperation::EditSelf, reply.requestDateTime(), reply.data(), userId);
    return true;
}

bool TexsampleCore::showConfirmEmailChangeDialog(QWidget *parent)
{
    if (!parent)
        parent = bApp->mostSuitableWindow();
    return TUserInfoWidget::showConfirmEmailChangeDialog(mclient, parent);
}

bool TexsampleCore::showConfirmRegistrationDialog(QWidget *parent)
{
    if (!parent)
        parent = bApp->mostSuitableWindow();
    BDialog dlg(parent);
    dlg.setWindowTitle(tr("Registration confirmation", "dlg windowTitle"));
    TConfirmationWidget *cwgt = new TConfirmationWidget;
    dlg.setWidget(cwgt);
    dlg.addButton(QDialogButtonBox::Cancel, SLOT(reject()));
    QPushButton *btnAccept = dlg.addButton(QDialogButtonBox::Ok, SLOT(accept()));
    btnAccept->setEnabled(cwgt->hasValidInput());
    connect(cwgt, SIGNAL(inputValidityChanged(bool)), btnAccept, SLOT(setEnabled(bool)));
    dlg.resize(450, 0);
    while (dlg.exec() == BDialog::Accepted) {
        TConfirmRegistrationRequestData requestData;
        requestData.setConfirmationCode(cwgt->code());
        TReply r = mclient->performAnonymousOperation(TOperation::ConfirmRegistration, requestData, parent);
        if (r.success()) {
            bApp->showStatusBarMessage(tr("You have successfully confirmed your account registration", "message"));
            return true;
        } else {
            QMessageBox msg(dlg.parentWidget());
            msg.setWindowTitle(tr("Registration confirmation error", "msgbox windowTitle"));
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Failed to confirm rgistration due to the following error:", "msgbox text"));
            msg.setInformativeText(r.message());
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
        }
    }
    return false;
}

void TexsampleCore::showGroupManagementWidget()
{
    if (!mgroupManagementDialog.isNull())
        return mgroupManagementDialog->activateWindow();
    BTranslation t = BTranslation::translate("TexsampleCore", "Group management", "wgt windowTitle");
    Dialog *dlg = new Dialog(&Settings::TexsampleCore::setGroupManagementDialogGeometry, t);
    TGroupWidget *gwgt = new TGroupWidget(mgroupModel);
    gwgt->setCache(mcache);
    gwgt->setClient(mclient);
    dlg->setWidget(gwgt);
    QByteArray geometry = Settings::TexsampleCore::groupManagementDialogGeometry();
    if (!geometry.isEmpty())
        dlg->restoreGeometry(geometry);
    else
        dlg->resize(800, 400);
    mgroupManagementDialog = dlg;
    dlg->show();
}

void TexsampleCore::showInviteManagementWidget()
{
    if (!minviteManagementDialog.isNull())
        return minviteManagementDialog->activateWindow();
    BTranslation t = BTranslation::translate("TexsampleCore", "Invite management", "wgt windowTitle");
    Dialog *dlg = new Dialog(&Settings::TexsampleCore::setInviteManagementDialogGeometry, t);
    TInviteWidget *iwgt = new TInviteWidget(minviteModel);
    iwgt->setCache(mcache);
    iwgt->setClient(mclient);
    dlg->setWidget(iwgt);
    QByteArray geometry = Settings::TexsampleCore::inviteManagementDialogGeometry();
    if (!geometry.isEmpty())
        dlg->restoreGeometry(geometry);
    else
        dlg->resize(800, 400);
    minviteManagementDialog = dlg;
    dlg->show();
}

bool TexsampleCore::showRecoverDialog(QWidget *parent)
{
    if (!mclient->isValid(true) && !showTexsampleSettings(parent))
        return false;
    if (!mclient->isValid(true))
        return false;
    BDialog dlg(parent ? parent : bApp->mostSuitableWindow());
    dlg.setWindowTitle(tr("Account recovery", "dlg windowTitle"));
    dlg.setWidget(new TRecoveryWidget(mclient));
    dlg.addButton(QDialogButtonBox::Close, &dlg, SLOT(reject()));
    dlg.resize(700, 0);
    dlg.exec();
    return true;
}

bool TexsampleCore::showRegisterDialog(QWidget *parent)
{
    if (!mclient->isValid(true) && !showTexsampleSettings(parent))
        return false;
    if (!mclient->isValid(true))
        return false;
    if (!parent)
        parent = bApp->mostSuitableWindow();
    BDialog dlg(parent);
    dlg.setWindowTitle(tr("Registration", "dlg windowTitle"));
    TUserInfoWidget *wgt = new TUserInfoWidget(TUserInfoWidget::RegisterMode);
    wgt->setClient(mclient);
    wgt->restorePasswordWidgetState(Settings::Texsample::passwordWidgetState());
    dlg.setWidget(wgt);
    QPushButton *btnOk = dlg.addButton(QDialogButtonBox::Ok, &dlg, SLOT(accept()));
    btnOk->setEnabled(wgt->hasValidInput());
    connect(wgt, SIGNAL(inputValidityChanged(bool)), btnOk, SLOT(setEnabled(bool)));
    dlg.addButton(QDialogButtonBox::Cancel, &dlg, SLOT(reject()));
    dlg.resize(800, 0);
    while (dlg.exec() == BDialog::Accepted) {
        TReply r = mclient->performAnonymousOperation(TOperation::Register, wgt->createRequestData(), parent);
        if (r.success()) {
            bApp->showStatusBarMessage(tr("You have successfully registered", "message"));
            QMessageBox msg(dlg.parentWidget());
            msg.setWindowTitle(tr("Registration", "msgbox windowTitle"));
            msg.setIcon(QMessageBox::Information);
            msg.setText(tr("You have successfully registered. Confirmation code was sent to your e-mail. "
                           "Please, click OK and enter that code to finish registration", "msgbox text"));
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
            if (showConfirmRegistrationDialog(dlg.parentWidget())) {
                Settings::Texsample::setLogin(wgt->login());
                Settings::Texsample::setPassword(wgt->password());
                updateClientSettings();
                if (mclient->isConnected())
                    mclient->reconnect();
                else
                    mclient->connectToServer();
            }
            break;
        } else {
            QMessageBox msg(dlg.parentWidget());
            msg.setWindowTitle(tr("Registration error", "msgbox windowTitle"));
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Failed to register due to the following error:", "msgbox text"));
            msg.setInformativeText(r.message());
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
        }
    }
    Settings::Texsample::setPasswordWidgetState(wgt->savePasswordWidgetState());
    return true;
}

void TexsampleCore::showLabInfo(quint64 labId)
{
    if (!labId)
        return;
    if (mlabInfoDialogs.contains(labId)) {
        QPointer<QWidget> wgt = mlabInfoDialogs.value(labId);
        if (!wgt.isNull())
            return wgt->activateWindow();
        else
            mlabInfoDialogs.remove(labId);
    }
    if (meditLabDialogs.contains(labId)) {
        QPointer<QWidget> wgt = meditLabDialogs.value(labId);
        if (!wgt.isNull())
            return wgt->activateWindow();
        meditLabDialogs.remove(labId);
    }
    LabInfoWidget *swgt = new LabInfoWidget(LabInfoWidget::ShowMode);
    swgt->setCache(mcache);
    swgt->setClient(mclient);
    swgt->setModel(mlabModel);
    if (!swgt->setLab(labId))
        return;
    BTranslation t = BTranslation::translate("TexsampleCore", "Lab: %1", "wgt windowTitle");
    t.setArgument(swgt->title());
    Dialog *dlg = new Dialog(&Settings::TexsampleCore::setLabInfoDialogGeometry, t);
    dlg->setWidget(swgt);
    QByteArray geometry = Settings::TexsampleCore::labInfoDialogGeometry();
    if (!geometry.isEmpty())
        dlg->restoreGeometry(geometry);
    else
        dlg->resize(800, 400);
    mlabInfoDialogs.insert(labId, dlg);
    dlg->show();
}

bool TexsampleCore::showTexsampleSettings(QWidget *parent)
{
    return bApp->showSettings(Application::TexsampleSettings, parent);
}

void TexsampleCore::showUserInfo(quint64 userId)
{
    if (!userId)
        return;
    if (muserInfoDialogs.contains(userId)) {
        QPointer<QWidget> wgt = muserInfoDialogs.value(userId);
        if (!wgt.isNull())
            return wgt->activateWindow();
        else
            muserInfoDialogs.remove(userId);
    }
    TUserInfoWidget *uwgt = new TUserInfoWidget(TUserInfoWidget::ShowMode);
    uwgt->setCache(mcache);
    uwgt->setClient(mclient);
    uwgt->setModel(muserModel);
    if (!uwgt->setUser(userId))
        return;
    BTranslation t = BTranslation::translate("TexsampleCore", "User: %1", "wgt windowTitle");
    t.setArgument(uwgt->login());
    Dialog *dlg = new Dialog(&Settings::TexsampleCore::setUserInfoDialogGeometry, t);
    dlg->setWidget(uwgt);
    QByteArray geometry = Settings::TexsampleCore::userInfoDialogGeometry();
    if (!geometry.isEmpty())
        dlg->restoreGeometry(geometry);
    else
        dlg->resize(800, 400);
    muserInfoDialogs.insert(userId, dlg);
    dlg->show();
}

void TexsampleCore::showUserManagementWidget()
{
    if (!muserManagementDialog.isNull())
        return muserManagementDialog->activateWindow();
    BTranslation t = BTranslation::translate("TexsampleCore", "User management", "wgt windowTitle");
    Dialog *dlg = new Dialog(&Settings::TexsampleCore::setUserManagementDialogGeometry, t);
    TUserWidget *uwgt = new TUserWidget(muserModel);
    uwgt->setCache(mcache);
    uwgt->setClient(mclient);
    dlg->setWidget(uwgt);
    QByteArray geometry = Settings::TexsampleCore::userManagementDialogGeometry();
    if (!geometry.isEmpty())
        dlg->restoreGeometry(geometry);
    else
        dlg->resize(800, 400);
    muserManagementDialog = dlg;
    dlg->show();
}

void TexsampleCore::updateLabList()
{
    if (!mclient->isAuthorized())
        return;
    TGetLabInfoListRequestData requestData;
    TReply reply = mclient->performOperation(TOperation::GetLabInfoList, requestData,
                                             mlabModel->lastUpdateDateTime(), bApp->mostSuitableWindow());
    if (!reply.success()) {
        QMessageBox msg(bApp->mostSuitableWindow());
        msg.setWindowTitle(tr("Updating lab list error", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Failed to update lab list due to the following error:", "msgbox text"));
        msg.setInformativeText(reply.message());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }
    TGetLabInfoListReplyData replyData = reply.data().value<TGetLabInfoListReplyData>();
    mlabModel->update(replyData.newLabs(), replyData.deletedLabs(), reply.requestDateTime());
    mcache->setData(TOperation::GetLabInfoList, reply.requestDateTime(), replyData);
}

/*============================== Static private methods ====================*/

TexsampleCore::CheckForNewVersionResult TexsampleCore::checkForNewVersionFunction(bool persistent)
{
    CheckForNewVersionResult result(persistent);
    TGetLatestAppVersionRequestData request;
    request.setClientInfo(TClientInfo::create());
    TNetworkClient *client = tSmp->client();
    if (!client->isValid(true))
        return result;
    TReply reply = client->performAnonymousOperation(TOperation::GetLatestAppVersion, request);
    result.success = reply.success();
    if (result.success) {
        TGetLatestAppVersionReplyData data = reply.data().value<TGetLatestAppVersionReplyData>();
        result.url = data.downloadUrl();
        result.version = data.version();
    }
    result.message = reply.message();
    return result;
}

void TexsampleCore::runExecutable(const QString &url, const QString &path)
{
    if (path.isEmpty() || url.isEmpty())
        return;
    BeQt::execProcess(QFileInfo(url).path(), BTextTools::wrapped(url, "\""), QStringList(), 5 * BeQt::Second, -1);
    BDirTools::rmdir(path);
}

void TexsampleCore::showMessageFunction(const QString &text, const QString &informativeText, bool error,
                                        QWidget *parentWidget)
{
    if (QThread::currentThread() != bApp->thread())
        return TNetworkClient::defaultShowMessageFunction(text, informativeText, error, parentWidget);
    QMessageBox msg(parentWidget ? parentWidget : bApp->mostSuitableWindow());
    if (error) {
        msg.setWindowTitle(tr("TeXSample error", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
    } else {
        msg.setWindowTitle(tr("TeXSample message", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Information);
    }
    msg.setText(text);
    msg.setInformativeText(informativeText);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

bool TexsampleCore::waitForConnectedFunction(BNetworkConnection *connection, int timeout, QWidget *parentWidget,
                                             QString *msg)
{
    if (!connection)
        return bRet(msg, tr("Null connection pointer", "error"), false);
    if (connection->error() != QAbstractSocket::UnknownSocketError)
        return bRet(msg, connection->errorString(), false);
    if (connection->isConnected())
        return bRet(msg, QString(), true);
    if (QThread::currentThread() != bApp->thread()) {
        BeQt::waitNonBlocking(connection, SIGNAL(connected()), tSmp, SIGNAL(stopWaiting()), timeout);
        if (connection->isConnected())
            return bRet(msg, QString(), true);
        return bRet(msg, connection->errorString(), false);
    }
    QProgressDialog pd(parentWidget ? parentWidget : bApp->mostSuitableWindow());
    pd.setWindowTitle(tr("Connecting to server", "pdlg windowTitle"));
    pd.setLabelText(tr("Connecting to server, please, wait...", "pdlg labelText"));
    pd.setMinimum(0);
    pd.setMaximum(0);
    QTimer::singleShot(timeout, &pd, SLOT(close()));
    if (pd.exec() == QProgressDialog::Rejected)
        return bRet(msg, tr("Connection cancelled by user", "error"), false);
    if (connection->isConnected())
        return bRet(msg, QString(), true);
    else if (connection->error() != QAbstractSocket::UnknownSocketError)
        return bRet(msg, connection->errorString(), false);
    else
        return bRet(msg, tr("An error occured, or connection timed out", "error"), false);
}

void TexsampleCore::waitForDestroyed(QWidget *wgt, const QString &path)
{
    if (!wgt || path.isEmpty())
        return;
    BeQt::waitNonBlocking(wgt, SIGNAL(destroyed()));
    BDirTools::rmdir(path);
}

bool TexsampleCore::waitForFinishedFunction(BNetworkOperation *op, int timeout, QWidget *parentWidget, QString *msg)
{
    if (!op)
        return bRet(msg, tr("Null operation pointer", "error"), false);
    if (op->isError())
        return bRet(msg, tr("An error occured during operation", "error"), false);
    if (op->isFinished())
        return bRet(msg, QString(), true);
    if (QThread::currentThread() != bApp->thread()) {
        QList<BeQt::Until> until;
        until << BeQt::until(op, SIGNAL(finished()));
        until << BeQt::until(op, SIGNAL(error()));
        until << BeQt::until(tSmp, SIGNAL(stopWaiting()));
        BeQt::waitNonBlocking(until, timeout);
        if (op->isFinished())
            return bRet(msg, QString(), true);
        return bRet(msg, tr("Operation timed out", "error"), false);
    }
    BOperationProgressDialog dlg(op, parentWidget ? parentWidget : bApp->mostSuitableWindow());
    dlg.setWindowTitle(tr("Executing request...", "opdlg windowTitle"));
    dlg.setAutoCloseInterval(0);
    if (timeout > 0)
        QTimer::singleShot(timeout, op, SLOT(cancel()));
    dlg.exec();
    if (op->isCancelled())
        return bRet(msg, tr("Operation cancelled by user", "error"), false);
    else if (op->isError())
        return bRet(msg, tr("An error occured during operation", "error"), false);
    else if (op->isFinished())
        return bRet(msg, QString(), true);
    else
        return bRet(msg, tr("Operation timed out", "error"), false);
}

/*============================== Private slots =============================*/

void TexsampleCore::checkingForNewVersionFinished()
{
    Watcher *w = dynamic_cast<Watcher *>(sender());
    if (!w)
        return;
    mfutureWatchers.removeAll(w);
    CheckForNewVersionResult result = w->result();
    delete w;
    if (mdestructorCalled)
        return;
    QPushButton *btnDisable = (!result.persistent && Settings::General::checkForNewVersionOnStartup()) ?
                new QPushButton(tr("Disable version checking", "btn text")) : 0;
    if (!result.success) {
        QMessageBox msg(bApp->mostSuitableWindow());
        msg.setWindowTitle(tr("Checking for new version failed", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Failed to check for new version. The following error occured:", "msgbox text"));
        msg.setInformativeText(result.message);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.addButton(btnDisable, QMessageBox::AcceptRole);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        if (msg.clickedButton() == btnDisable)
            Settings::General::setCheckForNewVersionOnStartup(false);
        return;
    }
    QMessageBox msg(bApp->mostSuitableWindow());
    msg.setWindowTitle(tr("New version", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Information);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.addButton(btnDisable, QMessageBox::AcceptRole);
    msg.setDefaultButton(QMessageBox::Ok);
    if (result.version.isValid() && result.version > BVersion(Application::applicationVersion())) {
        QString s = tr("A new version of the application is available", "msgbox text")
                + " (v" + result.version.toString(BVersion::Full) + ").";
        if (result.url.isValid()) {
            s += " " + tr("Click the following link to go to the download page:", "msgbox text")
                    + " <a href=\"" + result.url.toString() + "\">" + tr("download", "msgbox text") + "</a>";
        }
        msg.setText(s);
        msg.setInformativeText(tr("You should always use the latest application version. "
                                  "Bugs are fixed and new features are implemented in new versions.",
                                  "msgbox informativeText"));
        msg.exec();
    } else if (result.persistent) {
        msg.setText(tr("You are using the latest version.", "msgbox text"));
        msg.exec();
    }
    if (msg.clickedButton() == btnDisable)
        Settings::General::setCheckForNewVersionOnStartup(false);
}

void TexsampleCore::checkTexsample()
{
    bool b = true;
    if (!Settings::Texsample::hasTexsample()) {
        QMessageBox msg(bApp->mostSuitableWindow());
        msg.setWindowTitle(tr("TeXSample configuration", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Question);
        msg.setText(tr("It seems that you have not configured TeXSample service yet.\n"
                       "Would you like to do it now?", "msgbox text"));
        msg.setInformativeText(tr("To remove this notification, you have to configure or disable TeXSample service",
                                  "msgbox informativeText"));
        QPushButton *btnRegister = msg.addButton(tr("Register", "btn text"), QMessageBox::AcceptRole);
        QPushButton *btnConfig = msg.addButton(tr("I have an account", "btn text"), QMessageBox::AcceptRole);
        QPushButton *btnDisable = msg.addButton(tr("Disable TeXSample", "btn text"), QMessageBox::RejectRole);
        msg.addButton(tr("Not right now", "btn text"), QMessageBox::RejectRole);
        msg.setDefaultButton(btnConfig);
        msg.exec();
        if (msg.clickedButton() == btnRegister) {
            if (showRegisterDialog())
                Settings::Texsample::setConnectOnStartup(true);
        } else if (msg.clickedButton() == btnConfig) {
            if (showTexsampleSettings())
                mclient->connectToServer();
            else
                b = false;
        } else if (msg.clickedButton() == btnDisable) {
            Settings::Texsample::setConnectOnStartup(false);
            b = false;
        }
    } else if (Settings::Texsample::connectOnStartup()) {
        if (!mclient->isValid())
            b = showTexsampleSettings();
        mclient->connectToServer();
    }
    if (Settings::General::checkForNewVersionOnStartup() && (b || mclient->isValid(true)))
        checkForNewVersion();
}

void TexsampleCore::clientAuthorizedChanged(bool authorized)
{
    if (!authorized)
        return;
    updateLabList();
}

void TexsampleCore::editLabDialogFinished(int result)
{
    BDialog *dlg = qobject_cast<BDialog *>(sender());
    if (!dlg)
        return;
    LabInfoWidget *swgt = qobject_cast<LabInfoWidget *>(dlg->widget());
    if (!swgt)
        return;
    bool b = (BDialog::Accepted == result);
    Settings::TexsampleCore::setSendLabDialogGeometry(dlg->saveGeometry());
    if (!b)
        return dlg->deleteLater();
    if (!mclient->isAuthorized())
        return dlg->deleteLater();
    quint64 labId = dlg->property("lab_id").toULongLong();
    if (!labId)
        return dlg->deleteLater();
    QWidget *parent = bApp->mostSuitableWindow();
    TReply reply = mclient->performOperation(TOperation::EditLab, swgt->createRequestData(), 5 * BeQt::Minute, parent);
    if (!reply.success()) {
        QMessageBox msg(parent);
        msg.setWindowTitle(tr("Editing lab error", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Failed to edit lab due to the following error:", "msgbox text"));
        msg.setInformativeText(reply.message());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        dlg->deleteLater();
        return;
    }
    mlabModel->updateLab(labId, reply.data().value<TEditLabReplyData>().labInfo());
    mcache->setData(TOperation::EditLab, reply.requestDateTime(), reply.data());
    bApp->showStatusBarMessage(tr("Lab was successfully edited", "message"));
    dlg->deleteLater();
}

void TexsampleCore::sendLabDialogFinished(int result)
{
    BDialog *dlg = qobject_cast<BDialog *>(sender());
    if (!dlg)
        return;
    LabInfoWidget *swgt = qobject_cast<LabInfoWidget *>(dlg->widget());
    if (!swgt)
        return;
    bool b = (BDialog::Accepted == result);
    Settings::TexsampleCore::setSendLabDialogGeometry(dlg->saveGeometry());
    Settings::TexsampleCore::setSendLabWidgetState(swgt->saveState());
    if (!b)
        return dlg->deleteLater();
    QWidget *parent = bApp->mostSuitableWindow();
    TReply reply = mclient->performOperation(TOperation::AddLab, swgt->createRequestData(), 5 * BeQt::Minute, parent);
    if (!reply.success()) {
        QMessageBox msg(parent);
        msg.setWindowTitle(tr("Sending lab error", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Failed to send lab due to the following error:", "msgbox text"));
        msg.setInformativeText(reply.message());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        dlg->deleteLater();
        return;
    }
    TAddLabReplyData replyData = reply.data().value<TAddLabReplyData>();
    mlabModel->addLab(replyData.labInfo());
    mcache->setData(TOperation::AddLab, reply.requestDateTime(), replyData);
    bApp->showStatusBarMessage(tr("Lab was successfully sent", "message"));
    dlg->deleteLater();
}
