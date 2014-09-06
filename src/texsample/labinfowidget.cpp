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

#include "labinfowidget.h"

#include "application.h"
#include "labmodel.h"
#include "settings.h"
#include "texsamplecore.h"

#include <TAddLabRequestData>
#include <TAuthorInfo>
#include <TAuthorInfoList>
#include <TAuthorListWidget>
#include <TEditLabRequestData>
#include <TeXSample>
#include <TFileInfo>
#include <TGroupInfoList>
#include <TGroupListWidget>
#include <TLabInfo>
#include <TLabType>
#include <TNetworkClient>
#include <TTagWidget>

#include <BeQt>
#include <BInputField>

#include <QByteArray>
#include <QChar>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPlainTextEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSpinBox>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QToolButton>
#include <QValidator>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QVBoxLayout>
#include <QWidget>

/*============================================================================
================================ LabInfoWidget ===============================
============================================================================*/

/*============================== Static private constants ==================*/

const QString LabInfoWidget::DateTimeFormat = "dd MMMM yyyy hh:mm";
const Qt::TextInteractionFlags LabInfoWidget::TextInteractionFlags = Qt::TextSelectableByKeyboard
        | Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard;

/*============================== Public constructors =======================*/

LabInfoWidget::LabInfoWidget(Mode m, QWidget *parent) :
    QWidget(parent), mmode(m)
{
    mcache = 0;
    mclient = 0;
    mid = 0;
    mmodel = 0;
    msenderId = 0;
    mvalid = false;
    //
    mledtTitle = 0;
    minputTitle = 0;
    mtgwgt = 0;
    mlblSender = 0;
    mcmboxType = 0;
    mlblSize = 0;
    mlblCreationDT = 0;
    mlblUpdateDT = 0;
    mlstwgtAuthors = 0;
    mptedtDescription = 0;
    mlstwgtGroups = 0;
    mcboxEditData = 0;
    ldlwgt = 0;
    eflwgt = 0;
    //
    QVBoxLayout *vlt = new QVBoxLayout(this);
    switch (mmode) {
    case AddMode: {
        createMainGroup(vlt);
        QHBoxLayout *hlt = new QHBoxLayout;
        createExtraGroup(hlt);
        createAuthorsGroup(hlt);
        vlt->addLayout(hlt);
        hlt = new QHBoxLayout;
        createDescriptionGroup(hlt);
        createGroupsGroup(hlt);
        vlt->addLayout(hlt);
        hlt = new QHBoxLayout;
        createLabDataListGroup(hlt);
        createExtraFileListGroup(hlt);
        vlt->addLayout(hlt);
        break;
    }
    case EditMode: {
        createMainGroup(vlt);
        QHBoxLayout *hlt = new QHBoxLayout;
        createExtraGroup(hlt);
        createAuthorsGroup(hlt);
        vlt->addLayout(hlt);
        hlt = new QHBoxLayout;
        createDescriptionGroup(hlt);
        createGroupsGroup(hlt);
        vlt->addLayout(hlt);
        hlt = new QHBoxLayout;
        createLabDataListGroup(hlt);
        createExtraFileListGroup(hlt);
        vlt->addLayout(hlt);
        break;
    }
    case ShowMode: {
        createMainGroup(vlt, true);
        QHBoxLayout *hlt = new QHBoxLayout;
        createExtraGroup(hlt, true);
        createAuthorsGroup(hlt, true);
        vlt->addLayout(hlt);
        hlt = new QHBoxLayout;
        createDescriptionGroup(hlt, true);
        createGroupsGroup(hlt, true);
        vlt->addLayout(hlt);
        hlt = new QHBoxLayout;
        createExtraFileListGroup(hlt, true);
        vlt->addLayout(hlt);
        break;
    }
    default: {
        break;
    }
    }
    //
    //
    /*mvalid = false;
    mcheckSource = false;
    mid = 0;
    msenderId = 0;
    Qt::TextInteractionFlags tiflags = Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse
            | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard;
    QSignalMapper *mpr = new QSignalMapper(this);
    connect(mpr, SIGNAL(mapped(int)), this, SLOT(selectFile(int)));
    //
    //QVBoxLayout *vlt = new QVBoxLayout(this);
      QFormLayout *flt = new QFormLayout;
        mledtTitle = new QLineEdit;
          mledtTitle->setReadOnly(ShowMode == mmode);
          mledtTitle->setMaxLength(120);
          connect(mledtTitle, SIGNAL(textChanged(QString)), this, SLOT(checkInputs()));
          minputTitle = new BInputField;
          minputTitle->addWidget(mledtTitle);
          minputTitle->setShowStyle(ShowMode == mmode ? BInputField::ShowNever : BInputField::ShowAlways);
        flt->addRow(tr("Title:", "lbl text"), minputTitle);
        mtgswgt = new TTagsWidget;
          mtgswgt->setReadOnly(ShowMode == mmode);
        flt->addRow(tr("Tags:", "lbl text"), mtgswgt);
        mcmboxType = new QComboBox;
          foreach (const TLabInfo::Type &t, TLabInfo::allTypes())
              mcmboxType->addItem(TLabInfo::typeToString(t, true), t);
          mcmboxType->setEnabled(ShowMode != mmode);
          mcmboxType->setCurrentIndex(mcmboxType->findData(TLabInfo::DesktopType));
          connect(mcmboxType, SIGNAL(currentIndexChanged(int)), this, SLOT(cmboxTypeCurrentIndexChanged(int)));
        flt->addRow(tr("Type:", "lbl text"), mcmboxType);
      vlt->addLayout(flt);
      //TODO: files
      QHBoxLayout *hlt = new QHBoxLayout;
        flt = new QFormLayout;
          mlblSender = new QLabel;
            mlblSender->setTextInteractionFlags(tiflags);
            connect(mlblSender, SIGNAL(linkActivated(QString)), this, SLOT(showSenderInfo()));
          flt->addRow(tr("Sender:", "lbl text"), mlblSender);
          mlblCreationDT = new QLabel;
            mlblCreationDT->setTextInteractionFlags(tiflags);
          flt->addRow(tr("Created:", "lbl text"), mlblCreationDT);
          mlblUpdateDT = new QLabel;
            mlblUpdateDT->setTextInteractionFlags(tiflags);
          flt->addRow(tr("Updated:", "lbl text"), mlblUpdateDT);
        hlt->addLayout(flt);
        QGroupBox *gbox = new QGroupBox(tr("Authors", "gbox title"));
          QHBoxLayout *hltw = new QHBoxLayout(gbox);
            mlstwgtAuthors = new TListWidget;
              mlstwgtAuthors->setReadOnly(ShowMode == mmode);
              mlstwgtAuthors->setButtonsVisible(ShowMode != mmode);
            hltw->addWidget(mlstwgtAuthors);
        hlt->addWidget(gbox);
      vlt->addLayout(hlt);
      hlt = new QHBoxLayout;
        gbox = new QGroupBox(tr("Comment", "gbox title"));
          hltw = new QHBoxLayout(gbox);
            mptedtComment = new QPlainTextEdit;
              mptedtComment->setReadOnly(ShowMode == mmode);
            hltw->addWidget(mptedtComment);
        hlt->addWidget(gbox);
        gbox = new QGroupBox(tr("Groups", "gbox title"));
          hltw = new QHBoxLayout(gbox);
            mlstwgtGroups = new TListWidget;
              mlstwgtGroups->setReadOnly(ShowMode == mmode);
              mlstwgtGroups->setButtonsVisible(ShowMode != mmode);
              if (ShowMode != mmode)
              {
                  QStringList groups;
                  sClient->getClabGroupsList(groups, parent);
                  mlstwgtGroups->setAvailableItems(groups);
              }
            hltw->addWidget(mlstwgtGroups);
        hlt->addWidget(gbox);
      vlt->addLayout(hlt);
      gbox = new QGroupBox(tr("Source", "gbox title"));
        gbox->setVisible(ShowMode != mmode);
        flt = new QFormLayout(gbox);
        foreach (int t, bRangeD(BeQt::LinuxOS, BeQt::WindowsOS))
        {
            mhltFile.insert(t, new QHBoxLayout);
              mledtFile.insert(t, new QLineEdit);
                connect(mledtFile.value(t), SIGNAL(textChanged(QString)), this, SLOT(checkInputs()));
              mhltFile.value(t)->addWidget(mledtFile.value(t));
              if (BeQt::LinuxOS == t)
              {
                  mbtnSearch = new QPushButton(tr("Search..."));
                    bSetMapping(mpr, mbtnSearch, SIGNAL(clicked()), t);
                  mhltFile.value(t)->addWidget(mbtnSearch);
              }
              else
              {
                  QPushButton *btn = new QPushButton(tr("Search..."));
                    bSetMapping(mpr, btn, SIGNAL(clicked()), t);
                  mhltFile.value(t)->addWidget(btn);
              }
            flt->addRow(" ", mhltFile.value(t));
        }
      vlt->addWidget(gbox);
      gbox = new QGroupBox(tr("Attached files", "gbox title"));
        hltw = new QHBoxLayout(gbox);
          flswgt = new FilesWidget(ShowMode == mmode);
            connect(flswgt, SIGNAL(getFile(QString)), this, SLOT(getFile(QString)));
          hltw->addWidget(flswgt);
      vlt->addWidget(gbox);
    //
    cmboxTypeCurrentIndexChanged(0);
    Application::setRowVisible(mlblSender, AddMode != mmode);
    Application::setRowVisible(mlblCreationDT, AddMode != mmode);
    Application::setRowVisible(mlblUpdateDT, AddMode != mmode);
    checkInputs();*/
}

/*============================== Public methods ============================*/

TAbstractCache *LabInfoWidget::cache() const
{
    return mcache;
}

TNetworkClient *LabInfoWidget::client() const
{
    return mclient;
}

QVariant LabInfoWidget::createRequestData() const
{
    if (!hasValidInput())
        return QVariant();
    switch (mmode) {
    case AddMode: {
        TAddLabRequestData data;
        /*data.setAuthors(authors());
        data.setDescritpion(mptedtDescription->toPlainText().replace(QChar::ParagraphSeparator, '\n'));
        data.setProject(msource);
        data.setTags(mtgwgt->tags());
        data.setTitle(mledtTitle->text());*/
        return data;
    }
    case EditMode: {
        TEditLabRequestData data;
        /*data.setAdminRemark(mptedtRemark->toPlainText().replace(QChar::ParagraphSeparator, '\n'));
        data.setAuthors(authors());
        data.setDescritpion(mptedtDescription->toPlainText().replace(QChar::ParagraphSeparator, '\n'));
        data.setEditProject(mcboxEditSource->isChecked());
        data.setId(mid);
        if (mcboxEditSource->isChecked())
            data.setProject(msource);
        data.setRating(quint8(msboxRating->value()));
        data.setTags(mtgwgt->tags());
        data.setTitle(mledtTitle->text());
        data.setType(mcmboxType->itemData(mcmboxType->currentIndex()).toInt());*/
        return data;
    }
    case ShowMode:
    default: {
        break;
    }
    }
    return QVariant();
    //
    /*
    info.setId(mid);
    TUserInfo u(msenderId, TUserInfo::BriefInfoContext);
    u.setLogin(msenderLogin);
    u.setRealName(msenderRealName);
    info.setSender(u);
    info.setTitle(mledtTitle->text());
    info.setTags(mtgswgt->tags());
    info.setType(mcmboxType->itemData(mcmboxType->currentIndex()).toInt());
    //TODO: size
    info.setCreationDateTime(QDateTime::fromString(mlblCreationDT->text(), DateTimeFormat));
    info.setUpdateDateTime(QDateTime::fromString(mlblUpdateDT->text(), DateTimeFormat));
    info.setAuthors(mlstwgtAuthors->items());
    info.setGroups(mlstwgtGroups->items());
    info.setComment(mptedtComment->toPlainText().replace(QChar::ParagraphSeparator, '\n'));
    info.setExtraAttachedFileNames(flswgt->files());
      */
}

bool LabInfoWidget::hasValidInput() const
{
    return mvalid;
}

LabInfoWidget::Mode LabInfoWidget::mode() const
{
    return mmode;
}

LabModel *LabInfoWidget::model() const
{
    return mmodel;
}

void LabInfoWidget::restoreState(const QByteArray &state)
{
    QVariantMap m = BeQt::deserialize(state).toMap();
    mlstwgtAuthors->setAvailableAuthors(m.value("authors").value<TAuthorInfoList>());
    mtgwgt->setAvailableTags(m.value("tags").toStringList());
    mledtTitle->setText(m.value("title").toString());
}

QByteArray LabInfoWidget::saveState() const
{
    QVariantMap m;
    TAuthorInfoList list = mlstwgtAuthors->availableAuthors();
    list << mlstwgtAuthors->authors();
    bRemoveDuplicates(list);
    m.insert("authors", list);
    m.insert("tags", mtgwgt->availableTags());
    m.insert("title", mledtTitle->text());
    return BeQt::serialize(m);
}

void LabInfoWidget::setCache(TAbstractCache *cache)
{
    mcache = cache;
}

void LabInfoWidget::setClient(TNetworkClient *client)
{
    mclient = client;
}

bool LabInfoWidget::setLab(quint64 labId)
{
    if (AddMode == mmode || !mmodel)
        return false;
    TLabInfo info = mmodel->labInfo(labId);
    mid = info.id();
    msenderId = info.senderId();
    /*resetFile(info.mainSourceFile().fileName(), info.sourceSize());
    mtgwgt->setTags(info.tags());
    mledtTitle->setText(info.title());
    mlblSender->setText("<a href=dummy>" + info.senderLogin() + "</a>");
    mlblSender->setToolTip(tr("Click the link to see info about the sender", "lbl toolTip"));
    msboxRating->setValue(info.rating());
    mcmboxType->setCurrentIndex(mcmboxType->findData(int(info.type())));
    mlblCreationDT->setText(info.creationDateTime().toTimeSpec(Qt::LocalTime).toString(DateTimeFormat));
    mlblUpdateDT->setText(info.lastModificationDateTime().toTimeSpec(Qt::LocalTime).toString(DateTimeFormat));
    mlstwgtAuthors->setAuthors(info.authors());
    mptedtDescription->setPlainText(info.description());
    mptedtRemark->setPlainText(info.adminRemark());
    QString s = BeQt::fileSizeToString(info.previewSize(), BeQt::KilobytesFormat);
    if(mtbtnShowPreview)
        mtbtnShowPreview->setToolTip(tr("Show sample preview", "tbtn toolTip") + " (" + s + ")");
    checkInputs();*/
    return info.isValid();
    //
    /*
    mid = info.id();
    msenderId = info.sender().id();
    msenderLogin = info.sender().login();
    msenderRealName = info.sender().realName();
    mledtTitle->setText(info.title());
    if (!mledtTitle->hasAcceptableInput())
        mledtTitle->clear();
    mtgswgt->setTags(info.tags());
    mcmboxType->setCurrentIndex(mcmboxType->findData(info.type()));
    if (mcmboxType->currentIndex() < 0)
        mcmboxType->setCurrentIndex(0);
    if (!msenderLogin.isEmpty())
    {
        QString s = "<a href=x>" + msenderLogin + "</a>";
        s += !msenderRealName.isEmpty() ? (" (" + msenderRealName + ")") : QString();
        mlblSender->setText(s);
        mlblSender->setToolTip(tr("Click the link to see info about the sender", "lbl toolTip"));
    }
    else
    {
        mlblSender->clear();
        mlblSender->setToolTip("");
    }
    if (info.creationDateTime().isValid())
        mlblCreationDT->setText(info.creationDateTime(Qt::LocalTime).toString(DateTimeFormat));
    else
        mlblCreationDT->clear();
    if (info.updateDateTime().isValid())
        mlblUpdateDT->setText(info.updateDateTime(Qt::LocalTime).toString(DateTimeFormat));
    else
        mlblUpdateDT->clear();
    mlstwgtAuthors->setItems(info.authors());
    mlstwgtGroups->setItems(info.groups());
    mptedtComment->setPlainText(info.comment());
    flswgt->addFiles(info.extraAttachedFileNames());
    setFocus();
    checkInputs();
      */
}

void LabInfoWidget::setModel(LabModel *model)
{
    mmodel = model;
}

QString LabInfoWidget::title() const
{
    return mledtTitle->text();
}

/*============================== Private methods ===========================*/

void LabInfoWidget::createAuthorsGroup(QHBoxLayout *hlt, bool readOnly)
{
    //
}

void LabInfoWidget::createDescriptionGroup(QHBoxLayout *hlt, bool readOnly)
{
    //
}

void LabInfoWidget::createExtraFileListGroup(QHBoxLayout *hlt, bool readOnly)
{
    //
}

void LabInfoWidget::createExtraGroup(QHBoxLayout *hlt, bool readOnly)
{
    //
}

void LabInfoWidget::createGroupsGroup(QHBoxLayout *hlt, bool readOnly)
{
    //
}

void LabInfoWidget::createLabDataListGroup(QHBoxLayout *hlt, bool readOnly)
{
    //
}

void LabInfoWidget::createMainGroup(QVBoxLayout *vlt, bool readOnly)
{
    //
}

void LabInfoWidget::createTagsField(QFormLayout *flt, bool readOnly)
{
    //
}

void LabInfoWidget::createTitleField(QFormLayout *flt, bool readOnly)
{
    //
}

/*============================== Private slots =============================*/

void LabInfoWidget::checkInputs()
{
    /*minputTitle->setValid(!mledtTitle->text().isEmpty() && mledtTitle->hasAcceptableInput());
    bool src = true;
    if (mcheckSource)
    {
        switch (mcmboxType->currentIndex())
        {
        case 0:
            src = linuxProject().isValid() || macProject().isValid() || winProject().isValid();
            break;
        case 1:
            src = webProject().isValid();
            break;
        case 2:
            src = !url().isEmpty();
            break;
        default:
            break;
        }
    }
    bool v = info().isValid() && (!mcheckSource || src);
    if (v == mvalid)
        return;
    mvalid = v;
    emit validityChanged(v);*/
    //
    //
    /*
    bool idValid = (AddMode == mmode) || mid;
    bool titleValid = mledtTitle->hasAcceptableInput();
    bool sourceValid = (!mcboxEditSource || !mcboxEditSource->isChecked() || msource.isValid())
            && mledtFileName->hasAcceptableInput();
    minputTitle->setValid(titleValid);
    minputFileName->setValid(sourceValid);
    mtbtnSetupFromCurrentDocument->setEnabled(meditor && meditor->documentAvailable());
    bool v = idValid && titleValid && sourceValid;
    if (v == mvalid)
        return;
    mvalid = v;
    emit inputValidityChanged(mvalid);
      */
}

void LabInfoWidget::cmboxTypeCurrentIndexChanged(int index)
{
    //
}

void LabInfoWidget::showExtraFile(const QString &fileName)
{
    if (!mid || fileName.isEmpty())
        return;
    tSmp->getExtraFile(mid, fileName, this);
}

void LabInfoWidget::showSenderInfo()
{
    if (!msenderId)
        return;
    tSmp->showUserInfo(msenderId);
}
