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
#include "extrafilelistwidget.h"
#include "labdatalistwidget.h"
#include "labmodel.h"
#include "settings.h"
#include "texsamplecore.h"

#include <TAddLabRequestData>
#include <TAuthorInfo>
#include <TAuthorInfoList>
#include <TAuthorListWidget>
#include <TEditLabRequestData>
#include <TeXSample>
#include <TFileInfoList>
#include <TGroupInfoList>
#include <TGroupListWidget>
#include <TIdList>
#include <TLabDataInfo>
#include <TLabDataInfoList>
#include <TLabDataList>
#include <TLabInfo>
#include <TLabType>
#include <TNetworkClient>
#include <TTagWidget>
#include <TUserInfo>

#include <BeQt>
#include <BInputField>

#include <QByteArray>
#include <QChar>
#include <QCheckBox>
#include <QComboBox>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIODevice>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPlainTextEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QString>
#include <QStringList>
#include <QValidator>
#include <QVariant>
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
    mldlwgt = 0;
    meflwgt = 0;
    //
    QVBoxLayout *vlt = new QVBoxLayout(this);
    switch (mmode) {
    case AddMode: {
        createMainGroup(vlt);
        QHBoxLayout *hlt = new QHBoxLayout;
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
        createExtraGroup(hlt);
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
        data.setAuthors(mlstwgtAuthors->authors());
        data.setDataList(mldlwgt->dataList());
        data.setDescritpion(mptedtDescription->toPlainText().replace(QChar::ParagraphSeparator, '\n'));
        data.setExtraFiles(meflwgt->newFileList());
        data.setGroups(mlstwgtGroups->groupIds());
        data.setTags(mtgwgt->tags());
        data.setTitle(mledtTitle->text());
        return data;
    }
    case EditMode: {
        TEditLabRequestData data;
        data.setId(mid);
        data.setAuthors(mlstwgtAuthors->authors());
        data.setEditData(mcboxEditData->isChecked());
        if (mcboxEditData->isChecked())
            data.setDataList(mldlwgt->dataList());
        data.setDeletedExtraFiles(meflwgt->deletedFileList());
        data.setDescritpion(mptedtDescription->toPlainText().replace(QChar::ParagraphSeparator, '\n'));
        data.setGroups(mlstwgtGroups->groupIds());
        data.setNewExtraFiles(meflwgt->newFileList());
        data.setTags(mtgwgt->tags());
        data.setTitle(mledtTitle->text());
        return data;
    }
    case ShowMode:
    default: {
        break;
    }
    }
    return QVariant();
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
    mldlwgt->restoreState(m.value("lab_data_list_widget_state").toByteArray());
    meflwgt->restoreState(m.value("extra_file_list_widget_state").toByteArray());
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
    m.insert("lab_data_list_widget_state", mldlwgt->saveState());
    m.insert("extra_file_list_widget_state", meflwgt->saveState());
    return BeQt::serialize(m);
}

void LabInfoWidget::setCache(TAbstractCache *cache)
{
    mcache = cache;
}

void LabInfoWidget::setClient(TNetworkClient *client)
{
    if (mclient)
        disconnect(mclient, SIGNAL(authorizedChanged(bool)), this, SLOT(clientAuthorizedChanged(bool)));
    mclient = client;
    if (client)
        connect(client, SIGNAL(authorizedChanged(bool)), this, SLOT(clientAuthorizedChanged(bool)));
    clientAuthorizedChanged(client && client->isAuthorized());
}

bool LabInfoWidget::setLab(quint64 labId)
{
    if (AddMode == mmode || !mmodel)
        return false;
    TLabInfo info = mmodel->labInfo(labId);
    mid = info.id();
    msenderId = info.senderId();
    mledtTitle->setText(info.title());
    mlblSender->setText("<a href=dummy>" + info.senderLogin() + "</a>");
    mlblSender->setToolTip(tr("Click the link to see info about the sender", "lbl toolTip"));
    mtgwgt->setTags(info.tags());
    int type = !info.dataInfos().isEmpty() ? int(info.dataInfos().first().type()) : TLabType::NoType;
    mcmboxType->setCurrentIndex(mcmboxType->findData(type));
    int size = 0;
    foreach (const TLabDataInfo &ldi, info.dataInfos()) {
        if (TLabType::DesktopApplication != ldi.type() || ldi.os() == BeQt::osType()) {
            size = ldi.size();
            break;
        }
    }
    mlblSize->setText(size ? BeQt::fileSizeToString(size, BeQt::MegabytesFormat) : tr("Unknown", "lbl text"));
    mlblCreationDT->setText(info.creationDateTime().toTimeSpec(Qt::LocalTime).toString(DateTimeFormat));
    mlblUpdateDT->setText(info.lastModificationDateTime().toTimeSpec(Qt::LocalTime).toString(DateTimeFormat));
    mlstwgtAuthors->setAuthors(info.authors());
    mptedtDescription->setPlainText(info.description());
    mlstwgtGroups->setGroups(info.groups());
    meflwgt->setLabId(info.id());
    meflwgt->setFileInfos(info.extraFiles());
    checkInputs();
    return info.isValid();
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
    QGroupBox *gbox = new QGroupBox(tr("Authors:", "gbox title"));
      QVBoxLayout *vlt = new QVBoxLayout(gbox);
        mlstwgtAuthors = new TAuthorListWidget;
          mlstwgtAuthors->setReadOnly(readOnly);
          mlstwgtAuthors->setButtonsVisible(!readOnly);
        vlt->addWidget(mlstwgtAuthors);
    hlt->addWidget(gbox);
}

void LabInfoWidget::createDescriptionGroup(QHBoxLayout *hlt, bool readOnly)
{
    QGroupBox *gbox = new QGroupBox(tr("Description:", "gbox title"));
      QVBoxLayout *vlt = new QVBoxLayout(gbox);
        mptedtDescription = new QPlainTextEdit;
          mptedtDescription->setReadOnly(readOnly);
        vlt->addWidget(mptedtDescription);
    hlt->addWidget(gbox);
}

void LabInfoWidget::createEditDataField(QFormLayout *flt)
{
    mcboxEditData = new QCheckBox;
      connect(mcboxEditData, SIGNAL(toggled(bool)), this, SLOT(checkInputs()));
    flt->addRow(tr("Edit data:", "lbl text"), mcboxEditData);
}

void LabInfoWidget::createExtraFileListGroup(QHBoxLayout *hlt, bool readOnly)
{
    QGroupBox *gbox = new QGroupBox(tr("Extra files:", "gbox title"));
      QVBoxLayout *vlt = new QVBoxLayout(gbox);
        meflwgt = new ExtraFileListWidget(readOnly);
        vlt->addWidget(meflwgt);
    hlt->addWidget(gbox);
}

void LabInfoWidget::createExtraGroup(QHBoxLayout *hlt)
{
    QFormLayout *flt = new QFormLayout;
      mlblSender = new QLabel;
        mlblSender->setTextInteractionFlags(TextInteractionFlags);
        connect(mlblSender, SIGNAL(linkActivated(QString)), this, SLOT(showSenderInfo()));
      flt->addRow(tr("Sender:", "lbl text"), mlblSender);
      mcmboxType = new QComboBox;
        static const QList<TLabType> LabTypes = QList<TLabType>() << TLabType::DesktopApplication
                                                                  << TLabType::WebApplication << TLabType::Url;
        foreach (const TLabType &t, LabTypes)
            mcmboxType->addItem(t.toString(), int(t));
        mcmboxType->setEnabled(false);
      flt->addRow(tr("Type:", "lbl text"), mcmboxType);
      mlblSize = new QLabel;
      flt->addRow(tr("Size:", "lbl text"), mlblSize);
      mlblCreationDT = new QLabel;
        mlblCreationDT->setTextInteractionFlags(TextInteractionFlags);
      flt->addRow(tr("Creation date:", "lbl text"), mlblCreationDT);
      mlblUpdateDT = new QLabel;
        mlblUpdateDT->setTextInteractionFlags(TextInteractionFlags);
      flt->addRow(tr("Modified:", "lbl text"), mlblUpdateDT);
    hlt->addLayout(flt);
}

void LabInfoWidget::createGroupsGroup(QHBoxLayout *hlt, bool readOnly)
{
    QGroupBox *gbox = new QGroupBox(tr("Groups:", "gbox title"));
      QVBoxLayout *vlt = new QVBoxLayout(gbox);
        mlstwgtGroups = new TGroupListWidget;
          mlstwgtGroups->setReadOnly(readOnly);
          mlstwgtGroups->setButtonsVisible(!readOnly);
        vlt->addWidget(mlstwgtGroups);
    hlt->addWidget(gbox);
}

void LabInfoWidget::createLabDataListGroup(QHBoxLayout *hlt)
{
    QGroupBox *gbox = new QGroupBox(tr("Data:", "gbox title"));
      QVBoxLayout *vlt = new QVBoxLayout(gbox);
        mldlwgt = new LabDataListWidget;
          connect(mldlwgt, SIGNAL(inputValidityChanged(bool)), this, SLOT(checkInputs()));
        vlt->addWidget(mldlwgt);
    hlt->addWidget(gbox);
    if (mcboxEditData) {
        gbox->setVisible(false);
        connect(mcboxEditData, SIGNAL(toggled(bool)), gbox, SLOT(setVisible(bool)));
    }
}

void LabInfoWidget::createMainGroup(QVBoxLayout *vlt, bool readOnly)
{
    QFormLayout *flt = new QFormLayout;
      createTitleField(flt, readOnly);
      if (EditMode == mmode)
          createEditDataField(flt);
      createTagsField(flt, readOnly);
    vlt->addLayout(flt);
}

void LabInfoWidget::createTagsField(QFormLayout *flt, bool readOnly)
{
    mtgwgt = new TTagWidget;
      mtgwgt->setReadOnly(readOnly);
      mtgwgt->setButtonsVisible(!readOnly);
    flt->addRow(tr("Tags:", "lbl text"), mtgwgt);
}

void LabInfoWidget::createTitleField(QFormLayout *flt, bool readOnly)
{
    mledtTitle = new QLineEdit;
      mledtTitle->setReadOnly(readOnly);
      QRegExp rx(".{1," + QString::number(Texsample::MaximumLabTitleLength) + "}");
      mledtTitle->setValidator(new QRegExpValidator(rx, this));
      connect(mledtTitle, SIGNAL(textChanged(QString)), this, SLOT(checkInputs()));
      minputTitle = new BInputField(readOnly ? BInputField::ShowNever : BInputField::ShowAlways);
      minputTitle->addWidget(mledtTitle);
    flt->addRow(tr("Title:", "lbl text"), minputTitle);
}

/*============================== Private slots =============================*/

void LabInfoWidget::checkInputs()
{
    bool idValid = (AddMode == mmode) || mid;
    bool titleValid = mledtTitle->hasAcceptableInput();
    bool dataValid = (ShowMode == mmode)
            || ((mcboxEditData && !mcboxEditData->isChecked()) || !mldlwgt->dataList().isEmpty());
    minputTitle->setValid(titleValid);
    bool v = idValid && titleValid && dataValid;
    if (v == mvalid)
        return;
    mvalid = v;
    emit inputValidityChanged(mvalid);
}

void LabInfoWidget::clientAuthorizedChanged(bool)
{
    if (mlstwgtGroups)
        mlstwgtGroups->setAvailableGroups(mclient->userInfo().availableGroups());
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
