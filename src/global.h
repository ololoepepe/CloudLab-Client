#ifndef GLOBAL_H
#define GLOBAL_H

class BPassword;

class QByteArray;

#include <QStringList>
#include <QMap>
#include <QString>

namespace Global
{

enum ProxyMode
{
    NoProxy = 0,
    SystemProxy,
    UserProxy
};

//General
void setMultipleWindowsEnabled(bool enabled);
void setCheckForNewVersions(bool b);
//TeXSample
void setAutoconnection(bool enabled);
void setHost(const QString &host);
void setHostHistory(const QStringList &history);
void setLogin(const QString &login);
void setPasswordWidgetSate(const QByteArray &state);
void setPasswordState(const QByteArray &state);
void setPassword(const BPassword &pwd);
void setPassword(const QByteArray &pwd, int charCountHint = 0);
void setPassword(const QString &pwd);
//Network
void setProxyMode(ProxyMode m);
void setProxyHost(const QString &host);
void setProxyPort(int p);
void setProxyLogin(const QString &login);
void setProxyPassword(const QString &pwd);
//General
bool multipleWindowsEnabled();
bool checkForNewVersions();
//TeXSample
bool hasTexsample();
bool autoconnection();
QString host();
QStringList hostHistory();
QString login();
QByteArray passwordWidgetState();
QByteArray passwordState();
BPassword password();
QByteArray encryptedPassword(int *charCountHint = 0);
void savePasswordState();
void loadPasswordState();
//Network
ProxyMode proxyMode();
QString proxyHost();
int proxyPort();
QString proxyLogin();
QString proxyPassword();

}

#endif // GLOBAL_H
