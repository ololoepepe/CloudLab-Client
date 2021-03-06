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

#ifndef TEXSAMPLEWIDGET_H
#define TEXSAMPLEWIDGET_H

class ConnectionAction;
class LabProxyModel;
class MainWindow;

class QAction;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QModelIndex;
class QPoint;
class QString;
class QTableView;
class QToolBar;

#include <TNetworkClient>

#include <QList>
#include <QWidget>

/*============================================================================
================================ TexsampleWidget =============================
============================================================================*/

class TexsampleWidget : public QWidget
{
    Q_OBJECT
private:
    MainWindow *const Window;
private:
    LabProxyModel *mproxyModel;
    quint64 mlastId;
    //
    QToolBar *mtbarIndicator;
    //
    QToolBar *mtbar;
      ConnectionAction *mactConnection;
        QAction *mactConnect;
        QAction *mactDisconnect;
      QAction *mactUpdate;
      QAction *mactSend;
      QAction *mactTools;
        QAction *mactRegister;
        QAction *mactConfirmRegistration;
        QAction *mactConfirmEmailChange;
        QAction *mactRecover;
        QAction *mactEditAccount;
        QAction *mactSettings;
        QAction *mactAdministration;
          QAction *mactUserManagement;
          QAction *mactGroupManagement;
          QAction *mactInviteManagement;
    QGroupBox *mgboxSelect;
      QLabel *mlblSearch;
      QLineEdit *mledtSearch;
    QTableView *mtblvw;
public:
    explicit TexsampleWidget(MainWindow *window, QWidget *parent = 0);
    ~TexsampleWidget();
public:
    QWidget *indicator() const;
    QList<QAction *> toolBarActions() const;
private slots:
    void clientStateChanged(TNetworkClient::State state);
    void deleteLab();
    void editLab();
    void getLab();
    void retranslateUi();
    void sendLab();
    void showLabInfo();
    void tblvwCustomContextMenuRequested(const QPoint &pos);
    void tblvwDoubleClicked(const QModelIndex &index);
private:
    Q_DISABLE_COPY(TexsampleWidget)
};

#endif // TEXSAMPLEWIDGET_H
