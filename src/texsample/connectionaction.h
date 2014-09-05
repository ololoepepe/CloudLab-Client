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

#ifndef CONNECTIONACTION_H
#define CONNECTIONACTION_H

class QString;
class QWidget;

#include <QObject>
#include <QWidgetAction>

/*============================================================================
================================ ConnectionAction ============================
============================================================================*/

class ConnectionAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit ConnectionAction(QObject *parent = 0);
public:
    void resetIcon(const QString &toolTip, const QString &iconName, bool animated = false);
protected:
    QWidget *createWidget(QWidget *parent);
    void deleteWidget(QWidget *widget);
};

#endif // CONNECTIONACTION_H
