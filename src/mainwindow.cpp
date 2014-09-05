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

class QWidget;

#include "mainwindow.h"

#include "application.h"
#include "settings.h"
#include "texsample/texsamplecore.h"
#include "texsample/texsamplewidget.h"

#include <BGuiTools>

#include <QAction>
#include <QByteArray>
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QIcon>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QRect>
#include <QStatusBar>
#include <QString>
#include <QTimer>
#include <QToolBar>

/*============================================================================
================================ MainWindow ==================================
============================================================================*/

/*============================== Public constructors =======================*/

MainWindow::MainWindow() :
    QMainWindow(0)
{
    setAcceptDrops(true);
    setDockOptions(dockOptions() | QMainWindow::ForceTabbedDocks);
    setGeometry(200, 200, 400, 600); //The default
    restoreGeometry(Settings::MainWindow::windowGeometry());
    //
    initCentralWidget();
    initMenus();
    retranslateUi();
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
    //NOTE: Qt bug. Window state not restored without some delay (500 ms should be enough)
    QTimer::singleShot(500, this, SLOT(restoreStateWorkaround()));
}

MainWindow::~MainWindow()
{
    //
}

/*============================== Public slots ==============================*/

void MainWindow::showStatusBarMessage(const QString &message)
{
    statusBar()->showMessage(message);
}

/*============================== Purotected methods ========================*/

void MainWindow::closeEvent(QCloseEvent *e)
{
    Settings::MainWindow::setWindowGeometry(saveGeometry());
    Settings::MainWindow::setWindowState(saveState());
    return QMainWindow::closeEvent(e);
}

/*============================== Private methods ===========================*/

void MainWindow::initCentralWidget()
{
    mwgt = new TexsampleWidget(this);
    setCentralWidget(mwgt);
}

void MainWindow::initMenus()
{
    //File
    mmnuFile = menuBar()->addMenu("");
    mactQuit = mmnuFile->addAction("");
    mactQuit->setMenuRole(QAction::QuitRole);
    mactQuit->setIcon(Application::icon("exit"));
    mactQuit->setShortcut(QKeySequence("Ctrl+Q"));
    connect(mactQuit, SIGNAL(triggered()), this, SLOT(close()));
    //Edit
    mmnuEdit = menuBar()->addMenu("");
    QAction *act = BGuiTools::createStandardAction(BGuiTools::SettingsAction);
    act->setShortcut(QKeySequence("Ctrl+P"));
    mmnuEdit->addAction(act);
    //Clab
    mmnuClab = menuBar()->addMenu("");
    mmnuClab->addActions(mwgt->toolBarActions());
    //Help
    mmnuHelp = menuBar()->addMenu("");
    mmnuHelp->addAction(BGuiTools::createStandardAction(BGuiTools::HomepageAction));
    mmnuHelp->addSeparator();
    act = BGuiTools::createStandardAction(BGuiTools::HelpContentsAction);
    act->setShortcut(QKeySequence("F1"));
    mmnuHelp->addAction(act);
    mmnuHelp->addAction(BGuiTools::createStandardAction(BGuiTools::WhatsThisAction));
    mmnuHelp->addSeparator();
    mmnuHelp->addAction(BGuiTools::createStandardAction(BGuiTools::AboutAction));
}

/*============================== Private slots =============================*/

void MainWindow::restoreStateWorkaround()
{
    restoreState(Settings::MainWindow::windowState());
}

void MainWindow::retranslateUi()
{
    setWindowTitle("CloudLab Client");
    //menus
    mmnuFile->setTitle(tr("File", "mnu title"));
    mactQuit->setText(tr("Quit", "act text"));
    mmnuEdit->setTitle(tr("Edit", "mnu title"));
    mmnuClab->setTitle(tr("CloudLab", "mnuTitle"));
    mmnuHelp->setTitle(tr("Help", "mnuTitle"));
}
