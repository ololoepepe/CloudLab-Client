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

#ifndef APPLICATION_H
#define APPLICATION_H

class MainWindow;
class TexsampleCore;

class BAbstractSettingsTab;

class QWidget;

#include <TApplication>

#include <QList>
#include <QMap>
#include <QObject>
#include <QStringList>

#if defined(bApp)
#   undef bApp
#endif
#define bApp (static_cast<Application *>(BApplication::instance()))

/*============================================================================
================================ Application =================================
============================================================================*/

class Application : public TApplication
{
    Q_OBJECT
public:
    enum SettingsType
    {
        TexsampleSettings
    };
private:
    QMap<QObject *, MainWindow *> mmainWindows;
    TexsampleCore *mtexsampleCore;
public:
    explicit Application(int &argc, char **argv, const QString &applicationName, const QString &organizationName);
    ~Application();
public:
    static void resetProxy();
public:
    bool mergeWindows();
    MainWindow *mostSuitableWindow() const;
    bool showSettings(SettingsType type, QWidget *parent = 0);
    TexsampleCore *texsampleCore() const;
public slots:
    void messageReceived(const QStringList &args);
    void showStatusBarMessage(const QString &message);
protected:
    QList<BAbstractSettingsTab *> createSettingsTabs() const;
private:
    static bool testAppInit();
private:
    void addMainWindow(const QStringList &fileNames = QStringList());
    void compatibility();
    void createInitialWindow();
private slots:
    void mainWindowDestroyed(QObject *obj);
private:
    Q_DISABLE_COPY(Application)
};

#endif // APPLICATION_H
