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

#include "labproxymodel.h"
#include "labsmodel.h"
#include "client.h"
#include "application.h"
#include "texsamplesettingstab.h"

#include <TLabInfo>
#include <BTextTools>

#include <BeQtGlobal>

#include <QSortFilterProxyModel>
#include <QVariant>
#include <QModelIndex>
#include <QString>
#include <QStringList>
#include <QRegExp>

static QStringList words(const QString &string)
{
    QStringList list;
    QString s;
    foreach (int i, bRangeD(0, string.length() - 1))
    {
        if (string.at(i).isLetterOrNumber() || string.at(i) == '_')
        {
            s += string.at(i);
        }
        else
        {
            if (!list.contains(s) && !s.isEmpty())
                list << s;
            s.clear();
        }
    }
    if (!list.contains(s) && !s.isEmpty())
        list << s;
    return list;
}

/*============================================================================
================================ LabProxyModel ===============================
============================================================================*/

/*============================== Public constructors =======================*/

LabProxyModel::LabProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    mlabsModel = 0;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    connect(this, SIGNAL(sourceModelChanged()), SLOT(sourceModelChangedSlot()));
#endif
}

/*============================== Public methods ============================*/

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
void LabProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
    sourceModelChangedSlot();
}
#endif

void LabProxyModel::setSearchKeywords(const QStringList &list)
{
    msearchKeywords = list;
    msearchKeywords.removeAll("");
    msearchKeywords.removeDuplicates();
    invalidate();
}

/*============================== Public slots ==============================*/

void LabProxyModel::setSearchKeywordsString(const QString &string)
{
    static QRegExp rx("\\,\\s*");
    setSearchKeywords(string.split(rx, QString::SkipEmptyParts));
}

/*============================== Protected methods =========================*/

bool LabProxyModel::filterAcceptsColumn(int column, const QModelIndex &) const
{
    return mlabsModel && bRange(1, 2).contains(column);
}

bool LabProxyModel::filterAcceptsRow(int row, const QModelIndex &) const
{
    const TLabInfo *s = mlabsModel ? mlabsModel->lab(row) : 0;
    return s && matchesKeywords(*s);
}

/*============================== Private methods ===========================*/

bool LabProxyModel::matchesKeywords(const TLabInfo &info) const
{
    Qt::CaseSensitivity cs = Qt::CaseInsensitive;
    if (msearchKeywords.isEmpty())
        return true;
    QStringList list;
    foreach (const QString &a, info.authors())
    {
        foreach (const QString &w, words(a))
            if (!list.contains(w) && !w.isEmpty())
                list << w;
    }
    return msearchKeywords.contains(info.idString(), cs) || msearchKeywords.contains(info.sender().login())
            || BTextTools::intersects(msearchKeywords, words(info.sender().realName()), cs)
            || BTextTools::intersects(msearchKeywords, list, cs)
            || BTextTools::intersects(msearchKeywords, words(info.title()), cs)
            || BTextTools::intersects(msearchKeywords, info.tags(), cs);
}

/*============================== Private slots =============================*/

void LabProxyModel::sourceModelChangedSlot()
{
    mlabsModel = static_cast<LabsModel *>(sourceModel());
}
