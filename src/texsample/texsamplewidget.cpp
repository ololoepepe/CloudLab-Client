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

#include "texsamplewidget.h"

#include "application.h"
#include "connectionaction.h"
#include "labmodel.h"
#include "labproxymodel.h"
#include "mainwindow.h"
#include "settings.h"
#include "texsamplecore.h"

#include <TAccessLevel>
#include <TLabDataInfo>
#include <TLabDataInfoList>
#include <TLabInfo>
#include <TLabType>
#include <TNetworkClient>
#include <TUserInfo>

#include <BSignalDelayProxy>

#include <QAction>
#include <QByteArray>
#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMenu>
#include <QModelIndex>
#include <QString>
#include <QTableView>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

/*============================================================================
================================ TexsampleWidget =============================
============================================================================*/

/*============================== Public constructors =======================*/

TexsampleWidget::TexsampleWidget(MainWindow *window, QWidget *parent) :
    QWidget(parent), Window(window)
{
    mlastId = 0;
    mproxyModel = new LabProxyModel(tSmp->labModel(), this);
    TNetworkClient *client = tSmp->client();
    connect(client, SIGNAL(stateChanged(TNetworkClient::State)),
            this, SLOT(clientStateChanged(TNetworkClient::State)));
    //
    mtbarIndicator = new QToolBar;
    //
    QVBoxLayout *vlt = new QVBoxLayout(this);
      mtbar = new QToolBar(this);
        mactConnection = new ConnectionAction(this);
          QMenu *mnu = new QMenu;
            mactConnect = new QAction(this);
              mactConnect->setIcon(Application::icon("connect_established"));
              connect(mactConnect, SIGNAL(triggered()), tSmp, SLOT(connectToServer()));
            mnu->addAction(mactConnect);
            mactDisconnect = new QAction(this);
              mactDisconnect->setIcon(Application::icon("connect_no"));
              connect(mactDisconnect, SIGNAL(triggered()), tSmp, SLOT(disconnectFromServer()));
            mnu->addAction(mactDisconnect);
          mactConnection->setMenu(mnu);
        mtbar->addAction(mactConnection);
        mtbarIndicator->addAction(mactConnection);
        mactUpdate = new QAction(this);
          mactUpdate->setIcon(Application::icon("reload"));
          connect(mactUpdate, SIGNAL(triggered()), tSmp, SLOT(updateLabList()));
        mtbar->addAction(mactUpdate);
        mactSend = new QAction(this);
          mactSend->setIcon(Application::icon("mail_send"));
          connect(mactSend, SIGNAL(triggered()), this, SLOT(sendLab()));
        mtbar->addAction(mactSend);
        mactAdministration = new QAction(this);
          mactAdministration->setIcon(Application::icon("gear"));
          mnu = new QMenu;
            mactUserManagement = mnu->addAction(Application::icon("users"), "", tSmp,
                                                SLOT(showUserManagementWidget()));
            mactGroupManagement = mnu->addAction(Application::icon("group"), "", tSmp,
                                                 SLOT(showGroupManagementWidget()));
            mactInviteManagement = mnu->addAction(Application::icon("mail_send"), "", tSmp,
                                                  SLOT(showInviteManagementWidget()));
        mactAdministration->setMenu(mnu);
        mtbar->addAction(mactAdministration);
        static_cast<QToolButton *>(
                    mtbar->widgetForAction(mactAdministration))->setPopupMode(QToolButton::InstantPopup);
        mactTools = new QAction(this);
          mactTools->setIcon(Application::icon("configure"));
          mnu = new QMenu;
            mactRegister = new QAction(this);
              mactRegister->setIcon(Application::icon("add_user"));
              connect(mactRegister, SIGNAL(triggered()), tSmp, SLOT(showRegisterDialog()));
            mnu->addAction(mactRegister);
            mnu->addSeparator();
            mactConfirmRegistration = new QAction(this);
              mactConfirmRegistration->setIcon(Application::icon("checkmark"));
              connect(mactConfirmRegistration, SIGNAL(triggered()), tSmp, SLOT(showConfirmRegistrationDialog()));
            mnu->addAction(mactConfirmRegistration);
            mactConfirmEmailChange = new QAction(this);
              mactConfirmEmailChange->setIcon(Application::icon("mail_confirm"));
              connect(mactConfirmEmailChange, SIGNAL(triggered()), tSmp, SLOT(showConfirmEmailChangeDialog()));
            mnu->addAction(mactConfirmEmailChange);
            mnu->addSeparator();
            mactRecover = new QAction(this);
              mactRecover->setIcon(Application::icon("account_recover"));
              connect(mactRecover, SIGNAL(triggered()), tSmp, SLOT(showRecoverDialog()));
            mnu->addAction(mactRecover);
            mnu->addSeparator();
            mactEditAccount = new QAction(this);
              mactEditAccount->setIcon(Application::icon("user"));
              connect( mactEditAccount, SIGNAL(triggered()), tSmp, SLOT(showAccountManagementDialog()));
            mnu->addAction(mactEditAccount);
            mnu->addSeparator();
            mactSettings = new QAction(this);
              mactSettings->setIcon(Application::icon("configure"));
              connect(mactSettings, SIGNAL(triggered()), tSmp, SLOT(showTexsampleSettings()));
            mnu->addAction(mactSettings);
          mactTools->setMenu(mnu);
        mtbar->addAction(mactTools);
        static_cast<QToolButton *>(mtbar->widgetForAction(mactTools))->setPopupMode(QToolButton::InstantPopup);
      vlt->addWidget(mtbar);
      mgboxSelect = new QGroupBox(this);
        QFormLayout *flt = new QFormLayout;
          mlblSearch = new QLabel;
          QHBoxLayout *hlt = new QHBoxLayout;
            mledtSearch = new QLineEdit;
              BSignalDelayProxy *sdp = new BSignalDelayProxy(this);
              sdp->setStringConnection(mledtSearch, SIGNAL(textChanged(QString)),
                                       mproxyModel, SLOT(setSearchKeywordsString(QString)));
            hlt->addWidget(mledtSearch);
          flt->addRow(mlblSearch, hlt);
        mgboxSelect->setLayout(flt);
      vlt->addWidget(mgboxSelect);
      mtblvw = new QTableView(this);
        mtblvw->setAlternatingRowColors(true);
        mtblvw->setEditTriggers(QTableView::NoEditTriggers);
        mtblvw->setSelectionBehavior(QTableView::SelectRows);
        mtblvw->setSelectionMode(QTableView::SingleSelection);
        mtblvw->horizontalHeader()->setStretchLastSection(true);
        mtblvw->verticalHeader()->setVisible(false);
        mtblvw->setContextMenuPolicy(Qt::CustomContextMenu);
        mtblvw->setModel(mproxyModel);
        connect(mtblvw, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tblvwDoubleClicked(QModelIndex)));
        connect(mtblvw, SIGNAL(customContextMenuRequested(QPoint)),
                this, SLOT(tblvwCustomContextMenuRequested(QPoint)));
      vlt->addWidget(mtblvw);
    //
    retranslateUi();
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
    mtblvw->horizontalHeader()->restoreState(Settings::TexsampleWidget::labTableHeaderState());
    clientStateChanged(client->state());
}

TexsampleWidget::~TexsampleWidget()
{
    Settings::TexsampleWidget::setLabTableHeaderState(mtblvw->horizontalHeader()->saveState());
}

/*============================== Public methods ============================*/

QWidget *TexsampleWidget::indicator() const
{
    return mtbarIndicator;
}

QList<QAction *> TexsampleWidget::toolBarActions() const
{
    QList<QAction *> list;
    list << mactConnection;
    list << mactUpdate;
    list << mactSend;
    list << mactTools;
    return list;
}

/*============================== Private slots =============================*/

void TexsampleWidget::clientStateChanged(TNetworkClient::State state)
{
    QString s = tr("TeXSample state:", "act toolTip") + " ";
    switch (state)
    {
    case TNetworkClient::DisconnectedState:
        mactConnection->resetIcon(s + tr("disconnected", "act toolTip"), "connect_no");
        break;
    case TNetworkClient::ConnectingState:
        mactConnection->resetIcon(s + tr("connecting", "act toolTip"), "process", true);
        break;
    case TNetworkClient::ConnectedState:
        mactConnection->resetIcon(s + tr("connected", "act toolTip"), "process", true);
        break;
    case TNetworkClient::AuthorizedState:
        mactConnection->resetIcon(s + tr("authorized", "act toolTip"), "connect_established");
        break;
    case TNetworkClient::DisconnectingState:
        mactConnection->resetIcon(s + tr("disconnecting", "act toolTip"), "process", true);
        break;
    default:
        break;
    }
    int lvl = tSmp->client()->userInfo().accessLevel();
    bool authorized = tSmp->client()->isAuthorized();
    mactConnect->setEnabled(TNetworkClient::DisconnectedState == state);
    mactDisconnect->setEnabled(TNetworkClient::DisconnectedState != state
            && TNetworkClient::DisconnectingState != state);
    mactUpdate->setEnabled(authorized);
    mactSend->setEnabled(authorized);
    mactConfirmEmailChange->setEnabled(authorized);
    mactEditAccount->setEnabled(authorized);
    mactAdministration->setEnabled(lvl >= TAccessLevel::ModeratorLevel);
    mactUserManagement->setEnabled(lvl >= TAccessLevel::AdminLevel);
    mactGroupManagement->setEnabled(lvl >= TAccessLevel::ModeratorLevel);
    mactInviteManagement->setEnabled(lvl >= TAccessLevel::AdminLevel);
}

void TexsampleWidget::deleteLab()
{
    if (!mlastId)
        return;
    tSmp->deleteLab(mlastId);
}

void TexsampleWidget::editLab()
{
    if (!mlastId)
        return;
    tSmp->editLab(mlastId);
}

void TexsampleWidget::getLab()
{
    if (!mlastId)
        return;
    tSmp->getLab(mlastId);
}

void TexsampleWidget::retranslateUi()
{
    mactConnection->setText(tr("Connection", "act text"));
    mactConnection->setWhatsThis(tr("This action shows current connection state. "
                                    "Use it to connect or disconnect from the server", "act whatsThis"));
    clientStateChanged(tSmp->client()->state());
    mactConnect->setText(tr("Connect", "act text"));
    mactDisconnect->setText(tr("Disconnect", "act text"));
    mactUpdate->setText(tr("Update", "act text"));
    mactUpdate->setToolTip(tr("Update labs list", "act toolTip"));
    mactSend->setText(tr("Send lab...", "act text"));
    mactSend->setToolTip(tr("Send lab...", "act toolTip"));
    mactTools->setText(tr("Tools", "act text"));
    mactTools->setToolTip(tr("Tools", "act toolTip"));
    mactRegister->setText(tr("Register...", "act tooTip"));
    mactConfirmRegistration->setText(tr("Confirm registration...", "act text"));
    mactConfirmEmailChange->setText(tr("Confirm e-mail change...", "act text"));
    mactRecover->setText(tr("Recover account...", "act text"));
    mactSettings->setText(tr("TeXSample settings...", "act text"));
    mactEditAccount->setText(tr("Account management...", "act text"));
    mactAdministration->setText(tr("Administration", "act text"));
    mactUserManagement->setText(tr("User management...", "act text"));
    mactGroupManagement->setText(tr("Group management...", "act text"));
    mactInviteManagement->setText(tr("Invite management...", "act text"));
    //
    mgboxSelect->setTitle(tr("Selection", "gbox title"));
    //
    mlblSearch->setText(tr("Search:", "lbl text"));
}

void TexsampleWidget::sendLab()
{
    tSmp->sendLab();
}

void TexsampleWidget::showLabInfo()
{
    if (!mlastId)
        return;
    tSmp->showLabInfo(mlastId);
}

void TexsampleWidget::tblvwCustomContextMenuRequested(const QPoint &pos)
{
    mlastId = tSmp->labModel()->labIdAt(mproxyModel->mapToSource(mtblvw->indexAt(pos)).row());
    if (!mlastId)
        return;
    TNetworkClient *client = tSmp->client();
    QMenu mnu;
    QString s = " (" + BeQt::fileSizeToString(tSmp->labModel()->labInfo(mlastId).size(), BeQt::MegabytesFormat) + ")";
    QAction *act = mnu.addAction(tr("Get...", "act text") + (!s.isEmpty() ? s : QString()), this, SLOT(getLab()));
      act->setEnabled(client->isAuthorized());
      act->setIcon(Application::icon("editpaste"));
    mnu.addSeparator();
    act = mnu.addAction(tr("Information...", "act text"), this, SLOT(showLabInfo()));
      act->setIcon(Application::icon("help_about"));
    mnu.addSeparator();
    act = mnu.addAction(tr("Edit...", "act text"), this, SLOT(editLab()));
      act->setIcon(Application::icon("edit"));
      TLabInfo labInfo = tSmp->labModel()->labInfo(mlastId);
      TUserInfo userInfo = client->userInfo();
      bool own = (labInfo.senderId() == userInfo.id());
      act->setEnabled(client->isAuthorized() && (own || int(userInfo.accessLevel()) >= TAccessLevel::ModeratorLevel));
    act = mnu.addAction(tr("Delete...", "act text"), this, SLOT(deleteLab()));
    act->setEnabled(client->isAuthorized() && (own || int(userInfo.accessLevel()) >= TAccessLevel::AdminLevel));
      act->setIcon(Application::icon("editdelete"));
    mnu.exec(mtblvw->mapToGlobal(pos));
}

void TexsampleWidget::tblvwDoubleClicked(const QModelIndex &index)
{
    if (!tSmp->client()->isAuthorized())
        return;
    QModelIndex ind = mproxyModel->mapToSource(index);
    if (!ind.isValid())
        return;
    mlastId = tSmp->labModel()->labIdAt(ind.row());
    if (!mlastId)
        return;
    getLab();
}
