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

#include "labdatalistwidget.h"

#include <TLabDataList>

#include <QWidget>

/*============================================================================
================================ LabDataListWidget ===========================
============================================================================*/

/*============================== Public constructors =======================*/

LabDataListWidget::LabDataListWidget(QWidget *parent) :
    QWidget(parent)
{
    //
}

/*============================== Public methods ============================*/

const TLabDataList &LabDataListWidget::dataList() const
{
    static const TLabDataList Default; //TODO: it's a dummy
    return Default;
}

bool LabDataListWidget::hasValidInput() const
{
    //
}

/*
TLabProject LabInfoWidget::webProject() const
{
    TLabProject p((mcmboxType->currentIndex() == 1) ? mledtFile.value(BeQt::LinuxOS)->text() : QString());
    if (p.isExecutable())
        p.clear();
    return p;
}

TLabProject LabInfoWidget::linuxProject() const
{
    TLabProject p((mcmboxType->currentIndex() == 0) ? mledtFile.value(BeQt::LinuxOS)->text() : QString());
    if (!p.isExecutable())
        p.clear();
    return p;
}

TLabProject LabInfoWidget::macProject() const
{
    TLabProject p((mcmboxType->currentIndex() == 0) ? mledtFile.value(BeQt::MacOS)->text() : QString());
    if (!p.isExecutable())
        p.clear();
    return p;
}

TLabProject LabInfoWidget::winProject() const
{
    TLabProject p((mcmboxType->currentIndex() == 0) ? mledtFile.value(BeQt::WindowsOS)->text() : QString());
    if (!p.isExecutable())
        p.clear();
    return p;
}

QString LabInfoWidget::url() const
{
    return (mcmboxType->currentIndex() == 2) ? mledtFile.value(BeQt::LinuxOS)->text() : QString();
}
  */

/*
void LabInfoWidget::selectFile(int id)
{
    QLineEdit *ledt = mledtFile.value(id);
    if (!ledt)
        return;
    QString dir = ledt->text();
    if (dir.isEmpty())
        dir = QDir::homePath();
    QString fn = QFileDialog::getOpenFileName(this, tr("Select file", "getOpenFileName"), dir);
    if (fn.isEmpty())
        return;
    ledt->setText(fn);
}
  */


/*
void LabInfoWidget::cmboxTypeCurrentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        Application::setRowVisible(mhltFile.value(BeQt::LinuxOS), true);
        Application::setRowVisible(mhltFile.value(BeQt::MacOS), true);
        Application::setRowVisible(mhltFile.value(BeQt::WindowsOS), true);
        Application::labelForField<QLabel>(mhltFile.value(BeQt::LinuxOS))->setText(tr("Main file (Linux):",
                                                                                      "lbl text"));
        Application::labelForField<QLabel>(mhltFile.value(BeQt::MacOS))->setText(tr("Main file (Mac OS):", "lbl text"));
        Application::labelForField<QLabel>(mhltFile.value(BeQt::WindowsOS))->setText(tr("Main file (Windows):",
                                                                                        "lbl text"));
        mbtnSearch->setVisible(true);
        break;
    case 1:
        Application::setRowVisible(mhltFile.value(BeQt::LinuxOS), true);
        Application::setRowVisible(mhltFile.value(BeQt::MacOS), false);
        Application::setRowVisible(mhltFile.value(BeQt::WindowsOS), false);
        Application::labelForField<QLabel>(mhltFile.value(BeQt::LinuxOS))->setText(tr("Main file:", "lbl text"));
        mbtnSearch->setVisible(true);
        break;
    case 2:
        Application::setRowVisible(mhltFile.value(BeQt::LinuxOS), true);
        Application::setRowVisible(mhltFile.value(BeQt::MacOS), false);
        Application::setRowVisible(mhltFile.value(BeQt::WindowsOS), false);
        Application::labelForField<QLabel>(mhltFile.value(BeQt::LinuxOS))->setText(tr("URL:", "lbl text"));
        mbtnSearch->setVisible(false);
        break;
    default:
        break;
    }
    checkInputs();
}
  */

/*
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
  vlt->addWidget(gbox);*/

/*
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
  */
