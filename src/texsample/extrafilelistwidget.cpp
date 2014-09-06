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

#include <TBinaryFileList>
#include <TFileInfoList>

#include <QStringList>
#include <QWidget>

/*============================================================================
================================ ExtraFileListWidget =========================
============================================================================*/

/*FilesWidget::Line::Line()
{
    hlt = new QHBoxLayout;
    lbl = new QLabel;
    hlt->addWidget(lbl);
    hlt->addStretch();
    tbtn = 0;
    deleted = false;
}*/

/*============================== Public constructors =======================*/

ExtraFileListWidget::ExtraFileListWidget(QWidget *parent) :
    QWidget(parent)
{
    init();
}

ExtraFileListWidget::ExtraFileListWidget(bool readOnly, QWidget *parent) :
    QWidget(parent)
{
    init();
    setReadOnly(readOnly);
}

/*============================== Public methods ============================*/

QStringList ExtraFileListWidget::deletedFileList() const
{
    //
}

bool ExtraFileListWidget::isReadOnly() const
{
    //
}

TBinaryFileList ExtraFileListWidget::newFileList() const
{
    //
}

void ExtraFileListWidget::setFileInfos(const TFileInfoList &list)
{
    //
}

void ExtraFileListWidget::setReadOnly(bool ro)
{
    //
}

/*void FilesWidget::addFile(const QString &fn)
{
    if (fn.isEmpty() || lines.contains(fn))
        return;
    deleteLastDeleted();
    Line l;
    l.lbl->setText(labelText(fn));
    l.lbl->setToolTip(fn);
    connect(l.lbl, SIGNAL(linkActivated(QString)), this, SIGNAL(getFile(QString)));
    if (!ReadOnly)
    {
        l.tbtn = new QToolButton;
        l.tbtn->setIcon(Application::icon("editdelete"));
        l.tbtn->setToolTip(deleteToolTip);
        l.hlt->addWidget(l.tbtn);
        bSetMapping(mapper, l.tbtn, SIGNAL(clicked()), fn);
    }
    lines.insert(fn, l);
    vlt->insertLayout(vlt->count() - 1, l.hlt);
}

void FilesWidget::addFiles(const QStringList &list)
{
    foreach (const QString &fn, list)
        addFile(fn);
}

QStringList FilesWidget::files() const
{
    QStringList list;
    foreach (const QString &fn, lines.keys())
        if (!lines.value(fn).deleted)
            list << fn;
    return list;
}

bool FilesWidget::isReadOnly() const
{
    return ReadOnly;
}*/

/*============================== Static private constants ==================*/

/*const BTranslation FilesWidget::deleteToolTip = BTranslation::translate("FilesWidget", "Remove file", "tbtn toolTip");
const BTranslation FilesWidget::undeleteToolTip = BTranslation::translate("FilesWidget", "Cancel removal",
                                                                          "tbtn toolTip");*/

/*============================== Static private methods ====================*/

/*QString FilesWidget::labelText(const QString &fileName)
{
    if (fileName.isEmpty())
        return "";
    return "<a href=\"" + fileName + "\">" + QFileInfo(fileName).fileName() + "</a>";
}*/

/*============================== Private methods ===========================*/

void ExtraFileListWidget::init()
{
    /*hasDeleted = false;
    lastDir = QDir::homePath();
    vlt = new QVBoxLayout(this);
    mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(QString)), this, SLOT(mapped(QString)));
    if (!ReadOnly)
    {
        QHBoxLayout *hlt = new QHBoxLayout;
          hlt->addStretch();
          tbtn = new QToolButton;
            tbtn->setIcon(Application::icon("edit_add"));
            tbtn->setToolTip(tr("Add file", "tbtn toolTip"));
            connect(tbtn, SIGNAL(clicked()), this, SLOT(addFile()));
          hlt->addWidget(tbtn);
        vlt->addLayout(hlt);
    }*/
}

/*(void FilesWidget::deleteLastDeleted()
{
    if (!hasDeleted)
        return;
    foreach (const QString &ffn, lines.keys())
    {
        if (lines.value(ffn).deleted)
        {
            Line l = lines.take(ffn);
            delete l.lbl;
            delete l.tbtn;
            delete l.hlt;
            break;
        }
    }
    hasDeleted = false;
}*/

/*============================== Private slots =============================*/

/*void FilesWidget::mapped(const QString &fn)
{
    Line &l = lines[fn];
    if (l.deleted)
    {
        l.tbtn->setIcon(Application::icon("editdelete"));
        l.tbtn->setToolTip(deleteToolTip);
        l.lbl->setText(labelText(fn));
        hasDeleted = false;
    }
    else
    {
        deleteLastDeleted();
        l.tbtn->setIcon(Application::icon("reload"));
        l.tbtn->setToolTip(undeleteToolTip);
        l.lbl->setText(labelText(fn) + " [" + tr("deleted", "lbl text") + "]");
        hasDeleted = true;
    }
    l.deleted = !l.deleted;
}

void FilesWidget::addFile()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files", "fdlg caption"), lastDir);
    if (files.isEmpty())
        return;
    lastDir = QFileInfo(files.first()).path();
    addFiles(files);
}*/

/*
void LabInfoWidget::getFile(const QString &fileName)
{
    if (fileName.isEmpty())
        return;
    if (QFileInfo(fileName).isAbsolute())
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    else
        sClient->getExtraAttachedFile(mid, fileName, this);
}
  */


/*
  gbox = new QGroupBox(tr("Attached files", "gbox title"));
    hltw = new QHBoxLayout(gbox);
      flswgt = new FilesWidget(ShowMode == mmode);
        connect(flswgt, SIGNAL(getFile(QString)), this, SLOT(getFile(QString)));
      hltw->addWidget(flswgt);
  vlt->addWidget(gbox);*/
