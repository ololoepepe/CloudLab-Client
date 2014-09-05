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

class TexsampleWidget;

class QAction;
class QCloseEvent;
class QMenu;
class QString;

#include <QMainWindow>

/*============================================================================
================================ MainWindow ==================================
============================================================================*/

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    TexsampleWidget *mwgt;
    //
    QMenu *mmnuFile;
    QAction *mactQuit;
    QMenu *mmnuEdit;
    QMenu *mmnuAutotext;
    QMenu *mmnuClab;
    QMenu *mmnuHelp;
public:
    explicit MainWindow();
    ~MainWindow();
public slots:
    void showStatusBarMessage(const QString &message);
protected:
    void closeEvent(QCloseEvent *e);
private:
    void initCentralWidget();
    void initMenus();
private slots:
    void restoreStateWorkaround();
    void retranslateUi();
private:
    Q_DISABLE_COPY(MainWindow)
};

#endif // MAINWINDOW_H
