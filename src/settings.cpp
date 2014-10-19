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

#include "settings.h"

#include "application.h"

#include <BPassword>
#include <BPasswordWidget>

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariant>

/*============================================================================
================================ Settings ====================================
============================================================================*/

namespace Settings
{

/*============================================================================
================================ General =====================================
============================================================================*/

namespace General
{

static const QString RootPath = "Core";
static const QString CheckForNewVersionOnStartupPath = RootPath + "/check_for_new_version_on_startup";
static const QString MultipleWindowsEnabledPath = RootPath + "/multiple_windows_enabled";

bool checkForNewVersionOnStartup()
{
    return bSettings->value(CheckForNewVersionOnStartupPath, true).toBool();
}

bool multipleWindowsEnabled()
{
    return bSettings->value(MultipleWindowsEnabledPath, false).toBool();
}

void setCheckForNewVersionOnStartup(bool b)
{
    bSettings->setValue(CheckForNewVersionOnStartupPath, b);
}

void setMultipleWindowsEnabled(bool enabled)
{
    bSettings->setValue(MultipleWindowsEnabledPath, enabled);
}

}

/*============================================================================
================================ MainWindow ==================================
============================================================================*/

namespace MainWindow
{

static const QString RootPath = "MainWindow";
static const QString WindowGeometryPath = RootPath + "/window_geometry";
static const QString WindowStatePath = RootPath + "/window_state";

void setWindowGeometry(const QByteArray &geometry)
{
    bSettings->setValue(WindowGeometryPath, geometry);
}

void setWindowState(const QByteArray &state)
{
    bSettings->setValue(WindowStatePath, state);
}

QByteArray windowGeometry()
{
    return bSettings->value(WindowGeometryPath).toByteArray();
}

QByteArray windowState()
{
    return bSettings->value(WindowStatePath).toByteArray();
}

}

/*============================================================================
================================ Network =====================================
============================================================================*/

namespace Network
{

static const QString RootPath = "Network";
static const QString ProxyPath = RootPath + "/Proxy";
static const QString ProxyHostPath = ProxyPath + "/host";
static const QString ProxyLoginPath = ProxyPath + "/login";
static const QString ProxyModePath = ProxyPath + "/mode";
static const QString ProxyPasswordPath = ProxyPath + "/password";
static const QString ProxyPortPath = ProxyPath + "/port";

QList<ProxyMode> allProxyModes()
{
    return QList<ProxyMode>() << NoProxy << SystemProxy << UserProxy;
}

QString proxyHost()
{
    return bSettings->value(ProxyHostPath).toString();
}

ProxyMode proxyMode()
{
    return enum_cast<ProxyMode>(bSettings->value(ProxyModePath, NoProxy), allProxyModes(), NoProxy);
}

QString proxyLogin()
{
    return bSettings->value(ProxyLoginPath).toString();
}

QString proxyPassword()
{
    return bSettings->value(ProxyPasswordPath).toString();
}

quint16 proxyPort()
{
    return bSettings->value(ProxyPortPath).toUInt();
}

void setProxyHost(const QString &host)
{
    bSettings->setValue(ProxyHostPath, host);
}

void setProxyLogin(const QString &login)
{
    bSettings->setValue(ProxyLoginPath, login);
}

void setProxyMode(ProxyMode m)
{
    bSettings->setValue(ProxyModePath, (int) m);
}

void setProxyPassword(const QString &pwd)
{
    bSettings->setValue(ProxyPasswordPath, pwd);
}

void setProxyPort(quint16 p)
{
    bSettings->setValue(ProxyPortPath, p);
}

}

/*============================================================================
================================ Texsample ===================================
============================================================================*/

namespace Texsample
{

static const QString RootPath = "TeXSample";
static const QString CachingEnabledPath = RootPath + "/caching_enabled";
static const QString ConnectOnStartupPath = RootPath + "/connect_on_startup";
static const QString HostPath = RootPath + "/host";
static const QString HostHistoryPath = RootPath + "/host_history";
static const QString LoginPath = RootPath + "/login";
static const QString PasswordPath = RootPath + "/password";
static const QString PasswordWidgetStatePath = RootPath + "/password_widget_state";

BPassword mpassword;

bool cachingEnabled()
{
    return bSettings->value(CachingEnabledPath, true).toBool();
}

bool connectOnStartup()
{
    return bSettings->value(ConnectOnStartupPath, true).toBool();
}

bool hasTexsample()
{
    return bSettings->contains(ConnectOnStartupPath);
}

QString host(bool resolveSpecialName)
{
    QString h = bSettings->value(HostPath, UsueTexsampleServerHost).toString();
    typedef QMap<QString, QString> StringMap;
    init_once(StringMap, map, StringMap()) {
        map.insert(UsueTexsampleServerHost, "texsample-server.no-ip.org");
    }
    return (resolveSpecialName && map.contains(h)) ? map.value(h) : h;
}

QStringList hostHistory()
{
    return bSettings->value(HostHistoryPath).toStringList();
}

void loadPassword()
{
    mpassword = bSettings->value(PasswordPath).value<BPassword>();
}

QString login()
{
    return bSettings->value(LoginPath).toString();
}

BPassword password()
{
    return mpassword;
}

QByteArray passwordWidgetState()
{
    return bSettings->value(PasswordWidgetStatePath).toByteArray();
}

void savePassword()
{
    bSettings->setValue(PasswordPath, mpassword.toEncrypted());
}

void setCachingEnabled(bool enabled)
{
    bSettings->setValue(CachingEnabledPath, enabled);
}

void setConnectOnStartup(bool enabled)
{
    bSettings->setValue(ConnectOnStartupPath, enabled);
}

void setHost(const QString &host)
{
    bSettings->setValue(HostPath, host);
}

void setHostHistory(const QStringList &history)
{
    bSettings->setValue(HostHistoryPath, history);
}

void setLogin(const QString &login)
{
    bSettings->setValue(LoginPath, login);
}

void setPassword(const BPassword &pwd)
{
    mpassword = pwd;
}

void setPasswordWidgetState(const QByteArray &state)
{
    bSettings->setValue(PasswordWidgetStatePath, state);
    if (!BPasswordWidget::savePassword(state))
        bSettings->remove(PasswordPath);
}

}

/*============================================================================
================================ TexsampleCore ===============================
============================================================================*/

namespace TexsampleCore
{

static const QString RootPath = "TexsampleCore";
static const QString AccountManagementDialogGeometryPath = RootPath + "/account_management_dialog_geometry";
static const QString GroupManagementDialogGeometryPath = RootPath + "/group_management_dialog_geometry";
static const QString InviteManagementDialogGeometryPath = RootPath + "/invite_management_dialog_geometry";
static const QString LabInfoDialogGeometryPath = RootPath + "/lab_info_dialog_geometry";
static const QString SendLabDialogGeometryPath = RootPath + "/send_lab_dialog_geometry";
static const QString SendLabWidgetStatePath = RootPath + "/send_lab_widget_state";
static const QString UserInfoDialogGeometryPath = RootPath + "/user_info_dialog_geometry";
static const QString UserManagementDialogGeometryPath = RootPath + "/user_management_dialog_geometry";

QByteArray accountManagementDialogGeometry()
{
    return bSettings->value(AccountManagementDialogGeometryPath).toByteArray();
}

QByteArray groupManagementDialogGeometry()
{
    return bSettings->value(GroupManagementDialogGeometryPath).toByteArray();
}

QByteArray inviteManagementDialogGeometry()
{
    return bSettings->value(InviteManagementDialogGeometryPath).toByteArray();
}

QByteArray labInfoDialogGeometry()
{
    return bSettings->value(LabInfoDialogGeometryPath).toByteArray();
}

QByteArray sendLabDialogGeometry()
{
    return bSettings->value(SendLabDialogGeometryPath).toByteArray();
}

QByteArray sendLabWidgetState()
{
    return bSettings->value(SendLabWidgetStatePath).toByteArray();
}

QByteArray userInfoDialogGeometry()
{
    return bSettings->value(LabInfoDialogGeometryPath).toByteArray();
}

QByteArray userManagementDialogGeometry()
{
    return bSettings->value(UserManagementDialogGeometryPath).toByteArray();
}

void setAccountManagementDialogGeometry(const QByteArray &geometry)
{
    bSettings->setValue(AccountManagementDialogGeometryPath, geometry);
}

void setGroupManagementDialogGeometry(const QByteArray &geometry)
{
    bSettings->setValue(GroupManagementDialogGeometryPath, geometry);
}

void setInviteManagementDialogGeometry(const QByteArray &geometry)
{
    bSettings->setValue(InviteManagementDialogGeometryPath, geometry);
}

void setLabInfoDialogGeometry(const QByteArray &geometry)
{
    bSettings->setValue(LabInfoDialogGeometryPath, geometry);
}

void setSendLabDialogGeometry(const QByteArray &geometry)
{
    bSettings->setValue(SendLabDialogGeometryPath, geometry);
}

void setSendLabWidgetState(const QByteArray &state)
{
    bSettings->setValue(SendLabWidgetStatePath, state);
}

void setUserInfoDialogGeometry(const QByteArray &geometry)
{
    bSettings->setValue(UserInfoDialogGeometryPath, geometry);
}

void setUserManagementDialogGeometry(const QByteArray &geometry)
{
    bSettings->setValue(UserManagementDialogGeometryPath, geometry);
}

}

/*============================================================================
================================ TexsampleWidget =============================
============================================================================*/

namespace TexsampleWidget
{

static const QString RootPath = "TexsampleWidget";
static const QString LabTableHeaderStatePath = RootPath + "/lab_table_header_state";

void setLabTableHeaderState(const QByteArray &state)
{
    bSettings->setValue(LabTableHeaderStatePath, state);
}

QByteArray labTableHeaderState()
{
    return bSettings->value(LabTableHeaderStatePath).toByteArray();
}

}

}
