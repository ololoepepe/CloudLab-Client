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

#include "labmodel.h"

#include <TAuthorInfo>
#include <TAuthorInfoList>
#include <TLabInfo>

#include <BTextTools>

#include <QModelIndex>
#include <QRegExp>
#include <QSortFilterProxyModel>
#include <QString>
#include <QStringList>
#include <QVariant>

/*============================================================================
================================ LabProxyModel ===============================
============================================================================*/

/*============================== Public constructors =======================*/

LabProxyModel::LabProxyModel(LabModel *sourceModel, QObject *parent) :
    QSortFilterProxyModel(parent)
{
    mlabModel = sourceModel;
    setSourceModel(sourceModel);
}

/*============================== Public slots ==============================*/

void LabProxyModel::setSearchKeywords(const QStringList &list)
{
    QStringList nlist = list;
    nlist.removeAll("");
    nlist.removeDuplicates();
    if (nlist == msearchKeywords)
        return;
    msearchKeywords = nlist;
    invalidate();
}

void LabProxyModel::setSearchKeywordsString(const QString &string)
{
    QRegExp rx("\\,\\s*");
    setSearchKeywords(string.split(rx, QString::SkipEmptyParts));
}

/*============================== Protected methods =========================*/

bool LabProxyModel::filterAcceptsColumn(int column, const QModelIndex &) const
{
    static const QList<int> Columns = QList<int>() << 1;
    return mlabModel && Columns.contains(column);
}

bool LabProxyModel::filterAcceptsRow(int row, const QModelIndex &) const
{
    if (!mlabModel)
        return false;
    TLabInfo info = mlabModel->labInfoAt(row);
    return matchesKeywords(info);
}

/*============================== Private methods ===========================*/

bool LabProxyModel::matchesKeywords(const TLabInfo &info) const
{
    static const Qt::CaseSensitivity Cs = Qt::CaseInsensitive;
    if (msearchKeywords.isEmpty())
        return true;
    if (msearchKeywords.contains(QString::number(info.id()), Cs) || msearchKeywords.contains(info.senderLogin()))
       return true;
    foreach (const TAuthorInfo &author, info.authors()) {
        if (msearchKeywords.contains(author.name(), Cs) || msearchKeywords.contains(author.surname(), Cs)
                || msearchKeywords.contains(author.patronymic(), Cs)) {
            return true;
        }
    }
    if (msearchKeywords.contains(info.title(), Cs) || BTextTools::intersects(msearchKeywords, info.tags(), Cs))
        return true;
    return false;
}
