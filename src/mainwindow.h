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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class ClabWidget;

class QString;
class QAction;
class QMenu;
class QSignalMapper;
class QCloseEvent;
class QToolBar;
class QByteArray;
class QLabel;

#include <BApplication>

#include <QMainWindow>
#include <QTextCodec>

/*============================================================================
================================ MainWindow ==================================
============================================================================*/

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow();
    ~MainWindow();
public:
    static QByteArray getWindowGeometry();
    static QByteArray getWindowState();
    static void setWindowGeometry(const QByteArray &geometry);
    static void setWindowState(const QByteArray &state);
protected:
    void closeEvent(QCloseEvent *e);
private:
    void initCentralWidget();
    void initMenus();
private slots:
    void retranslateUi();
private:
    QSignalMapper *mmprAutotext;
    QSignalMapper *mmprOpenFile;
    //
    ClabWidget *mwgt;
    //
    QMenu *mmnuFile;
      QAction *mactQuit;
    QMenu *mmnuEdit;
      QMenu *mmnuAutotext;
    QMenu *mmnuClab;
    QMenu *mmnuHelp;
private:
    Q_DISABLE_COPY(MainWindow)
};

#endif // MAINWINDOW_H
