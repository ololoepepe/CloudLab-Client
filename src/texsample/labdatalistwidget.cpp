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
