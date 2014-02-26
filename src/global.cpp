#include "global.h"
#include "application.h"

#include <BPasswordWidget>

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariant>
#include <QSettings>

namespace Global
{

QByteArray pwdState;

//General

void setMultipleWindowsEnabled(bool enabled)
{
    bSettings->setValue("Core/multiple_windows_enabled", enabled);
}

void setCheckForNewVersions(bool b)
{
    bSettings->setValue("Core/check_for_new_versions", b);
}

//TeXSample

void setAutoconnection(bool enabled)
{
    bSettings->setValue("TeXSample/Client/autoconnection", enabled);
}

void setHost(const QString &host)
{
    bSettings->setValue("TeXSample/Client/host", host);
}

void setHostHistory(const QStringList &history)
{
    bSettings->setValue("TeXSample/Client/host_history", history);
}

void setLogin(const QString &login)
{
    bSettings->setValue("TeXSample/Client/login", login);
}

void setPasswordWidgetSate(const QByteArray &state)
{
    bSettings->setValue("TeXSample/Client/password_widget_state", state);
}

void setPasswordState(const QByteArray &state)
{
    pwdState = state;
}

void setPassword(const BPassword &pwd)
{
    setPasswordState(pwd.save(BPassword::AlwaysEncryptedMode));
}

void setPassword(const QByteArray &pwd, int charCountHint)
{
    setPassword(BPassword(QCryptographicHash::Sha1, pwd, charCountHint));
}

void setPassword(const QString &pwd)
{
    setPassword(BPassword(pwd));
}

//Network

void setProxyMode(ProxyMode m)
{
    if (!bRangeD(NoProxy, UserProxy).contains(m))
        return;
    bSettings->setValue("Network/Proxy/mode", (int) m);
}

void setProxyHost(const QString &host)
{
    bSettings->setValue("Network/Proxy/host", host);
}

void setProxyPort(int p)
{
    if (p < 0)
        return;
    bSettings->setValue("Network/Proxy/port", p);
}

void setProxyLogin(const QString &login)
{
    bSettings->setValue("Network/Proxy/login", login);
}

void setProxyPassword(const QString &pwd)
{
    bSettings->setValue("Network/Proxy/password", pwd);
}

//General

bool multipleWindowsEnabled()
{
    return bSettings->value("Core/multiple_windows_enabled", true).toBool();
}

bool checkForNewVersions()
{
    return bSettings->value("Core/check_for_new_versions", true).toBool();
}

//TeXSample

bool hasTexsample()
{
    return bSettings->contains("TeXSample/Client/autoconnection");
}

bool autoconnection()
{
    return bSettings->value("TeXSample/Client/autoconnection", true).toBool();
}

QString host()
{
    QString host = bSettings->value("TeXSample/Client/host", "auto_select").toString();
    return hostHistory().contains(host) ? host : "auto_select";
}

QStringList hostHistory()
{
    return bSettings->value("TeXSample/Client/host_history").toStringList();
}

QString login()
{
    return bSettings->value("TeXSample/Client/login").toString();
}

QByteArray passwordWidgetState()
{
    return bSettings->value("TeXSample/Client/password_widget_state").toByteArray();
}

QByteArray passwordState()
{
    return pwdState;
}

BPassword password()
{
    BPassword pwd;
    pwd.restore(passwordState());
    return pwd;
}

QByteArray encryptedPassword(int *charCountHint)
{
    return password().encryptedPassword(charCountHint);
}

void savePasswordState()
{
    BPasswordWidget *pwdwgt = new BPasswordWidget;
    pwdwgt->restoreWidgetState(passwordWidgetState());
    bSettings->setValue("TeXSample/Client/password_state", pwdwgt->savePassword() ? pwdState : QByteArray());
    delete pwdwgt;
}

void loadPasswordState()
{
    pwdState = bSettings->value("TeXSample/Client/password_state").toByteArray();
}

//Network

ProxyMode proxyMode()
{
    bool ok = false;
    int m = bSettings->value("Network/Proxy/mode").toInt(&ok);
    return (ok && bRangeD(NoProxy, UserProxy).contains(m)) ? static_cast<ProxyMode>(m) : NoProxy;
}

QString proxyHost()
{
    return bSettings->value("Network/Proxy/host").toString();
}

int proxyPort()
{
    bool ok = false;
    int p = bSettings->value("Network/Proxy/port").toInt(&ok);
    return (ok && p >= 0) ? p : 0;
}

QString proxyLogin()
{
    return bSettings->value("Network/Proxy/login").toString();
}

QString proxyPassword()
{
    return bSettings->value("Network/Proxy/password").toString();
}

}
