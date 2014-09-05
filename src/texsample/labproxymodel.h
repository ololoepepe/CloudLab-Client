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

#ifndef LABPROXYMODEL_H
#define LABPROXYMODEL_H

class LabsModel;

class TLabInfo;

class QVariant;
class QModelIndex;
class QString;
class QAbstractItemModel;

#include <QSortFilterProxyModel>
#include <QStringList>

/*============================================================================
================================ LabProxyModel ===============================
============================================================================*/

class LabProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit LabProxyModel(QObject *parent = 0);
public:
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    void setSourceModel(QAbstractItemModel *sourceModel);
#endif
    void setSearchKeywords(const QStringList &list);
public slots:
    void setSearchKeywordsString(const QString &string);
protected:
    bool filterAcceptsColumn(int column, const QModelIndex &parent) const;
    bool filterAcceptsRow(int row, const QModelIndex &parent) const;
private:
    bool matchesKeywords(const TLabInfo &info) const;
private slots:
    void sourceModelChangedSlot();
private:
    LabsModel *mlabsModel;
    QStringList msearchKeywords;
private:
    Q_DISABLE_COPY(LabProxyModel)
};

#endif // LABPROXYMODEL_H
