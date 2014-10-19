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

#include "application.h"

#include "mainwindow.h"
#include "settings.h"
#include "settingstab/generalsettingstab.h"
#include "settingstab/networksettingstab.h"
#include "settingstab/texsamplesettingstab.h"
#include "texsample/texsamplecore.h"

#include <TApplication>

#include <BAboutDialog>
#include <BAbstractSettingsTab>
#include <BDirTools>
#include <BGuiTools>
#include <BPasswordWidget>
#include <BPluginsSettingsTab>
#include <BSettingsDialog>
#include <BVersion>

#include <QByteArray>
#include <QDebug>
#include <QList>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QNetworkProxyQuery>
#include <QObject>
#include <QSettings>
#include <QSize>
#include <QUrl>
#include <QVariant>

/*============================================================================
================================ Application =================================
============================================================================*/

/*============================== Public constructors =======================*/

Application::Application(int &argc, char **argv, const QString &applicationName, const QString &organizationName) :
    TApplication(argc, argv, applicationName, organizationName)
{
#if defined(BUILTIN_RESOURCES)
    Q_INIT_RESOURCE(cloudlab_client);
    Q_INIT_RESOURCE(cloudlab_client_doc);
    Q_INIT_RESOURCE(cloudlab_client_translations);
#endif
    setApplicationVersion("1.0.0");
    setOrganizationDomain("http://sourceforge.net/projects/cloudlab-client");
    setApplicationCopyrightPeriod("2013-2014");
    compatibility();
    resetProxy();
    QFont fnt = font();
    fnt.setPointSize(10);
    setFont(fnt);
    setThemedIconsEnabled(false);
    setPreferredIconFormats(QStringList() << "png");
    setWindowIcon(icon("cloudlab-client"));
    installBeqtTranslator("qt");
    installBeqtTranslator("beqt");
    installBeqtTranslator("texsample");
    installBeqtTranslator("cloudlab-client");
    BAboutDialog::setDefaultMinimumSize(800, 400);
    setApplicationDescriptionFile(findResource("description", BDirTools::GlobalOnly) + "/DESCRIPTION.txt");
    setApplicationChangeLogFile(findResource("changelog", BDirTools::GlobalOnly) + "/ChangeLog.txt");
    setApplicationLicenseFile(findResource("copying", BDirTools::GlobalOnly) + "/COPYING.txt");
    setApplicationAuthorsFile(findResource("infos/authors.beqt-info", BDirTools::GlobalOnly));
    setApplicationTranslationsFile(findResource("infos/translators.beqt-info", BDirTools::GlobalOnly));
    setApplicationThanksToFile(findResource("infos/thanks-to.beqt-info", BDirTools::GlobalOnly));
    aboutDialogInstance()->setupWithApplicationData();
    mtexsampleCore = new TexsampleCore;
    createInitialWindow();
    setHelpBrowserDefaultGeometry(BGuiTools::centerOnScreenGeometry(1000, 800, 100, 50));
}

Application::~Application()
{
    delete mtexsampleCore;
    if (BPasswordWidget::savePassword(Settings::Texsample::passwordWidgetState()))
        Settings::Texsample::savePassword();
#if defined(BUILTIN_RESOURCES)
    Q_CLEANUP_RESOURCE(cloudlab_client);
    Q_CLEANUP_RESOURCE(cloudlab_client_doc);
    Q_CLEANUP_RESOURCE(cloudlab_client_translations);
#endif
}

/*============================== Static public methods =====================*/

void Application::resetProxy()
{
    switch (Settings::Network::proxyMode()) {
    case Settings::Network::NoProxy:
        QNetworkProxy::setApplicationProxy(QNetworkProxy());
        break;
    case Settings::Network::SystemProxy: {
        QNetworkProxyQuery query = QNetworkProxyQuery(QUrl("http://www.google.com"));
        QList<QNetworkProxy> list = QNetworkProxyFactory::systemProxyForQuery(query);
        if (!list.isEmpty())
            QNetworkProxy::setApplicationProxy(list.first());
        else
            QNetworkProxy::setApplicationProxy(QNetworkProxy());
        break;
    }
    case Settings::Network::UserProxy: {
        QString host = Settings::Network::proxyHost();
        quint16 port = Settings::Network::proxyPort();
        QString login = Settings::Network::proxyLogin();
        QString password = Settings::Network::proxyPassword();
        QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy, host, port, login, password));
        break;
    }
    default:
        break;
    }
}

/*============================== Public methods ============================*/

bool Application::mergeWindows()
{
    if (mmainWindows.size() < 2)
        return true;
    QList<MainWindow *> list = mmainWindows.values();
    MainWindow *first = list.takeFirst();
    foreach (MainWindow *mw, list)
        mw->close();
    first->activateWindow();
    return true;
}

MainWindow *Application::mostSuitableWindow() const
{
    QWidget *wgt = activeWindow();
    QList<MainWindow *> list = mmainWindows.values();
    foreach (MainWindow *mw, list) {
        if (mw == wgt)
            return mw;
    }
    return !list.isEmpty() ? list.first() : 0;
}

bool Application::showSettings(SettingsType type, QWidget *parent)
{
    BAbstractSettingsTab *tab = 0;
    switch (type) {
    case TexsampleSettings:
        tab = new TexsampleSettingsTab;
        break;
    default:
        break;
    }
    if (!tab)
        return false;
    return (BSettingsDialog(tab, parent ? parent : mostSuitableWindow()).exec() == BSettingsDialog::Accepted);
}

TexsampleCore *Application::texsampleCore() const
{
    return mtexsampleCore;
}

/*============================== Public slots ==============================*/

void Application::messageReceived(const QStringList &args)
{
    if (Settings::General::multipleWindowsEnabled())
        addMainWindow(args);
}

void Application::showStatusBarMessage(const QString &message)
{
    MainWindow *mw = mostSuitableWindow();
    if (!mw)
        return;
    mw->showStatusBarMessage(message);
}

/*============================== Protected methods =========================*/

QList<BAbstractSettingsTab *> Application::createSettingsTabs() const
{
    QList<BAbstractSettingsTab *> list;
    list << new GeneralSettingsTab;
    list << new NetworkSettingsTab;
    list << new TexsampleSettingsTab;
    list << new BPluginsSettingsTab;
    return list;
}

/*============================== Static private methods ====================*/

bool Application::testAppInit()
{
    return bTest(bApp, "Application", "There must be an Application instance");
}

/*============================== Private methods ===========================*/

void Application::addMainWindow(const QStringList &)
{
    MainWindow *mw = new MainWindow;
    mw->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(mw, SIGNAL(destroyed(QObject *)), this, SLOT(mainWindowDestroyed(QObject *)));
    mmainWindows.insert(mw, mw);
    mw->show();
}

void Application::compatibility()
{
    bSettings->setValue("Global/version", BVersion(applicationVersion()));
}

void Application::createInitialWindow()
{
    if (!testAppInit())
        return;
    addMainWindow(arguments().mid(1));
}

/*============================== Private slots =============================*/

void Application::mainWindowDestroyed(QObject *obj)
{
    mmainWindows.remove(obj);
}
