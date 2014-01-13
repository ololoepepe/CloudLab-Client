#include "client.h"
#include "application.h"
#include "texsamplesettingstab.h"
#include "labsmodel.h"
#include "global.h"

#include <TUserInfo>
#include <TOperationResult>
#include <TClientInfo>
#include <TLabProject>
#include <TAccessLevel>
#include <TLabInfo>
#include <TLabInfoList>
#include <TeXSample>
#include <TInviteInfoList>
#include <TIdList>
#include <TProjectFile>
#include <TProjectFileList>

#include <BNetworkConnection>
#include <BGenericSocket>
#include <BPasswordWidget>
#include <BeQtGlobal>
#include <BNetworkOperation>
#include <BNetworkOperationMetaData>
#include <BDirTools>
#include <BSignalDelayProxy>
#include <BOperationProgressDialog>
#include <BTextTools>
#include <BVersion>

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QAbstractSocket>
#include <QSettings>
#include <QVariant>
#include <QApplication>
#include <QMessageBox>
#include <QWidget>
#include <QVariantMap>
#include <QVariantList>
#include <QDateTime>
#include <QList>
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>
#include <QTextCodec>
#include <QPushButton>
#include <QImage>
#include <QBuffer>
#include <QUuid>
#include <QRegExp>
#include <QProgressDialog>
#include <QTimer>
#include <QUrl>
#include <QWebView>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtConcurrentRun>

#include <QDebug>

B_DECLARE_TRANSLATE_FUNCTION

static bool handleNoopRequest(BNetworkOperation *op)
{
    bLogger->logInfo(translate("Client", "Replying to connection test...", "log"));
    op->reply();
    if (!op->waitForFinished())
        bLogger->logCritical(translate("Client", "Operation error", "log"));
    return true;
}

/*============================================================================
================================ Client ======================================
============================================================================*/

/*============================== Static public methods =====================*/

Client *Client::instance()
{
    if (!minstance)
        minstance = new Client;
    return minstance;
}

bool Client::hasAccessToService(const TService &s)
{
    return instance()->mservices.contains(s);
}

TOperationResult Client::registerUser(const TUserInfo &info, QWidget *parent)
{
    if (!info.isValid(TUserInfo::RegisterContext))
        return TOperationResult(TMessage::InvalidUserInfoError);
    BNetworkConnection c(BGenericSocket::TcpSocket);
    QString host = Global::host();
    c.connectToHost(host.compare("auto_select") ? host : QString("texsample-server.no-ip.org"), Texsample::MainPort);
    parent = chooseParent(parent);
    if (!c.isConnected() && !c.waitForConnected(BeQt::Second / 2))
    {
        QProgressDialog pd(parent);
        pd.setWindowTitle(tr("Connecting to server", "pdlg windowTitle"));
        pd.setLabelText(tr("Connecting to server, please, wait...", "pdlg labelText"));
        pd.setMinimum(0);
        pd.setMaximum(0);
        QTimer::singleShot(10 * BeQt::Second, &pd, SLOT(close()));
        if (pd.exec() == QProgressDialog::Rejected)
        {
            c.close();
            return TOperationResult(TMessage::ClientOperationCanceledError);
        }
    }
    if (!c.isConnected())
    {
        c.close();
        return TOperationResult(TMessage::ClientConnectionTimeoutError);
    }
    QVariantMap out;
    out.insert("user_info", info);
    out.insert("locale", Application::locale());
    BNetworkOperation *op = c.sendRequest(Texsample::RegisterRequest, out);
    showProgressDialog(op, parent);
    c.close();
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::getRecoveryCode(const QString &email, QWidget *parent)
{
    if (email.isEmpty())
        return TOperationResult(TMessage::InvalidEmailError);
    BNetworkConnection c(BGenericSocket::TcpSocket);
    QString host = Global::host();
    c.connectToHost(host.compare("auto_select") ? host : QString("texsample-server.no-ip.org"), Texsample::MainPort);
    parent = chooseParent(parent);
    if (!c.isConnected() && !c.waitForConnected(BeQt::Second / 2))
    {
        QProgressDialog pd(parent);
        pd.setWindowTitle(tr("Connecting to server", "pdlg windowTitle"));
        pd.setLabelText(tr("Connecting to server, please, wait...", "pdlg labelText"));
        pd.setMinimum(0);
        pd.setMaximum(0);
        QTimer::singleShot(10 * BeQt::Second, &pd, SLOT(close()));
        if (pd.exec() == QProgressDialog::Rejected)
        {
            c.close();
            return TOperationResult(TMessage::ClientOperationCanceledError);
        }
    }
    if (!c.isConnected())
    {
        c.close();
        return TOperationResult(TMessage::ClientConnectionTimeoutError);
    }
    QVariantMap out;
    out.insert("email", email);
    out.insert("locale", Application::locale());
    BNetworkOperation *op = c.sendRequest(Texsample::GetRecoveryCodeRequest, out);
    showProgressDialog(op, parent);
    c.close();
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::recoverAccount(const QString &email, const QString &code, const QByteArray &password,
                                        QWidget *parent)
{
    if (email.isEmpty() || BeQt::uuidFromText(code).isNull() || password.isEmpty())
        return TOperationResult(TMessage::InvalidDataError);
    BNetworkConnection c(BGenericSocket::TcpSocket);
    QString host = Global::host();
    c.connectToHost(host.compare("auto_select") ? host : QString("texsample-server.no-ip.org"), Texsample::MainPort);
    parent = chooseParent(parent);
    if (!c.isConnected() && !c.waitForConnected(BeQt::Second / 2))
    {
        QProgressDialog pd(parent);
        pd.setWindowTitle(tr("Connecting to server", "pdlg windowTitle"));
        pd.setLabelText(tr("Connecting to server, please, wait...", "pdlg labelText"));
        pd.setMinimum(0);
        pd.setMaximum(0);
        QTimer::singleShot(10 * BeQt::Second, &pd, SLOT(close()));
        if (pd.exec() == QProgressDialog::Rejected)
        {
            c.close();
            return TOperationResult(TMessage::ClientOperationCanceledError);
        }
    }
    if (!c.isConnected())
    {
        c.close();
        return TOperationResult(TMessage::ClientConnectionTimeoutError);
    }
    QVariantMap out;
    out.insert("email", email);
    out.insert("recovery_code", code);
    out.insert("password", password);
    out.insert("locale", Application::locale());
    BNetworkOperation *op = c.sendRequest(Texsample::RecoverAccountRequest, out);
    showProgressDialog(op, parent);
    c.close();
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::checkForNewVersions(QWidget *parent)
{
    return checkForNewVersions(false, parent);
}

TOperationResult Client::checkForNewVersions(bool persistent, QWidget *parent)
{
    BNetworkConnection c(BGenericSocket::TcpSocket);
    QString host = Global::host();
    c.connectToHost(host.compare("auto_select") ? host : QString("texsample-server.no-ip.org"), Texsample::MainPort);
    parent = chooseParent(parent);
    if (!c.isConnected() && !c.waitForConnected(BeQt::Second / 2))
    {
        QProgressDialog pd(parent);
        pd.setWindowTitle(tr("Connecting to server", "pdlg windowTitle"));
        pd.setLabelText(tr("Connecting to server, please, wait...", "pdlg labelText"));
        pd.setMinimum(0);
        pd.setMaximum(0);
        QTimer::singleShot(10 * BeQt::Second, &pd, SLOT(close()));
        if (pd.exec() == QProgressDialog::Rejected)
        {
            c.close();
            return TOperationResult(TMessage::ClientOperationCanceledError);
        }
    }
    if (!c.isConnected())
    {
        c.close();
        return TOperationResult(TMessage::ClientConnectionTimeoutError);
    }
    QVariantMap out;
    out.insert("client_info", TClientInfo::createInfo());
    BNetworkOperation *op = c.sendRequest(Texsample::GetLatestAppVersionRequest, out);
    showProgressDialog(op, parent);
    c.close();
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    BVersion ver = in.value("version").value<BVersion>();
    QString url = in.value("url").toString();
    QMessageBox msg(parent);
    msg.setWindowTitle(tr("New version", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Information);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    if (ver.isValid() && ver > BVersion(QApplication::applicationVersion()))
    {
        msg.setText(tr("A new version of the application is available", "msgbox text")
                    + " (v" + ver.toString(BVersion::Full) + "). " +
                    tr("Click the following link to go to the download page:", "msgbox text")
                    + " <a href=\"" + url + "\">" + tr("download", "msgbox text") + "</a>");
        msg.exec();
    }
    else if (persistent)
    {
        msg.setText(tr("You are using the latest version.", "msgbox text"));
        msg.exec();
    }
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::generateInvites(TInviteInfoList &invites, const QDateTime &expiresDT, quint8 count,
                                         const TServiceList &services, const QStringList &clabGroups, QWidget *parent)
{
    if (!instance()->isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!count || count > Texsample::MaximumInvitesCount)
        return TOperationResult(TMessage::ClientInvalidInvitesCountError);
    QVariantMap out;
    out.insert("expiration_dt", expiresDT);
    out.insert("count", count ? count : 1);
    out.insert("services", services);
    out.insert("clab_groups", clabGroups);
    BNetworkOperation *op = instance()->mconnection->sendRequest(Texsample::GenerateInvitesRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    invites = in.value("invite_infos").value<TInviteInfoList>();
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::getInvitesList(TInviteInfoList &list, QWidget *parent)
{
    if (!instance()->isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    BNetworkOperation *op = instance()->mconnection->sendRequest(Texsample::GetInvitesListRequest);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    list = in.value("invite_infos").value<TInviteInfoList>();
    return in.value("operation_result").value<TOperationResult>();
}

/*============================== Public constructors =======================*/

Client::Client(QObject *parent) :
    QObject(parent)
{
    mstate = DisconnectedState;
    mreconnect = false;
    mconnection = new BNetworkConnection(BGenericSocket::TcpSocket, this);
    mconnection->installRequestHandler(BNetworkConnection::operation(BNetworkConnection::NoopOperation),
                                       &handleNoopRequest);
    connect(mconnection, SIGNAL(connected()), this, SLOT(connected()));
    connect(mconnection, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(mconnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    mhost = Global::host();
    mlogin = Global::login();
    mpassword = Global::encryptedPassword();
    mid = 0;
    mlanguageChangeProxy = new BSignalDelayProxy(this);
    mlanguageChangeProxy->setConnection(bApp, SIGNAL(languageChanged()), this, SLOT(languageChanged()));
}

Client::~Client()
{
    delete sModel;
}

/*============================== Public methods ============================*/

bool Client::updateSettings()
{
    QString login = Global::login();
    QByteArray password = Global::encryptedPassword();
    QString host = Global::host();
    bool b = false;
    if (host != mhost || login != mlogin || password != mpassword)
    {
        bool bcc = canConnect();
        if (host != mhost)
        {
            mhost = host;
            emit hostChanged(host);
        }
        if (login != mlogin)
        {
            mlogin = login;
            emit loginChanged(login);
            sModel->clear();
            mlabsListUpdateDT = QDateTime().toUTC();
        }
        mpassword = password;
        bool bccn = canConnect();
        if (bcc != bccn)
            emit canConnectChanged(bccn);
        if (ConnectingState == mstate || ConnectedState == mstate || AuthorizedState == mstate)
            reconnect();
        b = true;
    }
    return b;
}

void Client::setConnected(bool b)
{
   if (b)
       connectToServer();
   else
       disconnectFromServer();
}

Client::State Client::state() const
{
    return mstate;
}

bool Client::canConnect() const
{
    return (DisconnectedState == mstate && !mhost.isEmpty() && !mlogin.isEmpty());
}

bool Client::canDisconnect() const
{
    return (ConnectingState == mstate || ConnectedState == mstate || AuthorizedState == mstate);
}

bool Client::isAuthorized() const
{
    return (AuthorizedState == mstate);
}

QString Client::login() const
{
    return mlogin;
}

TAccessLevel Client::accessLevel() const
{
    return maccessLevel;
}

TServiceList Client::services() const
{
    return mservices;
}

quint64 Client::userId() const
{
    return mid;
}

TOperationResult Client::addUser(const TUserInfo &info, const QStringList &clabGroups, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!info.isValid(TUserInfo::AddContext))
        return TOperationResult(TMessage::InvalidUserInfoError);
    QVariantMap out;
    out.insert("user_info", info);
    out.insert("clab_groups", clabGroups);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::AddUserRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::editUser(const TUserInfo &info, const QStringList &clabGroups, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!info.isValid(TUserInfo::EditContext))
        return TOperationResult(TMessage::InvalidUserInfoError);
    QVariantMap out;
    out.insert("user_info", info);
    out.insert("edit_clab", true);
    out.insert("clab_groups", clabGroups);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::EditUserRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::updateAccount(TUserInfo info, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    info.setId(mid);
    if (!info.isValid(TUserInfo::UpdateContext))
        return TOperationResult(TMessage::InvalidUserInfoError);
    QVariantMap out;
    out.insert("user_info", info);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::UpdateAccountRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::getUserInfo(quint64 id, TUserInfo &info, QStringList &clabGroups, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!id)
        return TOperationResult(TMessage::InvalidUserIdError);
    QVariantMap out;
    out.insert("user_id", id);
    out.insert("clab_groups", true);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::GetUserInfoRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    info = in.value("user_info").value<TUserInfo>();
    clabGroups = in.value("clab_groups").toStringList();
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::getUserInfo(const QString &login, TUserInfo &info, QStringList &clabGroups, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (login.isEmpty())
        return TOperationResult(TMessage::InvalidLoginError);
    QVariantMap out;
    out.insert("user_login", login);
    out.insert("clab_groups", true);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::GetUserInfoRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    info = in.value("user_info").value<TUserInfo>();
    clabGroups = in.value("clab_groups").toStringList();
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::editClabGroups(const QStringList newGroups, const QStringList &deletedGroups, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (newGroups.isEmpty() && deletedGroups.isEmpty())
        return TOperationResult(TMessage::InvalidDataError);
    QVariantMap out;
    out.insert("new_groups", newGroups);
    out.insert("deleted_groups", deletedGroups);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::EditClabGroupsRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::getClabGroupsList(QStringList &list, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::GetClabGroupsListRequest);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    list = in.value("groups_list").toStringList();
    return in.value("operation_result").value<TOperationResult>();
}

TOperationResult Client::addLab(const TLabInfo &info, const TLabProject &webProject,
                                const TProjectFileList &extraFiles, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!info.isValid(TLabInfo::AddContext) || !webProject.isValid())
        return TOperationResult(TMessage::InvalidDataError);
    QVariantMap out;
    out.insert("web_project", webProject);
    out.insert("lab_info", info);
    out.insert("extra_files", extraFiles);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::AddLabRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (r)
        updateLabsList();
    return r;
}

TOperationResult Client::addLab(const TLabInfo &info, const TLabProject &linuxProject, const TLabProject &macProject,
                                const TLabProject &winProject, const TProjectFileList &extraFiles, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!info.isValid(TLabInfo::AddContext)
            || (!linuxProject.isValid() && !macProject.isValid() && !winProject.isValid()))
        return TOperationResult(TMessage::InvalidDataError);
    QVariantMap out;
    if (linuxProject.isValid())
        out.insert("linux_project", linuxProject);
    if (macProject.isValid())
        out.insert("mac_project", macProject);
    if (winProject.isValid())
        out.insert("win_project", winProject);
    out.insert("lab_info", info);
    out.insert("extra_files", extraFiles);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::AddLabRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (r)
        updateLabsList();
    return r;
}

TOperationResult Client::addLab(const TLabInfo &info, const QString &url, const TProjectFileList &extraFiles,
                                QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!info.isValid(TLabInfo::AddContext) || url.isEmpty())
        return TOperationResult(TMessage::InvalidDataError);
    QVariantMap out;
    out.insert("lab_url", url);
    out.insert("lab_info", info);
    out.insert("extra_files", extraFiles);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::AddLabRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (r)
        updateLabsList();
    return r;
}

TOperationResult Client::editLab(const TLabInfo &info, const TLabProject &webProject,
                                 const QStringList &deletedExtraFiles, const TProjectFileList &newExtraFiles,
                                 QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!info.isValid(TLabInfo::EditContext))
        return TOperationResult(TMessage::InvalidDataError);
    QVariantMap out;
    if (webProject.isValid())
        out.insert("web_project", webProject);
    out.insert("lab_info", info);
    out.insert("new_extra_files", newExtraFiles);
    out.insert("deleted_extra_files", deletedExtraFiles);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::EditLabRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (r)
        updateLabsList();
    return r;
}

TOperationResult Client::editLab(const TLabInfo &info, const TLabProject &linuxProject, const TLabProject &macProject,
                                 const TLabProject &winProject, const QStringList &deletedExtraFiles,
                                 const TProjectFileList &newExtraFiles, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!info.isValid(TLabInfo::EditContext))
        return TOperationResult(TMessage::InvalidDataError);
    QVariantMap out;
    if (linuxProject.isValid())
        out.insert("linux_project", linuxProject);
    if (macProject.isValid())
        out.insert("mac_project", macProject);
    if (winProject.isValid())
        out.insert("win_project", winProject);
    out.insert("lab_info", info);
    out.insert("new_extra_files", newExtraFiles);
    out.insert("deleted_extra_files", deletedExtraFiles);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::EditLabRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (r)
        updateLabsList();
    return r;
}

TOperationResult Client::editLab(const TLabInfo &info, const QString &url, const QStringList &deletedExtraFiles,
                                 const TProjectFileList &newExtraFiles, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!info.isValid(TLabInfo::EditContext))
        return TOperationResult(TMessage::InvalidDataError);
    QVariantMap out;
    if (!url.isEmpty())
        out.insert("lab_url", url);
    out.insert("lab_info", info);
    out.insert("new_extra_files", newExtraFiles);
    out.insert("deleted_extra_files", deletedExtraFiles);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::EditLabRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (r)
        updateLabsList();
    return r;
}

TOperationResult Client::deleteLab(quint64 id, const QString &reason, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!id)
        return TOperationResult(TMessage::InvalidSampleIdError);
    QVariantMap out;
    out.insert("lab_id", id);
    out.insert("reason", reason);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::DeleteLabRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (r)
        updateLabsList();
    return r;
}

TOperationResult Client::updateLabsList(bool full, QWidget *parent)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    QVariantMap out;
    out.insert("update_dt", !full ? labInfosUpdateDateTime() : QDateTime());
    BNetworkOperation *op = mconnection->sendRequest(Texsample::GetLabsListRequest, out);
    showProgressDialog(op, parent);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (r)
        updateLabInfos(in.value("new_lab_infos").value<TLabInfoList>(), in.value("deleted_lab_infos").value<TIdList>(),
                       in.value("update_dt").toDateTime());
    return r;
}

TOperationResult Client::showLab(quint64 id, QWidget *)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!id)
        return TOperationResult(TMessage::InvalidLabIdError);
    QVariantMap out;
    out.insert("lab_id", id);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::GetLabRequest, out);
    showProgressDialog(op);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (!r)
        return r;
#if defined(Q_OS_WIN)
        QString path = Application::location(Application::DataPath, Application::SharedResources) + "/labs/"
                + BeQt::pureUuidText(QUuid::createUuid());
#else
        QString path = QDir::tempPath() + "/clab-client/" + BeQt::pureUuidText(QUuid::createUuid());
#endif
    TLabInfo::Type t = TLabInfo::typeFromInt(in.value("type").toInt());
    QString url = in.value("url").toString();
    TLabProject p = in.value("project").value<TLabProject>();
    if (p.isValid())
    {
        if (!p.save(path))
            return TOperationResult(TMessage::ClientFileSystemError);
        url = p.mainFilePath(path);
    }
    if (TLabInfo::DesktopType == t)
    {
        QtConcurrent::run(&Client::runExecutable, url, path);
    }
    else
    {
        QWidget *wgt = new QWidget;
        wgt->setAttribute(Qt::WA_DeleteOnClose, true);
        wgt->setWindowTitle(tr("Lab", "wgt windowTitle"));
        QVBoxLayout *vlt = new QVBoxLayout(wgt);
          QWebView *wvw = new QWebView;
            wvw->setUrl(TLabInfo::UrlType == t ? QUrl::fromUserInput(url) : QUrl::fromLocalFile(url));
          vlt->addWidget(wvw);
          QDialogButtonBox *dlgbbox = new QDialogButtonBox;
            dlgbbox->addButton(QDialogButtonBox::Close);
            connect(dlgbbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), wgt, SLOT(close()));
          vlt->addWidget(dlgbbox);
        wgt->show();
        if (TLabInfo::WebType == t)
            QtConcurrent::run(&Client::waitForDestroyed, wgt, path);
    }
    return TOperationResult(true);
}

TOperationResult Client::getExtraAttachedFile(quint64 labId, const QString &fileName, QWidget *)
{
    if (!isAuthorized())
        return TOperationResult(TMessage::NotAuthorizedError);
    if (!labId)
        return TOperationResult(TMessage::InvalidLabIdError);
    if (fileName.isEmpty())
        return TOperationResult(TMessage::InvalidDataError);
    QVariantMap out;
    out.insert("lab_id", labId);
    out.insert("file_name", fileName);
    BNetworkOperation *op = mconnection->sendRequest(Texsample::GetLabExtraAttachedFileRequest, out);
    showProgressDialog(op);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return TOperationResult(TMessage::ClientOperationError);
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (!r)
        return r;
    TProjectFile pf = in.value("file").value<TProjectFile>();
    QString path = QDir::tempPath() + "/clab-client/files/" + BeQt::pureUuidText(QUuid::createUuid());
    if (!pf.save(path))
        return TOperationResult(TMessage::ClientFileSystemError);
    QDesktopServices::openUrl(QUrl::fromLocalFile(path + "/" + pf.fileName()));
    return TOperationResult(true);
}

/*============================== Public slots ==============================*/

void Client::connectToServer()
{
    if (!canConnect() || (Global::encryptedPassword().isEmpty() && !Application::showLoginDialog()))
        return;
    if (Global::encryptedPassword().isEmpty())
    {
        QMessageBox msg( Application::mostSuitableWindow() );
        msg.setWindowTitle( tr("No password", "msgbox windowTitle") );
        msg.setIcon(QMessageBox::Information);
        msg.setText( tr("You have not specified your password", "msgbox text") );
        msg.setInformativeText( tr("You can't connect without password", "msgbox informativeText") );
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }
    setState(ConnectingState);
    mconnection->connectToHost(mhost.compare("auto_select") ? mhost : QString("texsample-server.no-ip.org"),
                               Texsample::MainPort);
}

void Client::reconnect()
{
    if (DisconnectedState == mstate || DisconnectingState == mstate)
        return;
    mreconnect = true;
    disconnectFromServer();
}

void Client::disconnectFromServer()
{
    if (!canDisconnect())
        return;
    if (ConnectingState == mstate)
    {
        mconnection->abort();
        setState(DisconnectedState);
    }
    else
    {
        setState(DisconnectingState);
        mconnection->disconnectFromHost();
    }
}

/*============================== Static private methods ====================*/

void Client::showProgressDialog(BNetworkOperation *op, QWidget *parent)
{
    if (!op)
        return;
    if (op->waitForFinished(ProgressDialogDelay))
        return;
    BOperationProgressDialog dlg(op, chooseParent(parent));
    dlg.setWindowTitle(tr("Executing request...", "opdlg windowTitle"));
    dlg.setAutoCloseInterval(0);
    dlg.exec();
}

QWidget *Client::chooseParent(QWidget *supposed)
{
    return supposed ? supposed : Application::mostSuitableWindow();
}

void Client::showConnectionErrorMessage(const QString &errorString)
{
    QMessageBox msg(Application::mostSuitableWindow());
    msg.setWindowTitle(tr("TeXSample connection error", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Critical);
    msg.setText(tr("The following connection error occured:", "msgbox text") + "\n" + errorString);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

void Client::waitForDestroyed(QWidget *wgt, const QString &path)
{
    if (!wgt || path.isEmpty())
        return;
    BeQt::waitNonBlocking(wgt, SIGNAL(destroyed()));
    BDirTools::rmdir(path);
}

void Client::runExecutable(const QString &url, const QString &path)
{
    if (path.isEmpty() || url.isEmpty())
        return;
    BeQt::execProcess(BTextTools::wrapped(url), QStringList(), 5 * BeQt::Second, -1);
    BDirTools::rmdir(path);
}

/*============================== Private methods ===========================*/

void Client::setState(State s, const TAccessLevel &alvl, const TServiceList &list)
{
    if (s == mstate)
        return;
    bool b = (AuthorizedState == s || AuthorizedState == mstate);
    bool bcc = canConnect();
    bool bcd = canDisconnect();
    mstate = s;
    emit stateChanged(s);
    if (b)
        emit authorizedChanged(AuthorizedState == mstate);
    TAccessLevel palvl = maccessLevel;
    maccessLevel = alvl;
    mservices = list;
    if (palvl != alvl)
        emit accessLevelChanged(alvl);
    bool bccn = canConnect();
    bool bcdn = canDisconnect();
    if (bcc != bccn)
        emit canConnectChanged(bccn);
    if (bcd != bcdn)
        emit canDisconnectChanged(bcdn);
}

void Client::updateLabInfos(const TLabInfoList &newInfos, const TIdList &deletedInfos, const QDateTime &updateDT)
{
    mlabsListUpdateDT = updateDT.toUTC();
    sModel->removeLabs(deletedInfos);
    sModel->insertLabs(newInfos);
}

QDateTime Client::labInfosUpdateDateTime(Qt::TimeSpec spec) const
{
    return mlabsListUpdateDT.toTimeSpec(spec);
}

/*============================== Private slots =============================*/

void Client::connected()
{
    setState(ConnectedState);
    QVariantMap out;
    out.insert("login", mlogin);
    out.insert("password", mpassword);
    out.insert("client_info", TClientInfo::createInfo());
    BNetworkOperation *op = mconnection->sendRequest(Texsample::AuthorizeRequest, out);
    showProgressDialog(op);
    QVariantMap in = op->variantData().toMap();
    op->deleteLater();
    if (op->isError())
        return;
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (r)
    {
        mid = in.value("user_id").toULongLong();
        setState(AuthorizedState, in.value("access_level").value<TAccessLevel>(),
                 in.value("services").value<TServiceList>());
        updateLabsList();
    }
    else
    {
        disconnectFromServer();
        showConnectionErrorMessage(r.messageString());
    }
}

void Client::disconnected()
{
    setState(DisconnectedState);
    mid = 0;
    if (mreconnect)
    {
        mreconnect = false;
        connectToServer();
    }
}

void Client::error(QAbstractSocket::SocketError)
{
    setState(DisconnectedState);
    mid = 0;
    QString errorString = mconnection->errorString();
    if (mconnection->isConnected())
        mconnection->close();
    showConnectionErrorMessage(errorString);
}

void Client::languageChanged()
{
    if (!isAuthorized())
        return;
    QVariantMap out;
    out.insert("locale", BApplication::locale());
    BNetworkOperation *op = mconnection->sendRequest(Texsample::ChangeLocaleRequest, out);
    showProgressDialog(op);
    op->deleteLater();
}

/*============================== Static private constants ==================*/

const int Client::ProgressDialogDelay = BeQt::Second / 2;
const int Client::MaxLabSize = 199 * BeQt::Megabyte;

/*============================== Static private members ====================*/

Client *Client::minstance = 0;
