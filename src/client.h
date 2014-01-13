#ifndef CLIENT_H
#define CLIENT_H

class TOperationResult;
class TLabInfo;
class TService;
class TIdList;
class TInviteInfoList;
class TProjectFileList;

class BNetworkConnection;
class BNetworkOperation;
class BSignalDelayProxy;

#include <TUserInfo>
#include <TLabProject>
#include <TAccessLevel>
#include <TLabInfoList>
#include <TServiceList>

#include <QObject>
#include <QAbstractSocket>
#include <QByteArray>
#include <QString>
#include <QMap>
#include <QList>
#include <QDateTime>
#include <QImage>
#include <QUuid>
#include <QVariantMap>
#include <QVariantList>
#include <QStringList>

#define sClient Client::instance()

/*============================================================================
================================ Client ======================================
============================================================================*/

class Client : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        DisconnectedState,
        ConnectingState,
        ConnectedState,
        AuthorizedState,
        DisconnectingState
    };
public:
    static Client *instance();
    static bool hasAccessToService(const TService &s);
    static TOperationResult registerUser(const TUserInfo &info, QWidget *parent = 0);
    static TOperationResult getRecoveryCode(const QString &email, QWidget *parent = 0);
    static TOperationResult recoverAccount(const QString &email, const QString &code, const QByteArray &password,
                                           QWidget *parent = 0);
    static TOperationResult checkForNewVersions(QWidget *parent = 0);
    static TOperationResult checkForNewVersions(bool persistent, QWidget *parent = 0);
    static TOperationResult generateInvites(TInviteInfoList &invites, const QDateTime &expiresDT, quint8 count,
                                            const TServiceList &services, const QStringList &clabGroups,
                                            QWidget *parent = 0);
    static TOperationResult getInvitesList(TInviteInfoList &list, QWidget *parent = 0);
public:
    explicit Client(QObject *parent = 0);
    ~Client();
public:
    bool updateSettings();
    void setConnected(bool b);
    State state() const;
    bool canConnect() const;
    bool canDisconnect() const;
    bool isAuthorized() const;
    QString login() const;
    TAccessLevel accessLevel() const;
    TServiceList services() const;
    quint64 userId() const;
    TOperationResult addUser(const TUserInfo &info, const QStringList &clabGroups, QWidget *parent = 0);
    TOperationResult editUser(const TUserInfo &info, const QStringList &clabGroups, QWidget *parent = 0);
    TOperationResult updateAccount(TUserInfo info, QWidget *parent = 0);
    TOperationResult getUserInfo(quint64 id, TUserInfo &info, QStringList &clabGroups, QWidget *parent = 0);
    TOperationResult getUserInfo(const QString &login, TUserInfo &info, QStringList &clabGroups, QWidget *parent = 0);
    TOperationResult editClabGroups(const QStringList newGroups, const QStringList &deletedGroups,
                                    QWidget *parent = 0);
    TOperationResult getClabGroupsList(QStringList &list, QWidget *parent = 0);
    TOperationResult addLab(const TLabInfo &info, const TLabProject &webProject, const TProjectFileList &extraFiles,
                            QWidget *parent = 0);
    TOperationResult addLab(const TLabInfo &info, const TLabProject &linuxProject, const TLabProject &macProject,
                            const TLabProject &winProject, const TProjectFileList &extraFiles, QWidget *parent = 0);
    TOperationResult addLab(const TLabInfo &info, const QString &url, const TProjectFileList &extraFiles,
                            QWidget *parent = 0);
    TOperationResult editLab(const TLabInfo &info, const TLabProject &webProject, const QStringList &deletedExtraFiles,
                             const TProjectFileList &newExtraFiles, QWidget *parent = 0);
    TOperationResult editLab(const TLabInfo &info, const TLabProject &linuxProject, const TLabProject &macProject,
                             const TLabProject &winProject, const QStringList &deletedExtraFiles,
                             const TProjectFileList &newExtraFiles, QWidget *parent = 0);
    TOperationResult editLab(const TLabInfo &info, const QString &url, const QStringList &deletedExtraFiles,
                             const TProjectFileList &newExtraFiles, QWidget *parent = 0);
    TOperationResult deleteLab(quint64 id, const QString &reason, QWidget *parent = 0);
    TOperationResult updateLabsList(bool full = false, QWidget *parent = 0);
    TOperationResult showLab(quint64 id, QWidget *parent = 0);
    TOperationResult getExtraAttachedFile(quint64 labId, const QString &fileName, QWidget *parent = 0);
public slots:
    void connectToServer();
    void reconnect();
    void disconnectFromServer();
private:
    static void showProgressDialog(BNetworkOperation *op, QWidget *parent = 0);
    static QWidget *chooseParent(QWidget *supposed = 0);
    static void showConnectionErrorMessage(const QString &errorString);
    static void waitForDestroyed(QWidget *wgt, const QString &path);
    static void runExecutable(const QString &url, const QString &path);
private:
    void setState(State s, const TAccessLevel &alvl = TAccessLevel::NoLevel,
                  const TServiceList &list = TServiceList());
    void updateLabInfos(const TLabInfoList &newInfos, const TIdList &deletedInfos, const QDateTime &updateDT);
    QDateTime labInfosUpdateDateTime(Qt::TimeSpec spec = Qt::UTC) const;
private slots:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError err);
    void languageChanged();
signals:
    void loginChanged(const QString &login);
    void hostChanged(const QString &host);
    void stateChanged(Client::State s);
    void canConnectChanged(bool b);
    void canDisconnectChanged(bool b);
    void authorizedChanged(bool authorized);
    void accessLevelChanged(int lvl);
private:
    static const int ProgressDialogDelay;
    static const int MaxLabSize;
private:
    static Client *minstance;
private:
    BNetworkConnection *mconnection;
    QString mhost;
    QString mlogin;
    QByteArray mpassword;
    TAccessLevel maccessLevel;
    TServiceList mservices;
    quint64 mid;
    State mstate;
    bool mreconnect;
    mutable QDateTime mlabsListUpdateDT;
    BSignalDelayProxy *mlanguageChangeProxy;
private:
    Q_DISABLE_COPY(Client)
};

#endif // CLIENT_H
