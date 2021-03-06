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

#ifndef TEXSAMPLESETTINGSTAB_H
#define TEXSAMPLESETTINGSTAB_H

class BLoginWidget;

class QCheckBox;
class QIcon;
class QString;

#include <BAbstractSettingsTab>

#include <QObject>
#include <QStringList>

/*============================================================================
================================ TexsampleSettingsTab ========================
============================================================================*/

class TexsampleSettingsTab : public BAbstractSettingsTab
{
    Q_OBJECT
private:
    BLoginWidget *lgnwgt;
    QCheckBox *cboxConnectOnStartup;
    QCheckBox *cboxCaching;
public:
    explicit TexsampleSettingsTab();
public:
    QIcon icon() const;
    QString id() const;
    bool restoreDefault();
    bool saveSettings();
    QString title() const;
private slots:
    void clearCache();
private:
    Q_DISABLE_COPY(TexsampleSettingsTab)
};

#endif // TEXSAMPLESETTINGSTAB_H
