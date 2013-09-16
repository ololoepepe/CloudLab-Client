#include "texsamplesettingstab.h"
#include "application.h"
#include "client.h"
#include "global.h"

#include <BAbstractSettingsTab>
#include <BPasswordWidget>
#include <BDirTools>

#include <QObject>
#include <QVariantMap>
#include <QString>
#include <QIcon>
#include <QCheckBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QVariant>
#include <QByteArray>
#include <QSettings>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QToolButton>
#include <QHBoxLayout>

/*============================================================================
================================ TexsampleSettingsTab ========================
============================================================================*/

/*============================== Public constructors =======================*/

TexsampleSettingsTab::TexsampleSettingsTab() :
    BAbstractSettingsTab()
{
    QVBoxLayout *vlt = new QVBoxLayout(this);
      QGroupBox *gbox = new QGroupBox(tr("Connection", "gbox title"), this);
        QFormLayout *flt = new QFormLayout;
          mhltHost = new QHBoxLayout;
            mcmboxHost = new QComboBox(gbox);
              mcmboxHost->setEditable(true);
              mcmboxHost->setMaxCount(10);
              updateHostHistory(Global::hostHistory());
              int ind = mcmboxHost->findText(Global::host());
              mcmboxHost->setCurrentIndex(ind > 0 ? ind : 0);
              connect(mcmboxHost, SIGNAL(currentIndexChanged(int)), this, SLOT(cmboxHostCurrentIndexChanged(int)));
            mhltHost->addWidget(mcmboxHost);
            mtbtnRemoveFromHistory = new QToolButton(gbox);
              mtbtnRemoveFromHistory->setIcon(Application::icon("editdelete"));
              mtbtnRemoveFromHistory->setToolTip(tr("Remove current host from history", "tbtn toolTip"));
              cmboxHostCurrentIndexChanged(mcmboxHost->currentIndex());
              connect(mtbtnRemoveFromHistory, SIGNAL(clicked()), this, SLOT(removeCurrentHostFromHistory()));
            mhltHost->addWidget(mtbtnRemoveFromHistory);
          flt->addRow(tr("Host:", "lbl text"), mhltHost);
          mledtLogin = new QLineEdit(gbox);
            mledtLogin->setText(Global::login());
          flt->addRow(tr("Login:", "lbl text"), mledtLogin);
          mpwdwgt = new BPasswordWidget(gbox);
            mpwdwgt->restoreWidgetState(Global::passwordWidgetState());
            mpwdwgt->restorePasswordState(Global::passwordState());
          flt->addRow(tr("Password:", "lbl text"), mpwdwgt);
          mcboxAutoconnection = new QCheckBox(gbox);
            mcboxAutoconnection->setChecked(Global::autoconnection());
          flt->addRow(tr("Autoconnection:", "lbl text"), mcboxAutoconnection);
        gbox->setLayout(flt);
      vlt->addWidget(gbox);
}

/*============================== Public methods ============================*/

QString TexsampleSettingsTab::title() const
{
    return tr("CloudLab", "title");
}

QIcon TexsampleSettingsTab::icon() const
{
    return Application::icon("clab-client");
}

bool TexsampleSettingsTab::restoreDefault()
{
    mcmboxHost->setCurrentIndex(0);
    return true;
}

bool TexsampleSettingsTab::saveSettings()
{
    QString nhost = mcmboxHost->currentText();
    if (autoSelectText() == nhost)
        nhost = "auto_select";
    /*if (Global::hasTexsample() && Global::host() != nhost)
    {
        QMessageBox msg(this);
        msg.setWindowTitle( tr("Confirmation", "msgbox windowTitle") );
        msg.setIcon(QMessageBox::Question);
        msg.setText(tr("You are going to change server address. All cached files will be removed.", "msgbox text"));
        msg.setInformativeText( tr("Are you absolutely sure?", "msgbox informativeText") );
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setDefaultButton(QMessageBox::Yes);
        if (msg.exec() != QMessageBox::Yes)
            return false;
    }*/
    Global::setAutoconnection(mcboxAutoconnection->isChecked());
    Global::setHost(nhost);
    Global::setHostHistory(updateHostHistory());
    Global::setLogin(mledtLogin->text());
    Global::setPasswordSate(mpwdwgt->savePasswordState(BPassword::AlwaysEncryptedMode));
    Global::setPasswordWidgetSate(mpwdwgt->saveWidgetState());
    sClient->updateSettings();
    return true;
}

/*============================== Static private methods ====================*/

QString TexsampleSettingsTab::autoSelectText()
{
    return tr("Auto select");
}

/*============================== Private methods ===========================*/

QStringList TexsampleSettingsTab::updateHostHistory(const QStringList &history)
{
    QStringList list = QStringList() << autoSelectText();
    if (history.isEmpty())
    {
        list << mcmboxHost->currentText();
        foreach (int i, bRangeD(1, mcmboxHost->count() - 1))
            list << mcmboxHost->itemText(i);
        list.removeAll("");
        list.removeDuplicates();
    }
    else
    {
        list << history;
    }
    list = list.mid(0, 10);
    mcmboxHost->clear();
    mcmboxHost->addItems(list);
    mcmboxHost->setCurrentIndex(mcmboxHost->count() > 1 ? 1 : 0);
    return list.mid(1);
}

/*============================== Private slots =============================*/

void TexsampleSettingsTab::removeCurrentHostFromHistory()
{
    QString text = mcmboxHost->currentText();
    QStringList list = updateHostHistory();
    list.removeAll(text);
    mcmboxHost->clear();
    Global::setHostHistory(updateHostHistory(list));
}

void TexsampleSettingsTab::cmboxHostCurrentIndexChanged(int index)
{
    mtbtnRemoveFromHistory->setEnabled(index > 0);
}
