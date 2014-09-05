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

#ifndef SETTINGS_H
#define SETTINGS_H

class BPassword;

class QByteArray;
class QString;
class QStringList;

#include <QList>

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

bool checkForNewVersionOnStartup();
bool multipleWindowsEnabled();
void setCheckForNewVersionOnStartup(bool b);
void setMultipleWindowsEnabled(bool enabled);

}

/*============================================================================
================================ MainWindow ==================================
============================================================================*/

namespace MainWindow
{

void setWindowGeometry(const QByteArray &geometry);
void setWindowState(const QByteArray &state);
QByteArray windowGeometry();
QByteArray windowState();

}

/*============================================================================
================================ Network =====================================
============================================================================*/

namespace Network
{

enum ProxyMode
{
    NoProxy = 0,
    SystemProxy,
    UserProxy
};

QList<ProxyMode> allProxyModes();
QString proxyHost();
ProxyMode proxyMode();
QString proxyLogin();
QString proxyPassword();
quint16 proxyPort();
void setProxyHost(const QString &host);
void setProxyLogin(const QString &login);
void setProxyMode(ProxyMode m);
void setProxyPassword(const QString &pwd);
void setProxyPort(quint16 p);

}

/*============================================================================
================================ Texsample ===================================
============================================================================*/

namespace Texsample
{

const QString UsueTexsampleServerHost = "USUE TeXSample Server";

bool cachingEnabled();
bool connectOnStartup();
bool hasTexsample();
QString host(bool resolveSpecialName = false);
QStringList hostHistory();
void loadPassword();
QString login();
BPassword password();
QByteArray passwordWidgetState();
void savePassword();
void setCachingEnabled(bool enabled);
void setConnectOnStartup(bool enabled);
void setHost(const QString &host);
void setHostHistory(const QStringList &history);
void setLogin(const QString &login);
void setPassword(const BPassword &pwd);
void setPasswordWidgetState(const QByteArray &state);

}

/*============================================================================
================================ TexsampleCore ===============================
============================================================================*/

namespace TexsampleCore
{

QByteArray accountManagementDialogGeometry();
QByteArray groupManagementDialogGeometry();
QByteArray inviteManagementDialogGeometry();
QByteArray labInfoDialogGeometry();
QByteArray sendLabDialogGeometry();
QByteArray sendLabWidgetState();
QByteArray userInfoDialogGeometry();
QByteArray userManagementDialogGeometry();
void setAccountManagementDialogGeometry(const QByteArray &geometry);
void setGroupManagementDialogGeometry(const QByteArray &geometry);
void setInviteManagementDialogGeometry(const QByteArray &geometry);
void setLabInfoDialogGeometry(const QByteArray &geometry);
void setSendLabDialogGeometry(const QByteArray &geometry);
void setSendLabWidgetState(const QByteArray &state);
void setUserInfoDialogGeometry(const QByteArray &geometry);
void setUserManagementDialogGeometry(const QByteArray &geometry);

}

/*============================================================================
================================ TexsampleWidget =============================
============================================================================*/

namespace TexsampleWidget
{

void setLabTableHeaderState(const QByteArray &state);
QByteArray labTableHeaderState();

}

}

#endif // SETTINGS_H
