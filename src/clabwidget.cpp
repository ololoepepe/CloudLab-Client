#include "clabwidget.h"
#include "labsmodel.h"
#include "application.h"
#include "client.h"
#include "texsamplesettingstab.h"
#include "mainwindow.h"
#include "labwidget.h"
#include "global.h"
#include "labsproxymodel.h"

#include <TOperationResult>
#include <TUserInfo>
#include <TAccessLevel>
#include <TLabProject>
#include <TLabInfo>
#include <TUserWidget>
#include <TRecoveryDialog>
#include <TInvitesDialog>
#include <TListWidget>
#include <TProjectFile>
#include <TProjectFileList>

#include <BApplication>
#include <BSettingsDialog>
#include <BDirTools>
#include <BSignalDelayProxy>
#include <BNetworkOperation>
#include <BAbstractSettingsTab>
#include <BeQtGlobal>
#include <BInputField>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTableView>
#include <QHeaderView>
#include <QFormLayout>
#include <QLabel>
#include <QAction>
#include <QVariant>
#include <QVariantMap>
#include <QByteArray>
#include <QString>
#include <QMovie>
#include <QToolButton>
#include <QRegExp>
#include <QMenu>
#include <QSettings>
#include <QModelIndex>
#include <QMenu>
#include <QAction>
#include <QTextCodec>
#include <QInputDialog>
#include <QMessageBox>
#include <QIcon>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QWidgetAction>
#include <QList>
#include <QMetaObject>
#include <QCloseEvent>
#include <QButtonGroup>
#include <QRadioButton>

#include <QDebug>

/*============================================================================
================================ ConnectionAction ============================
============================================================================*/

class ConnectionAction : public QWidgetAction
{
public:
    explicit ConnectionAction(QObject *parent);
public:
    void resetIcon(const QString &toolTip, const QString &iconName, bool animated = false);
protected:
    QWidget *createWidget(QWidget *parent);
    void deleteWidget(QWidget *widget);
};

/*============================================================================
================================ AddLabDialog ================================
============================================================================*/

/*============================== Public constructors =======================*/

AddLabDialog::AddLabDialog(QWidget *parent) :
    BDialog(parent)
{
    mlabwgt = new LabWidget(LabWidget::AddMode);
    mlabwgt->setCheckSourceValidity(true);
    setWindowTitle(tr("Sending lab..."));
    mlabwgt->setInfo(bSettings->value("AddLabDialog/lab_widget_info").value<TLabInfo>());
    mlabwgt->restoreState(bSettings->value("ClabWidget/lab_widget_state").toByteArray());
    setWidget(mlabwgt);
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    addButton(tr("Clear", "btn text"), QDialogButtonBox::ActionRole, mlabwgt, SLOT(clear()));
    button(QDialogButtonBox::Ok)->setEnabled(mlabwgt->isValid());
    connect(mlabwgt, SIGNAL(validityChanged(bool)), button(QDialogButtonBox::Ok), SLOT(setEnabled(bool)));
    connect(button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(accept()));
    connect(button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
    restoreGeometry(bSettings->value("AddLabDialog/geometry").toByteArray());
}

/*============================== Public methods ============================*/

LabWidget *AddLabDialog::labWidget() const
{
    return mlabwgt;
}

/*============================== Protected methods =========================*/

void AddLabDialog::closeEvent(QCloseEvent *e)
{
    bSettings->setValue("AddLabDialog/lab_widget_info", mlabwgt->info());
    bSettings->setValue("ClabWidget/lab_widget_state", mlabwgt->saveState());
    bSettings->setValue("AddLabDialog/geometry", saveGeometry());
    e->accept();
}

/*============================================================================
================================ EditLabDialog ===============================
============================================================================*/

/*============================== Public constructors =======================*/

EditLabDialog::EditLabDialog(quint64 id, QWidget *parent) :
    BDialog(parent)
{
    if (!id)
        return;
    const TLabInfo *info = sModel->lab(id);
    if (!info)
        return;
    initialFiles = info->extraAttachedFileNames();
    mlabwgt = new LabWidget(LabWidget::EditMode);
    setWindowTitle(tr("Editing lab:") + " " + info->title());
    mlabwgt->restoreState(bSettings->value("ClabWidget/lab_widget_state").toByteArray());
    mlabwgt->setInfo(*info);
    setWidget(mlabwgt);
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    addButton(tr("Clear", "btn text"), QDialogButtonBox::ActionRole, mlabwgt, SLOT(clear()));
    button(QDialogButtonBox::Ok)->setEnabled(mlabwgt->isValid());
    connect(mlabwgt, SIGNAL(validityChanged(bool)), button(QDialogButtonBox::Ok), SLOT(setEnabled(bool)));
    connect(button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(accept()));
    connect(button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
    restoreGeometry(bSettings->value("EditLabDialog/geometry").toByteArray());
}

/*============================== Public methods ============================*/

LabWidget *EditLabDialog::labWidget() const
{
    return mlabwgt;
}

QStringList EditLabDialog::initialExtraFiles() const
{
    return initialFiles;
}

/*============================== Protected methods =========================*/

void EditLabDialog::closeEvent(QCloseEvent *e)
{
    bSettings->setValue("ClabWidget/lab_widget_state", mlabwgt->saveState());
    bSettings->setValue("EditLabDialog/geometry", saveGeometry());
    e->accept();
}

/*============================================================================
================================ SelectUserDialog ============================
============================================================================*/

/*============================== Public constructors =======================*/

SelectUserDialog::SelectUserDialog(QWidget *parent) :
    BDialog(parent)
{
    QButtonGroup *btngr = new QButtonGroup(this);
    connect(btngr, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));
    QWidget *wgt = new QWidget;
      QFormLayout *flt = new QFormLayout(wgt);
        QHBoxLayout *hlt = new QHBoxLayout;
          QRadioButton *rbtn = new QRadioButton(tr("ID", "rbtn text"));
          hlt->addWidget(rbtn);
          btngr->addButton(rbtn, 0);
          rbtn = new QRadioButton(tr("Login", "rbtn text"));
            rbtn->setChecked(true);
          hlt->addWidget(rbtn);
          btngr->addButton(rbtn, 1);
        flt->addRow(tr("Identifier:", "lbl text"), hlt);
        mledt = new QLineEdit;
          connect(mledt, SIGNAL(textChanged(QString)), this, SLOT(checkValidity()));
          mfield = new BInputField;
          mfield->addWidget(mledt);
        flt->addRow(tr("Value:", "lbl text"), mfield);
      wgt->setLayout(flt);
    setWidget(wgt);
    //
    addButton(QDialogButtonBox::Ok, SLOT(accept()));
    addButton(QDialogButtonBox::Cancel, SLOT(reject()));
    buttonClicked(1);
}

/*============================== Public methods ============================*/

quint64 SelectUserDialog::userId() const
{
    return mledt->text().toULongLong();
}

QString SelectUserDialog::userLogin() const
{
    return mledt->text();
}

/*============================== Private slots =============================*/

void SelectUserDialog::buttonClicked(int id)
{
    if (id)
    {
        delete mledt->validator();
    }
    else
    {
        QIntValidator *v = new QIntValidator(mledt);
        v->setBottom(1);
        mledt->setValidator(v);
    }
    mledt->setFocus();
    mledt->selectAll();
    checkValidity();
}

void SelectUserDialog::checkValidity()
{
    bool b = !mledt->text().isEmpty() && mledt->hasAcceptableInput();
    mfield->setValid(b);
    button(QDialogButtonBox::Ok)->setEnabled(b);
}

/*============================================================================
================================ ConnectionAction ============================
============================================================================*/

/*============================== Public constructors =======================*/

ConnectionAction::ConnectionAction(QObject *parent) :
    QWidgetAction(parent)
{
    //
}

/*============================== Public methods ============================*/

void ConnectionAction::resetIcon(const QString &toolTip, const QString &iconName, bool animated)
{
    setToolTip(toolTip);
    setIcon(BApplication::icon(iconName));
    foreach (QWidget *wgt, createdWidgets())
    {
        if (QString("QLabel") != wgt->metaObject()->className())
        {
            QToolButton *tbtn = static_cast<QToolButton *>(wgt);
            tbtn->setIcon(BApplication::icon(iconName));
            tbtn->setToolTip(toolTip);
            QLabel *lbl = tbtn->findChild<QLabel *>();
            if (animated)
            {
                tbtn->setText("");
                tbtn->setIcon(QIcon());
                lbl->setVisible(true);
                lbl->movie()->start();
            }
            else
            {
                lbl->movie()->stop();
                lbl->setVisible(false);
            }
        }
    }
}

/*============================== Protected methods =========================*/

QWidget *ConnectionAction::createWidget(QWidget *parent)
{
    if (!parent || QString("QMenu") == parent->metaObject()->className())
        return 0;
    QToolButton *tbtn = new QToolButton(parent);
      tbtn->setMenu(this->menu());
      tbtn->setPopupMode(QToolButton::InstantPopup);
      tbtn->setLayout(new QVBoxLayout);
      tbtn->layout()->setContentsMargins(0, 0, 0, 0);
        QLabel *lbl = new QLabel(tbtn);
        lbl->setAlignment(Qt::AlignCenter);
        QMovie *mov = new QMovie(BDirTools::findResource("icons/process.gif", BDirTools::GlobalOnly));
        mov->setScaledSize(tbtn->iconSize());
        lbl->setMovie(mov);
      tbtn->layout()->addWidget(lbl);
    return tbtn;
}

void ConnectionAction::deleteWidget(QWidget *widget)
{
    if (!widget)
        return;
    widget->deleteLater();
}

/*============================================================================
================================ ClabWidget =================================
============================================================================*/

/*============================== Public constructors =======================*/

ClabWidget::ClabWidget(MainWindow *window) :
    QWidget(window), Window(window)
{
    mlastId = 0;
    mproxyModel = new LabsProxyModel(this);
    mproxyModel->setSourceModel(sModel);
    connect(sClient, SIGNAL(stateChanged(Client::State)), this, SLOT(clientStateChanged(Client::State)));
    connect(sClient, SIGNAL(accessLevelChanged(int)), this, SLOT(clientAccessLevelChanged(int)));
    //
    QVBoxLayout *vlt = new QVBoxLayout(this);
      mtbar = new QToolBar(this);
        mactConnection = new ConnectionAction(this);
          QMenu *mnu = new QMenu;
            mactConnect = new QAction(this);
              mactConnect->setEnabled(sClient->canConnect());
              mactConnect->setIcon(Application::icon("connect_established"));
              connect(mactConnect, SIGNAL(triggered()), sClient, SLOT(connectToServer()));
              connect(sClient, SIGNAL(canConnectChanged(bool)), mactConnect, SLOT(setEnabled(bool)));
            mnu->addAction(mactConnect);
            mactDisconnect = new QAction(this);
              mactDisconnect->setEnabled(sClient->canDisconnect());
              mactDisconnect->setIcon( Application::icon("connect_no") );
              connect(mactDisconnect, SIGNAL(triggered()), sClient, SLOT(disconnectFromServer()));
              connect(sClient, SIGNAL(canDisconnectChanged(bool)), mactDisconnect, SLOT(setEnabled(bool)));
            mnu->addAction(mactDisconnect);
          mactConnection->setMenu(mnu);
        mtbar->addAction(mactConnection);
        mactUpdate = new QAction(this);
          mactUpdate->setEnabled(sClient->isAuthorized());
          mactUpdate->setIcon(BApplication::icon("reload"));
          connect(mactUpdate, SIGNAL(triggered()), this, SLOT(updateLabsList()));
          connect(sClient, SIGNAL(authorizedChanged(bool)), mactUpdate, SLOT(setEnabled(bool)));
        mtbar->addAction(mactUpdate);
        mactSend = new QAction(this);
          mactSend->setEnabled(sClient->isAuthorized());
          mactSend->setIcon(BApplication::icon("mail_send"));
          connect(sClient, SIGNAL(authorizedChanged(bool)), mactSend, SLOT( setEnabled(bool) ) );
          connect(mactSend, SIGNAL(triggered()), this, SLOT(addLab()));
        mtbar->addAction(mactSend);
        static_cast<QToolButton *>(mtbar->widgetForAction(mactSend))->setPopupMode(QToolButton::InstantPopup);
        mactTools = new QAction(this);
          mactTools->setIcon(Application::icon("configure"));
          mnu = new QMenu;
            mactRegister = new QAction(this);
              mactRegister->setIcon(Application::icon("add_user"));
              connect(mactRegister, SIGNAL(triggered()), this, SLOT(actRegisterTriggered()));
            mnu->addAction(mactRegister);
            mnu->addSeparator();
            mactRecover = new QAction(this);
              mactRecover->setIcon(Application::icon("account_recover"));
              connect(mactRecover, SIGNAL(triggered()), this, SLOT(actRecoverTriggered()));
            mnu->addAction(mactRecover);
            mnu->addSeparator();
            mactSettings = new QAction(this);
              mactSettings->setIcon(Application::icon("configure"));
              connect(mactSettings, SIGNAL(triggered()), this, SLOT(showSettings()));
            mnu->addAction(mactSettings);
            mactAccountSettings = new QAction(this);
              mactAccountSettings->setEnabled(sClient->isAuthorized());
              mactAccountSettings->setIcon(Application::icon("user"));
              connect(mactAccountSettings, SIGNAL(triggered()), this, SLOT(showAccountSettings()));
              connect(sClient, SIGNAL(authorizedChanged(bool)), mactAccountSettings, SLOT(setEnabled(bool)));
            mnu->addAction(mactAccountSettings);
          mactTools->setMenu(mnu);
        mtbar->addAction(mactTools);
        static_cast<QToolButton *>(mtbar->widgetForAction(mactTools))->setPopupMode(QToolButton::InstantPopup);
        mactAdministration = new QAction(this);
          mactAdministration->setEnabled(sClient->accessLevel() >= TAccessLevel::ModeratorLevel);
          mactAdministration->setIcon(Application::icon("gear"));
          mnu = new QMenu;
            mactAddUser = mnu->addAction(Application::icon("add_user"), "", this, SLOT(addUser()));
              mactAddUser->setEnabled(sClient->accessLevel() >= TAccessLevel::AdminLevel);
            mactEditUser = mnu->addAction(Application::icon("edit_user"), "", this, SLOT(editUser()));
              mactEditUser->setEnabled(sClient->accessLevel() >= TAccessLevel::AdminLevel);
            mactInvites = mnu->addAction(Application::icon("mail_send"), "", this, SLOT(actInvitesTriggered()));
              mactInvites->setEnabled(sClient->accessLevel() >= TAccessLevel::ModeratorLevel);
            mactGroups = mnu->addAction(Application::icon("users"), "", this, SLOT(editGroups()));
              mactGroups->setEnabled(sClient->accessLevel() >= TAccessLevel::AdminLevel);
          mactAdministration->setMenu(mnu);
        mtbar->addAction(mactAdministration);
        static_cast<QToolButton *>(mtbar->widgetForAction(mactAdministration))->setPopupMode(
                    QToolButton::InstantPopup);
      vlt->addWidget(mtbar);
      //
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
      //
      mtblvw = new QTableView(this);
        mtblvw->setAlternatingRowColors(true);
        mtblvw->setEditTriggers(QTableView::NoEditTriggers);
        mtblvw->setSelectionBehavior(QTableView::SelectRows);
        mtblvw->setSelectionMode(QTableView::SingleSelection);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        mtblvw->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#endif
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
    mtblvw->horizontalHeader()->restoreState(tableHeaderState());
}

ClabWidget::~ClabWidget()
{
    setTableHeaderState(mtblvw->horizontalHeader()->saveState());
}

/*============================== Public methods ============================*/

QList<QAction *> ClabWidget::toolBarActions() const
{
    QList<QAction *> list;
    list << mactConnection;
    list << mactUpdate;
    list << mactSend;
    list << mactTools;
    list << mactAdministration;
    return list;
}

/*============================== Static private methods ====================*/

void ClabWidget::setTableHeaderState(const QByteArray &state)
{
    bSettings->setValue("ClabWidget/table_header_state", state);
}

QByteArray ClabWidget::tableHeaderState()
{
    return bSettings->value("ClabWidget/table_header_state").toByteArray();
}

/*============================== Private methods ===========================*/

void ClabWidget::showAddingLabFailedMessage(const QString &errorString)
{
    QMessageBox msg(this);
    msg.setWindowTitle(tr("Sending lab error", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Critical);
    msg.setText(tr("Failed to send lab due to the following error:", "msgbox text"));
    msg.setInformativeText(errorString);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

void ClabWidget::showEditingLabFailedMessage(const QString &errorString)
{
    QMessageBox msg(this);
    msg.setWindowTitle(tr("Editing lab error", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Critical);
    msg.setText(tr("Failed to edit lab due to the following error:", "msgbox text"));
    msg.setInformativeText(errorString);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

/*============================== Private slots =============================*/

void ClabWidget::retranslateUi()
{
    mactConnection->setText(tr("Connection", "act text"));
    mactConnection->setWhatsThis(tr("This action shows current connection state. "
                                    "Use it to connect or disconnect from the server", "act whatsThis"));
    clientStateChanged(sClient->state());
    mactConnect->setText(tr("Connect", "act text"));
    mactDisconnect->setText(tr("Disconnect", "act text"));
    mactUpdate->setText(tr("Update", "act text"));
    mactUpdate->setToolTip(tr("Update labs list", "act toolTip"));
    mactSend->setText(tr("Send lab...", "act text"));
    mactTools->setText(tr("Tools", "act text"));
    mactTools->setToolTip(tr("Tools", "act toolTip"));
    mactRegister->setText( tr("Register...", "act tooTip") );
    mactRecover->setText(tr("Recover account...", "act text"));
    mactSettings->setText(tr("CloudLab settings...", "act text"));
    mactAccountSettings->setText(tr("Account management...", "act text"));
    mactAdministration->setText(tr("Administration", "act text"));
    mactAddUser->setText(tr("Add user...", "act text"));
    mactEditUser->setText(tr("Edit user...", "act text"));
    mactInvites->setText(tr("Manage invites...", "act text"));
    mactGroups->setText(tr("Manage groups...", "act text"));
    //
    mgboxSelect->setTitle(tr("Selection", "gbox title"));
    //
    mlblSearch->setText(tr("Search:", "lbl text"));
}

void ClabWidget::actRegisterTriggered()
{
    if (!Application::showRegisterDialog(Window))
        return;
    emit message(tr("You have successfully registered", "message"));
    bool b = sClient->isAuthorized();
    sClient->updateSettings();
    if (!b)
        sClient->connectToServer();
}

void ClabWidget::actRecoverTriggered()
{
    TRecoveryDialog(&Client::getRecoveryCode, &Client::recoverAccount, this).exec();
}

void ClabWidget::addUser()
{
    QStringList groups;
    if (!sClient->getClabGroupsList(groups, this))
    {
        //TODO: Show message
        return;
    }
    BDialog dlg(this);
    dlg.setWindowTitle(tr("Adding user", "dlg windowTitle"));
      TUserWidget *uwgt = new TUserWidget(TUserWidget::AddMode);
        uwgt->setAvailableServices(sClient->services());
        uwgt->setClabGroupsVisible(true);
        uwgt->setAvailableClabGroups(groups);
        uwgt->restorePasswordWidgetState(Global::passwordWidgetState());
      dlg.setWidget(uwgt);
      dlg.addButton(QDialogButtonBox::Ok, SLOT(accept()));
      dlg.button(QDialogButtonBox::Ok)->setEnabled(uwgt->isValid());
      connect(uwgt, SIGNAL(validityChanged(bool)), dlg.button(QDialogButtonBox::Ok), SLOT(setEnabled(bool)));
      dlg.addButton(QDialogButtonBox::Cancel, SLOT(reject()));
      dlg.setFixedHeight(dlg.sizeHint().height());
      dlg.setMinimumWidth(600);
    while (dlg.exec() == QDialog::Accepted)
    {
        TOperationResult r = sClient->addUser(uwgt->info(), uwgt->clabGroups(), this);
        if (r)
        {
            Global::setPasswordWidgetSate(uwgt->savePasswordWidgetState());
            emit message(tr("User was successfully added", "message"));
            return;
        }
        else
        {
            QMessageBox msg(dlg.parentWidget());
            msg.setWindowTitle(tr("Adding user error", "msgbox windowTitle"));
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Failed to add user due to the following error:", "msgbox text"));
            msg.setInformativeText(r.messageString());
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
        }
    }
    Global::setPasswordWidgetSate(uwgt->savePasswordWidgetState());
}

void ClabWidget::editUser()
{
    SelectUserDialog sdlg(this);
    if (sdlg.exec() != SelectUserDialog::Accepted)
        return;
    if (sClient->userId() == sdlg.userId() || Global::login() == sdlg.userLogin())
    {
        QMessageBox msg(this);
        msg.setWindowTitle(tr("Editing own account", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Information);
        msg.setText(tr("You are not allowed to edit your own account. Use \"Account management\" instead",
                       "msgbox text"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }
    QStringList groups;
    if (!sClient->getClabGroupsList(groups, this))
    {
        //TODO: Show message
        return;
    }
    TUserInfo info;
    QStringList userGroups;
    bool b = sdlg.userId() ? sClient->getUserInfo(sdlg.userId(), info, userGroups, this) :
                             sClient->getUserInfo(sdlg.userLogin(), info, userGroups, this);
    if (!b)
        return;
    BDialog dlg(this);
    dlg.setWindowTitle(tr("Editing user", "dlg windowTitle"));
      TUserWidget *uwgt = new TUserWidget(TUserWidget::EditMode);
        uwgt->setAvailableServices(sClient->services());
        uwgt->setClabGroupsVisible(true);
        uwgt->setAvailableClabGroups(groups);
        uwgt->setClabGroups(userGroups);
        uwgt->restorePasswordWidgetState(Global::passwordWidgetState());
        uwgt->setInfo(info);
      dlg.setWidget(uwgt);
      dlg.addButton(QDialogButtonBox::Ok, SLOT(accept()));
      dlg.button(QDialogButtonBox::Ok)->setEnabled(uwgt->isValid());
      connect(uwgt, SIGNAL(validityChanged(bool)), dlg.button(QDialogButtonBox::Ok), SLOT(setEnabled(bool)));
      dlg.addButton(QDialogButtonBox::Cancel, SLOT(reject()));
      dlg.setFixedHeight(dlg.sizeHint().height());
      dlg.setMinimumWidth(600);
    while (dlg.exec() == QDialog::Accepted)
    {
        TOperationResult r = sClient->editUser(uwgt->info(), uwgt->clabGroups(), this);
        if (r)
        {
            Global::setPasswordWidgetSate(uwgt->savePasswordWidgetState());
            emit message(tr("User info was successfully edited", "message"));
            return;
        }
        else
        {
            QMessageBox msg(this);
            msg.setWindowTitle(tr("Editing user error", "msgbox windowTitle"));
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Failed to edit user due to the following error:", "msgbox text"));
            msg.setInformativeText(r.messageString());
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
        }
    }
    Global::setPasswordWidgetSate(uwgt->savePasswordWidgetState());
}

void ClabWidget::actInvitesTriggered()
{
    TInvitesDialog dlg(&Client::hasAccessToService, &Client::getInvitesList, &Client::generateInvites, this);
    QStringList list;
    if (!sClient->getClabGroupsList(list, this))
    {
        //TODO: Show message
        return;
    }
    dlg.setAvailableClabGroups(list);
    dlg.exec();
}

void ClabWidget::editGroups()
{
    QStringList groups;
    if (!sClient->getClabGroupsList(groups, this))
    {
        //TODO: Show message
        return;
    }
    BDialog dlg(this);
      TListWidget *lstwgt = new TListWidget;
        lstwgt->setAvailableItems(groups);
        lstwgt->setItems(groups);
      dlg.setWidget(lstwgt);
      dlg.addButton(QDialogButtonBox::Ok, SLOT(accept()));
      dlg.addButton(QDialogButtonBox::Cancel, SLOT(reject()));
    if (dlg.exec() != BDialog::Accepted)
        return;
    QStringList newGroups = lstwgt->items();
    QStringList deletedGroups;
    foreach (const QString &g, groups)
    {
        if (newGroups.contains(g))
            newGroups.removeAll(g);
        else
            deletedGroups << g;
    }
    if (!sClient->editClabGroups(newGroups, deletedGroups))
    {
        //TODO: Show message
        return;
    }
    emit message(tr("Groups have been successfully edited", "message"));
}

void ClabWidget::addLab()
{
    if (!maddDialog.isNull())
        return maddDialog->activateWindow();
    maddDialog = new AddLabDialog(this);
    maddDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(maddDialog.data(), SIGNAL(finished(int)), this, SLOT(addDialogFinished()));
    maddDialog->show();
}

void ClabWidget::editLab()
{
    if (!mlastId)
        return;
    if (meditDialogMap.contains(mlastId))
    {
        if (meditDialogMap.value(mlastId).isNull())
        {
            meditDialogMap.remove(mlastId);
            meditDialogIdMap.remove(QPointer<QObject>());
        }
        else
        {
            return meditDialogMap.value(mlastId)->activateWindow();
        }
    }
    if (!sModel->lab(mlastId))
        return;
    EditLabDialog *dlg = new EditLabDialog(mlastId, this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    meditDialogMap.insert(mlastId, dlg);
    meditDialogIdMap.insert(dlg, mlastId);
    connect(dlg, SIGNAL(finished(int)), this, SLOT(editDialogFinished()));
    dlg->show();
}

void ClabWidget::deleteLab()
{
    if (!mlastId)
        return;
    QString title = tr("Deleting lab", "idlg title");
    QString lblText = tr("You are going to delete a lab. Please, enter the reason:", "idlg lblText");
    bool ok = false;
    QString reason = QInputDialog::getText(this, title, lblText, QLineEdit::Normal, QString(), &ok);
    if (!ok)
        return;
    TOperationResult r = sClient->deleteLab(mlastId, reason, this);
    if (!r)
    {
        QMessageBox msg(this);
        msg.setWindowTitle(tr("Deleting lab error", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Failed to delete lab due to the following error:", "msgbox text"));
        msg.setInformativeText(r.messageString());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }
    emit message(tr("Lab was successfully deleted", "message"));
}

void ClabWidget::updateLabsList()
{
    sClient->updateLabsList(false, this);
}

void ClabWidget::showLabInfo()
{
    if (!mlastId)
        return;
    if (minfoDialogMap.contains(mlastId))
    {
        if (minfoDialogMap.value(mlastId).isNull())
        {
            minfoDialogMap.remove(mlastId);
            minfoDialogIdMap.remove(QPointer<QObject>());
        }
        else
        {
            return minfoDialogMap.value(mlastId)->activateWindow();
        }
    }
    const TLabInfo *l = sModel->lab(mlastId);
    if (!l)
        return;
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle(tr("Lab:", "windowTitle") + " " + l->title());
    QVBoxLayout *vlt = new QVBoxLayout(dlg);
      LabWidget *lwgt = new LabWidget(LabWidget::ShowMode);
        lwgt->setInfo(*l);
      vlt->addWidget(lwgt);
      vlt->addStretch();
      QDialogButtonBox *dlgbbox = new QDialogButtonBox;
        dlgbbox->addButton(QDialogButtonBox::Close);
        connect(dlgbbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), dlg, SLOT(close()));
      vlt->addWidget(dlgbbox);
      dlg->resize(bSettings->value("ClabWidget/lab_info_dialog_size", QSize(750, 550)).toSize());
    minfoDialogMap.insert(mlastId, dlg);
    minfoDialogIdMap.insert(dlg, mlastId);
    connect(dlg, SIGNAL(finished(int)), this, SLOT(infoDialogFinished()));
    dlg->show();
}

void ClabWidget::getLab()
{
    if (!mlastId)
        return;
    TOperationResult r = sClient->showLab(mlastId, this);
    if (!r)
    {
        QMessageBox msg(window());
        msg.setWindowTitle(tr("Getting lab failed", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Failed to get lad due to the following error:", "msgbox text"));
        msg.setInformativeText(r.messageString());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }
    emit message(tr("Lab was successfully downloaded", "message"));
}

void ClabWidget::showSettings()
{
    BSettingsDialog(new TexsampleSettingsTab, window()).exec();
}

void ClabWidget::showAccountSettings()
{
    if (!sClient->isAuthorized())
        return;
    Application::showSettings(window());
}

void ClabWidget::clientStateChanged(Client::State state)
{
    QString s = tr("CloudLab state:", "act toolTip") + " ";
    switch (state)
    {
    case Client::DisconnectedState:
        mactConnection->resetIcon(s + tr("disconnected", "act toolTip"), "connect_no");
        break;
    case Client::ConnectingState:
        mactConnection->resetIcon(s + tr("connecting", "act toolTip"), "process", true);
        break;
    case Client::ConnectedState:
        mactConnection->resetIcon(s + tr("connected", "act toolTip"), "process", true);
        break;
    case Client::AuthorizedState:
        mactConnection->resetIcon(s + tr("authorized", "act toolTip"), "connect_established");
        break;
    case Client::DisconnectingState:
        mactConnection->resetIcon(s + tr("disconnecting", "act toolTip"), "process", true);
        break;
    default:
        break;
    }
}

void ClabWidget::clientAccessLevelChanged(int lvl)
{
    mactAdministration->setEnabled(lvl >= TAccessLevel::ModeratorLevel);
    mactAddUser->setEnabled(lvl >= TAccessLevel::AdminLevel);
    mactEditUser->setEnabled(lvl >= TAccessLevel::AdminLevel);
    mactInvites->setEnabled(lvl >= TAccessLevel::ModeratorLevel);
    mactGroups->setEnabled(lvl >= TAccessLevel::AdminLevel);
    mactGroups->setEnabled(sClient->accessLevel() >= TAccessLevel::AdminLevel);
}

void ClabWidget::tblvwDoubleClicked(const QModelIndex &index)
{
    mlastId = sModel->indexAt( index.row() );
    getLab();
}

void ClabWidget::tblvwCustomContextMenuRequested(const QPoint &pos)
{
    mlastId = sModel->indexAt(mtblvw->indexAt(pos).row());
    if (!mlastId)
        return;
    QMenu mnu;
    QAction *act = mnu.addAction(tr("Get", "act text"));
      act->setEnabled(sClient->isAuthorized());
      act->setIcon(Application::icon("editpaste"));
      connect(act, SIGNAL(triggered()), this, SLOT(getLab()));
    mnu.addSeparator();
    act = mnu.addAction(tr("Information...", "act text"));
      act->setIcon(Application::icon("help_about"));
      connect(act, SIGNAL(triggered()), this, SLOT(showLabInfo()));
    mnu.addSeparator();
    act = mnu.addAction( tr("Edit...", "act text") );
      act->setEnabled(sClient->isAuthorized() && sClient->accessLevel() >= TAccessLevel::ModeratorLevel);
      act->setIcon(Application::icon("edit"));
      connect(act, SIGNAL(triggered()), this, SLOT(editLab()));
    act = mnu.addAction(tr("Delete...", "act text"));
      act->setEnabled(sClient->isAuthorized() && sClient->accessLevel() >= TAccessLevel::AdminLevel);
      act->setIcon(Application::icon("editdelete"));
      connect(act, SIGNAL(triggered()), this, SLOT(deleteLab()));
    mnu.exec(mtblvw->mapToGlobal(pos));
}

void ClabWidget::infoDialogFinished()
{
    QDialog *dlg = qobject_cast<QDialog *>(sender());
    if (!dlg)
        return;
    bSettings->setValue("ClabWidget/lab_info_dialog_size", dlg->size());
    minfoDialogMap.remove(minfoDialogIdMap.take(dlg));
    dlg->deleteLater();
}

void ClabWidget::addDialogFinished()
{
    if (maddDialog.isNull())
        return;
    if (maddDialog->result() == AddLabDialog::Accepted)
    {
        LabWidget *labwgt = maddDialog->labWidget();
        TLabInfo info = labwgt->info();
        TProjectFileList extra;
        foreach (const QString &fn, labwgt->extraAttachedFiles())
        {
            TProjectFile pf(fn, TProjectFile::Binary);
            if (!pf.isValid())
                return showAddingLabFailedMessage(tr("Failed to load file:") + " " + fn);
            extra << pf;
        }
        TOperationResult r;
        switch (info.type())
        {
        case TLabInfo::DesktopType:
            r = sClient->addLab(info, labwgt->linuxProject(), labwgt->macProject(), labwgt->winProject(), extra, this);
            break;
        case TLabInfo::WebType:
            r = sClient->addLab(info, labwgt->webProject(), extra, this);
            break;
        case TLabInfo::UrlType:
            r = sClient->addLab(info, labwgt->url(), extra, this);
            break;
        default:
            break;
        }
        if (!r)
            return showAddingLabFailedMessage(r.messageString());
        emit message(tr("Lab was successfully sent", "message"));
    }
    maddDialog->close();
    maddDialog->deleteLater();
}

void ClabWidget::editDialogFinished()
{
    EditLabDialog *dlg = qobject_cast<EditLabDialog *>(sender());
    if (!dlg)
        return;
    if (dlg->result() == EditLabDialog::Accepted)
    {
        TLabInfo info = dlg->labWidget()->info();
        LabWidget *labwgt = dlg->labWidget();
        QStringList initialFiles = dlg->initialExtraFiles();
        QStringList extraFiles = labwgt->extraAttachedFiles();
        QStringList deletedFiles;
        foreach (int i, bRangeR(extraFiles.size() - 1, 0))
        {
            const QString &fn = extraFiles.at(i);
            if (initialFiles.contains(fn))
                extraFiles.removeAll(fn);
        }
        foreach (const QString &fn, initialFiles)
            if (!extraFiles.contains(fn))
                deletedFiles << fn;
        TProjectFileList extra;
        foreach (const QString &fn, extraFiles)
        {
            TProjectFile pf(fn, TProjectFile::Binary);
            if (!pf.isValid())
                return showEditingLabFailedMessage(tr("Failed to load file:") + " " + fn);
            extra << pf;
        }
        TOperationResult r;
        switch (info.type())
        {
        case TLabInfo::DesktopType:
            r = sClient->editLab(info, labwgt->linuxProject(), labwgt->macProject(), labwgt->winProject(),
                                 deletedFiles, extra, this);
            break;
        case TLabInfo::WebType:
            r = sClient->editLab(info, labwgt->webProject(), deletedFiles, extra, this);
            break;
        case TLabInfo::UrlType:
            r = sClient->editLab(info, labwgt->url(), deletedFiles, extra, this);
            break;
        default:
            break;
        }
        if (!r)
            return showEditingLabFailedMessage(r.messageString());
        emit message(tr("Lab was successfully edited", "message"));
    }
    meditDialogMap.remove(meditDialogIdMap.take(dlg));
    dlg->close();
    dlg->deleteLater();
}
