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

#include "extrafilelistwidget.h"

#include "application.h"
#include "extrafilewidget.h"

#include <TBinaryFile>
#include <TBinaryFileList>
#include <TFileInfo>
#include <TFileInfoList>

#include <BeQt>
#include <BGuiTools>

#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QLayoutItem>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QSignalMapper>
#include <QString>
#include <QStringList>
#include <QToolButton>
#include <QVariant>
#include <QVariantMap>
#include <QWidget>

/*============================================================================
================================ ExtraFileListWidget =========================
============================================================================*/

/*============================== Public constructors =======================*/

ExtraFileListWidget::ExtraFileListWidget(QWidget *parent) :
    QWidget(parent)
{
    mlabId = 0;
    init();
}

ExtraFileListWidget::ExtraFileListWidget(quint64 labId, QWidget *parent) :
    QWidget(parent)
{
    mlabId = labId;
    init();
}

ExtraFileListWidget::ExtraFileListWidget(quint64 labId, bool readOnly, QWidget *parent) :
    QWidget(parent)
{
    mlabId = labId;
    init();
    setReadOnly(readOnly);
}

/*============================== Public methods ============================*/

QStringList ExtraFileListWidget::deletedFileList() const
{
    QStringList list;
    list << mdeletedFileNames;
    foreach (ExtraFileWidget *efw, mwidgets.values()) {
        if (!efw->isDeleted() || !mfileNames.contains(efw->fileName()))
            continue;
        list << efw->fileName();
    }
    return list;
}

bool ExtraFileListWidget::isReadOnly() const
{
    return mreadOnly;
}

TBinaryFileList ExtraFileListWidget::newFileList() const
{
    TBinaryFileList list;
    foreach (ExtraFileWidget *efw, mwidgets.values()) {
        if (efw->isDeleted() || mfileNames.contains(efw->fileName()))
            continue;
        TBinaryFile file(efw->fileName());
        if (!file.isValid())
            continue;
        list << file;
    }
    return list;
}

void ExtraFileListWidget::restoreState(const QByteArray &state)
{
    QVariantMap m = BeQt::deserialize(state).toMap();
    QString dir = m.value("last_dir").toString();
    if (!dir.isEmpty())
        mlastDir = dir;
}

QByteArray ExtraFileListWidget::saveState() const
{
    QVariantMap m;
    m.insert("last_dir", mlastDir);
    return BeQt::serialize(m);
}

void ExtraFileListWidget::setFileInfos(const TFileInfoList &list)
{
    foreach (ExtraFileWidget *efw, mwidgets.values())
        delete efw;
    mwidgets.clear();
    mdeletedFileNames.clear();
    mfileNames.clear();
    foreach (const TFileInfo &fi, list) {
        mfileNames << fi.fileName();
        addFile(fi.fileName(), fi.fileSize());
    }
}

void ExtraFileListWidget::setReadOnly(bool ro)
{
    mreadOnly = ro;
    mtbtnAdd->setVisible(!ro);
    foreach (ExtraFileWidget *efw, mwidgets.values())
        efw->setReadOnly(ro);
}

/*============================== Private methods ===========================*/

void ExtraFileListWidget::addFile(const QString &fileName, int size)
{
    if (fileName.isEmpty())
        return;
    QString fn = QFileInfo(fileName).fileName();
    foreach (ExtraFileWidget *efw, mwidgets.values()) {
        if (fn == QFileInfo(efw->fileName()).fileName()) {
            QMessageBox msg(this);
            msg.setWindowTitle(tr("Duplicate file", "msgbox windowTitle"));
            msg.setIcon(QMessageBox::Information);
            msg.setText(tr("This file is already in list:", "msgbox text"));
            msg.setInformativeText(fileName);
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
            return;
        }
    }
    ExtraFileWidget *efw = (size > 0) ? new ExtraFileWidget(mflt, mlabId, fileName, size, mreadOnly) :
                                        new ExtraFileWidget(mflt, fileName);
    bSetMapping(mdeleteMapper, efw, SIGNAL(deletePermanently()), (QWidget *) efw);
    mwidgets.insert(efw, efw);
}

void ExtraFileListWidget::init()
{
    mreadOnly = false;
    mlastDir = QDir::homePath();
    mdeleteMapper = new QSignalMapper(this);
    connect(mdeleteMapper, SIGNAL(mapped(QWidget*)), this, SLOT(deletePermanently(QWidget*)));
    //
    mflt = new QFormLayout(this);
    mtbtnAdd = new QToolButton;
    mtbtnAdd->setIcon(Application::icon("edit_add"));
    mtbtnAdd->setToolTip(tr("Add files...", "tbtn toolTip"));
    connect(mtbtnAdd, SIGNAL(clicked()), this, SLOT(addFiles()));
    QWidget *placeholder = new QWidget;
    placeholder->setMinimumWidth(250);
    mflt->addRow(placeholder, mtbtnAdd);
}

/*============================== Private slots =============================*/

void ExtraFileListWidget::addFiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Select files", "fdlg caption"), mlastDir);
    mlastDir = QFileInfo(fileNames.first()).path();
    if (fileNames.isEmpty())
        return;
    foreach (const QString &fn, fileNames)
        addFile(fn);
}

void ExtraFileListWidget::deletePermanently(QWidget *widget)
{
    mdeletedFileNames << qobject_cast<ExtraFileWidget *>(widget)->fileName();
    mwidgets.remove(widget);
    delete widget;
}
