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

//General

void setMultipleWindowsEnabled(bool enabled)
{
    bSettings->setValue("Core/multiple_windows_enabled", enabled);
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

void setPasswordSate(const QByteArray &state)
{
    bSettings->setValue("TeXSample/Client/password_state", state);
}

void setPassword(const BPassword &pwd)
{
    setPasswordSate(pwd.save(BPassword::AlwaysEncryptedMode));
}

void setPassword(const QByteArray &pwd, int charCountHint)
{
    setPassword(BPassword(QCryptographicHash::Sha1, pwd, charCountHint));
}

void setPassword(const QString &pwd)
{
    setPassword(BPassword(pwd));
}

//General

bool multipleWindowsEnabled()
{
    return bSettings->value("Core/multiple_windows_enabled", true).toBool();
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
    return bSettings->value("TeXSample/Client/password_state").toByteArray();
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

}
