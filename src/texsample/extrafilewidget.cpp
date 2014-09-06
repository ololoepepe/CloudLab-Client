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

#include "extrafilewidget.h"

#include "application.h"
#include "texsamplecore.h"

#include <BeQt>

#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QString>
#include <QToolButton>
#include <QWidget>

/*============================================================================
================================ ExtraFileWidget =============================
============================================================================*/

/*============================== Public constructors =======================*/

ExtraFileWidget::ExtraFileWidget(QFormLayout *flt, const QString &fileName, QWidget *parent) :
    QWidget(parent)
{
    mflt = flt;
    mlabId = 0;
    mfileName = fileName;
    msize = int(QFileInfo(fileName).size());
    mreadOnly = false;
    init();
}

ExtraFileWidget::ExtraFileWidget(QFormLayout *flt, quint64 labId, const QString &fileName, int size, QWidget *parent) :
    QWidget(parent)
{
    mflt = flt;
    mlabId = labId;
    mfileName = fileName;
    msize = size;
    mreadOnly = false;
    init();
}

ExtraFileWidget::ExtraFileWidget(QFormLayout *flt, quint64 labId, const QString &fileName, int size, bool readOnly,
                                 QWidget *parent) :
    QWidget(parent)
{
    mflt = flt;
    mlabId = labId;
    mfileName = fileName;
    msize = size;
    mreadOnly = false;
    init();
    setReadOnly(readOnly);
}

ExtraFileWidget::~ExtraFileWidget()
{
    delete mlbl;
    delete mtbtnDelete;
    delete mtbtnToggle;
    delete mhlt;
}

/*============================== Public methods ============================*/

QString ExtraFileWidget::fileName() const
{
    return mfileName;
}

bool ExtraFileWidget::isDeleted() const
{
    return mdeleted;
}

bool ExtraFileWidget::isReadOnly() const
{
    return mreadOnly;
}

void ExtraFileWidget::setReadOnly(bool ro)
{
    mreadOnly = ro;
    mtbtnDelete->setVisible(!ro && mdeleted);
    mtbtnToggle->setVisible(!ro);
}

/*============================== Private methods ===========================*/

void ExtraFileWidget::init()
{
    mdeleted = true; //NOTE: Set to true, so we can now call toggleDeleted
    //
    mlbl = new QLabel;
    mlbl->setToolTip(mfileName);
    connect(mlbl, SIGNAL(linkActivated(QString)), this, SLOT(showFile()));
    mhlt = new QHBoxLayout;
    mtbtnToggle = new QToolButton;
    connect(mtbtnToggle, SIGNAL(clicked()), this, SLOT(toggleDeleted()));
    mhlt->addWidget(mtbtnToggle);
    mtbtnDelete = new QToolButton;
    mtbtnDelete->setIcon(Application::icon("editdelete"));
    mtbtnDelete->setToolTip(tr("Delete permanently", "tbtn toolTip"));
    connect(mtbtnDelete, SIGNAL(clicked()), this, SIGNAL(deletePermanently()));
    mhlt->addWidget(mtbtnDelete);
    mtbtnDelete->setVisible(false);
    mflt->insertRow(mflt->rowCount() - 1, mlbl, mhlt);
    toggleDeleted();
}

/*============================== Private slots =============================*/

void ExtraFileWidget::showFile()
{
    if (mfileName.isEmpty())
        return;
    if (QFileInfo(mfileName).isAbsolute())
        bApp->openLocalFile(mfileName);
    else if (mlabId)
        tSmp->getExtraFile(mlabId, mfileName, parentWidget());
}

void ExtraFileWidget::toggleDeleted()
{
    mdeleted = !mdeleted;
    QString s = "<a href=dummy>" + QFileInfo(mfileName).fileName() + "</a>";
    s += " " + BeQt::fileSizeToString(msize, BeQt::KilobytesFormat);
    if (mdeleted) {
        mtbtnToggle->setIcon(Application::icon("reload"));
        mtbtnToggle->setToolTip(tr("Cancel removal", "tbtn toolTip"));
        mlbl->setText(s + " [" + tr("deleted", "lbl text") + "]");
        mtbtnDelete->setVisible(!mreadOnly);
    } else {
        mtbtnToggle->setIcon(Application::icon("editdelete"));
        mtbtnToggle->setToolTip(tr("Remove file", "tbtn toolTip"));
        mlbl->setText(s);
        mtbtnDelete->setVisible(false);
    }
}
