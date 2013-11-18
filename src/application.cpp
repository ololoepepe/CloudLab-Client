#include "application.h"
#include "client.h"
#include "texsamplesettingstab.h"
#include "mainwindow.h"
#include "global.h"

#include <TUserInfo>
#include <TOperationResult>
#include <TUserWidget>

#include <BApplication>
#include <BSettingsDialog>
#include <BPasswordWidget>
#include <BAbstractSettingsTab>
#include <BLocaleComboBox>
#include <BDialog>

#include <QObject>
#include <QVariantMap>
#include <QByteArray>
#include <QList>
#include <QMessageBox>
#include <QApplication>
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QSize>
#include <QPushButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QSettings>

#include <QDebug>

/*============================================================================
================================ GeneralSettingsTab ==========================
============================================================================*/

class GeneralSettingsTab : public BAbstractSettingsTab
{
    Q_DECLARE_TR_FUNCTIONS(GeneralSettingsTab)
public:
    explicit GeneralSettingsTab();
public:
    QString title() const;
    QIcon icon() const;
    bool hasDefault() const;
    bool restoreDefault();
    bool saveSettings();
private:
    BLocaleComboBox *mlcmbox;
    QCheckBox *mcboxMultipleWindows;
private:
    Q_DISABLE_COPY(GeneralSettingsTab)
};

/*============================================================================
================================ PasswordDialog ==============================
============================================================================*/

class PasswordDialog : public QDialog
{
    Q_DECLARE_TR_FUNCTIONS(PasswordDialog)
public:
    explicit PasswordDialog(QWidget *parent = 0);
public:
    void setLogin(const QString &login);
    void setPasswordState(const QByteArray &state);
    QString login() const;
    QByteArray passwordState() const;
private:
    QLineEdit *ledt;
    BPasswordWidget *mpwdwgt;
};

/*============================================================================
================================ GeneralSettingsTab ==========================
============================================================================*/

/*============================== Public constructors =======================*/

GeneralSettingsTab::GeneralSettingsTab() :
    BAbstractSettingsTab()
{
    QFormLayout *flt = new QFormLayout(this);
    mlcmbox = new BLocaleComboBox(true, this);
      mlcmbox->setCurrentLocale( Application::locale() );
    flt->addRow("Language (" + tr("language", "lbl text") + "):", mlcmbox);
    mcboxMultipleWindows = new QCheckBox(this);
      mcboxMultipleWindows->setChecked(Global::multipleWindowsEnabled());
    flt->addRow(tr("Enable multiple windows:", "lbl text"), mcboxMultipleWindows);
}

/*============================== Public methods ============================*/

QString GeneralSettingsTab::title() const
{
    return tr("General", "title");
}

QIcon GeneralSettingsTab::icon() const
{
    return Application::icon("configure");
}

bool GeneralSettingsTab::hasDefault() const
{
    return true;
}

bool GeneralSettingsTab::restoreDefault()
{
    mcboxMultipleWindows->setChecked(false);
    return true;
}

bool GeneralSettingsTab::saveSettings()
{
    if (Global::multipleWindowsEnabled() && !mcboxMultipleWindows->isChecked() && !Application::mergeWindows())
    {
        QMessageBox msg(this);
        msg.setWindowTitle( tr("Failed to change settings", "msgbox windowTitle") );
        msg.setIcon(QMessageBox::Information);
        msg.setText( tr("Can't disable multiple windows: some error occured", "msgbox text") );
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return false;
    }
    Application::setLocale(mlcmbox->currentLocale());
    Global::setMultipleWindowsEnabled(mcboxMultipleWindows->isChecked());
    return true;
}

/*============================================================================
================================ PasswordDialog ==============================
============================================================================*/

/*============================== Public constructors =======================*/

PasswordDialog::PasswordDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Login and password", "windowTitle"));
    QVBoxLayout *vlt = new QVBoxLayout(this);
      QFormLayout *flt = new QFormLayout;
        ledt = new QLineEdit;
          ledt->setText(Global::login());
          ledt->selectAll();
          ledt->setFocus();
        flt->addRow(tr("Login:", "lbl text"), ledt);
        mpwdwgt = new BPasswordWidget;
          mpwdwgt->restoreWidgetState(Global::passwordWidgetState());
        flt->addRow(tr("Password:", "lbl text"), mpwdwgt);
      vlt->addLayout(flt);
      QDialogButtonBox *dlgbbox = new QDialogButtonBox(this);
        QPushButton *btnOk = dlgbbox->addButton(QDialogButtonBox::Ok);
        btnOk->setDefault(true);
        connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
        connect(dlgbbox->addButton(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
      vlt->addWidget(dlgbbox);
    //
    setFixedSize(sizeHint());
}

/*============================== Public methods ============================*/

void PasswordDialog::setLogin(const QString &login)
{
    ledt->setText(login);
    ledt->selectAll();
    ledt->setFocus();
}

void PasswordDialog::setPasswordState(const QByteArray &state)
{
    mpwdwgt->restorePasswordState(state);
}

QString PasswordDialog::login() const
{
    return ledt->text();
}

QByteArray PasswordDialog::passwordState() const
{
    return mpwdwgt->savePasswordState(BPassword::AlwaysEncryptedMode);
}

/*============================================================================
================================ Application =================================
============================================================================*/

/*============================== Public constructors =======================*/

Application::Application() :
    BApplication()
{
    minitialWindowCreated = false;
    Global::loadPasswordState();
}

Application::~Application()
{
    Global::savePasswordState();
}

/*============================== Static public methods =====================*/

void Application::createInitialWindow(const QStringList &args)
{
    if (!testAppInit())
        return;
    if (!bTest(!bApp->minitialWindowCreated, "Application", "Initial window may be created only once"))
        return;
    bApp->addMainWindow(args);
    bApp->minitialWindowCreated = true;
    if (!Global::hasTexsample())
    {
        QMessageBox msg(mostSuitableWindow());
        msg.setWindowTitle( tr("TeXSample configuration", "msgbox windowTitle") );
        msg.setIcon(QMessageBox::Question);
        msg.setText( tr("It seems that you have not configured TeXSample service yet.\n"
                        "Would you like to do it now?", "msgbox text") );
        msg.setInformativeText( tr("To remove this notification, you have to configure the service",
                                   "msgbox informativeText") );
        QPushButton *btn1 = msg.addButton(tr("Register", "btn text"), QMessageBox::AcceptRole);
        QPushButton *btn2 = msg.addButton(tr("I have an account", "btn text"), QMessageBox::AcceptRole);
        msg.addButton(tr("Not right now", "btn text"), QMessageBox::RejectRole);
        msg.setDefaultButton(btn2);
        msg.exec();
        if (msg.clickedButton() == btn1)
        {
            if (!showRegisterDialog())
                return;
            Global::setAutoconnection(true);
        }
        else if (msg.clickedButton() == btn2)
        {
            if (BSettingsDialog(new TexsampleSettingsTab).exec() != BSettingsDialog::Accepted)
                return;
            sClient->connectToServer();
        }
    }
    else if (Global::autoconnection())
    {
        sClient->connectToServer();
    }
}

QWidget *Application::mostSuitableWindow()
{
    if (!testAppInit())
        return 0;
    QWidget *wgt = QApplication::activeWindow();
    QList<MainWindow *> list = bApp->mmainWindows.values();
    foreach (MainWindow *mw, list)
        if (mw == wgt)
            return wgt;
    return !list.isEmpty() ? list.first() : 0;
}

bool Application::mergeWindows()
{
    if (bApp->mmainWindows.size() < 2)
        return true;
    QList<MainWindow *> list = bApp->mmainWindows.values();
    MainWindow *first = list.takeFirst();
    foreach (MainWindow *mw, list)
        mw->close();
    first->activateWindow();
    return true;
}

void Application::handleExternalRequest(const QStringList &args)
{
    if (!testAppInit())
        return;
    if (Global::multipleWindowsEnabled())
    {
        bApp->addMainWindow(args);
    }
    else
    {
        MainWindow *mw = !bApp->mmainWindows.isEmpty() ? bApp->mmainWindows.values().first() : 0;
        if (!mw)
            return;
    }
}

bool Application::showPasswordDialog(QWidget *parent)
{
    PasswordDialog pd(parent ? parent : mostSuitableWindow());
    if (pd.exec() != QDialog::Accepted)
        return false;
    Global::setLogin(pd.login());
    Global::setPasswordState(pd.passwordState());
    sClient->updateSettings();
    return true;
}

bool Application::showSettings(QWidget *parent)
{
    BDialog dlg(parent ? parent : mostSuitableWindow());
      dlg.setWindowTitle(tr("Updating account", "dlg windowTitle"));
      TUserWidget *uwgt = new TUserWidget(TUserWidget::UpdateMode);
        TUserInfo info(TUserInfo::UpdateContext);
        QStringList groups;
        sClient->getUserInfo(sClient->userId(), info, groups, parent);
        uwgt->setInfo(info);
        uwgt->setClabGroups(groups);
        uwgt->restorePasswordWidgetState(Global::passwordWidgetState());
        uwgt->restoreState(bSettings->value("UpdateUserDialog/user_widget_state").toByteArray());
        uwgt->setPassword(Global::password());
      dlg.setWidget(uwgt);
      dlg.addButton(QDialogButtonBox::Ok, SLOT(accept()));
      dlg.button(QDialogButtonBox::Ok)->setEnabled(uwgt->isValid());
      connect(uwgt, SIGNAL(validityChanged(bool)), dlg.button(QDialogButtonBox::Ok), SLOT(setEnabled(bool)));
      dlg.addButton(QDialogButtonBox::Cancel, SLOT(reject()));
      dlg.setMinimumSize(600, dlg.sizeHint().height());
      if (dlg.exec() != BDialog::Accepted)
          return false;
    info = uwgt->info();
    TOperationResult r = sClient->updateAccount(info, parent);
    if (r)
    {
        Global::setPassword(uwgt->password());
        Global::setPasswordWidgetSate(uwgt->savePasswordWidgetState());
        if (!sClient->updateSettings())
            sClient->reconnect();
        return true;
    }
    else
    {
        QMessageBox msg(parent);
        msg.setWindowTitle(tr("Changing account failed", "msgbox windowTitle"));
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("The following error occured:", "msgbox text"));
        msg.setInformativeText(r.messageString());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        Global::setPasswordWidgetSate(uwgt->savePasswordWidgetState());
        return false;
    }
}

bool Application::showRegisterDialog(QWidget *parent)
{
    QDialog dlg(parent ? parent : mostSuitableWindow());
    dlg.setWindowTitle(tr("Registration", "dlg windowTitle"));
    QVBoxLayout *vlt = new QVBoxLayout(&dlg);
      TUserWidget *uwgt = new TUserWidget(TUserWidget::RegisterMode);
        uwgt->restorePasswordWidgetState(Global::passwordWidgetState());
      vlt->addWidget(uwgt);
      vlt->addStretch();
      QDialogButtonBox *dlgbbox = new QDialogButtonBox;
        dlgbbox->addButton(QDialogButtonBox::Ok);
        dlgbbox->button(QDialogButtonBox::Ok)->setEnabled(false);
        connect(uwgt, SIGNAL(validityChanged(bool)), dlgbbox->button(QDialogButtonBox::Ok), SLOT(setEnabled(bool)));
        connect(dlgbbox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), &dlg, SLOT(accept()));
        dlgbbox->addButton(QDialogButtonBox::Cancel);
        connect(dlgbbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), &dlg, SLOT(reject()));
      vlt->addWidget(dlgbbox);
      dlg.setMinimumWidth(700);
      dlg.setFixedHeight(dlg.sizeHint().height());
    while (dlg.exec() == QDialog::Accepted)
    {
        TUserInfo info = uwgt->info();
        TOperationResult r = Client::registerUser(info, dlg.parentWidget());
        if (r)
        {
            Global::setLogin(info.login());
            Global::setPassword(uwgt->password());
            Global::setPasswordWidgetSate(uwgt->savePasswordWidgetState());
            sClient->updateSettings();
            sClient->connectToServer();
            return true;
        }
        else
        {
            QMessageBox msg(dlg.parentWidget());
            msg.setWindowTitle(tr("Registration error", "msgbox windowTitle"));
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Failed to register due to the following error:", "msgbox text"));
            msg.setInformativeText(r.messageString());
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
        }
    }
    Global::setPasswordWidgetSate(uwgt->savePasswordWidgetState());
    return false;
}

/*============================== Protected methods =========================*/

QList<BAbstractSettingsTab *> Application::createSettingsTabs() const
{
    QList<BAbstractSettingsTab *> list;
    list << new GeneralSettingsTab;
    list << new TexsampleSettingsTab;
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

/*============================== Private slots =============================*/

void Application::mainWindowDestroyed(QObject *obj)
{
    mmainWindows.remove(obj);
}
