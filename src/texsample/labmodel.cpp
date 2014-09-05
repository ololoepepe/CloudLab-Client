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

#include "labmodel.h"

#include <TAuthorInfoList>
#include <TFileInfoList>
#include <TIdList>
#include <TLabDataInfoList>
#include <TLabInfo>
#include <TLabInfoList>

#include <QAbstractTableModel>
#include <QDateTime>
#include <QDebug>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QStringList>
#include <QVariant>

/*============================================================================
================================ LabModel ====================================
============================================================================*/

/*============================== Public constructors =======================*/

LabModel::LabModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    mlastUpdateDateTime.setTimeSpec(Qt::UTC);
}

/*============================== Public methods ============================*/

void LabModel::addLab(const TLabInfo &lab)
{
    TLabInfoList list;
    list << lab;
    addLabs(list);
}

void LabModel::addLabs(const TLabInfoList &labList)
{
    TLabInfoList list = labList;
    foreach (int i, bRangeR(list.size() - 1, 0)) {
        const TLabInfo &info = list.at(i);
        if (map.contains(info.id())) {
            if (info.lastModificationDateTime() > map.value(info.id())->lastModificationDateTime()) {
                int row = indexOf(info.id());
                labs[row] = info;
                map.insert(info.id(), &labs[row]);
                Q_EMIT dataChanged(index(row, 0), index(row, columnCount() - 1));
            }
            list.removeAt(i);
        } else if (!info.isValid()) {
            list.removeAt(i);
        }
    }
    if (list.isEmpty())
        return;
    int ind = labs.size();
    beginInsertRows(QModelIndex(), ind, ind + list.size() - 1);
    foreach (TLabInfo info, list) {
        labs.append(info);
        map.insert(info.id(), &labs.last());
    }
    endInsertRows();
}

void LabModel::clear()
{
    mlastUpdateDateTime = QDateTime().toUTC();
    if (labs.isEmpty())
        return;
    map.clear();
    beginRemoveRows(QModelIndex(), 0, labs.size() - 1);
    labs.clear();
    endRemoveRows();
}

int LabModel::columnCount(const QModelIndex &) const
{
    return 12;
}

QVariant LabModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() > columnCount() - 1 || Qt::DisplayRole != role)
        return QVariant();
    TLabInfo info = labInfoAt(index.row());
    if (!info.isValid())
        return QVariant();
    switch (index.column()) {
    case 0:
        return info.id();
    case 1:
        return info.title();
    case 2:
        return info.senderId();
    case 3:
        return info.senderLogin();
    case 4:
        return info.description();
    case 5:
        return info.authors();
    case 6:
        return info.tags();
    case 7:
        return info.groups();
    case 8:
        return info.dataInfos();
    case 9:
        return info.extraFiles();
    case 10:
        return info.creationDateTime();
    case 11:
        return info.lastModificationDateTime();
    default:
        return QVariant();
    }
}

QVariant LabModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::Horizontal != orientation || Qt::DisplayRole != role)
        return QVariant();
    switch (section) {
    case 0:
        return tr("ID", "headerData");
    case 1:
        return tr("Title", "headerData");
    case 2:
        return tr("Sender ID", "headerData");
    case 3:
        return tr("Sender login", "headerData");
    case 4:
        return tr("Description", "headerData");
    case 5:
        return tr("Authors", "headerData");
    case 6:
        return tr("Tags", "headerData");
    case 7:
        return tr("Groups", "headerData");
    case 8:
        return tr("Data infos", "headerData");
    case 9:
        return tr("Extra file infos", "headerData");
    case 10:
        return tr("Creation date", "headerData");
    case 11:
        return tr("Last modified", "headerData");
    default:
        return QVariant();
    }
}

quint64 LabModel::labIdAt(int index) const
{
    if (index < 0 || index >= labs.size())
        return 0;
    return labs.at(index).id();
}

TLabInfo LabModel::labInfo(quint64 id) const
{
    const TLabInfo *info = id ? map.value(id) : 0;
    if (!info)
        return TLabInfo();
    return *info;
}

TLabInfo LabModel::labInfoAt(int index) const
{
    if (index < 0 || index >= labs.size())
        return TLabInfo();
    return labs.at(index);
}

QDateTime LabModel::lastUpdateDateTime() const
{
    return mlastUpdateDateTime;
}

void LabModel::removeLab(quint64 id)
{
    if (!id || !map.contains(id))
        return;
    map.remove(id);
    int ind = indexOf(id);
    beginRemoveRows(QModelIndex(), ind, ind);
    labs.removeAt(ind);
    endRemoveRows();
}

void LabModel::removeLabs(const TIdList &idList)
{
    foreach (quint64 id, idList)
        removeLab(id);
}

int LabModel::rowCount(const QModelIndex &) const
{
    return labs.size();
}

void LabModel::update(const TLabInfoList &newLabs, const TIdList &deletedLabs, const QDateTime &requestDateTime)
{
    removeLabs(deletedLabs);
    addLabs(newLabs);
    mlastUpdateDateTime = requestDateTime.toUTC();
}

void LabModel::update(const TLabInfoList &newLabs, const QDateTime &requestDateTime)
{
    update(newLabs, TIdList(), requestDateTime);
}

void LabModel::updateLab(quint64 labId, const TLabInfo &newInfo)
{
    TLabInfo *info = map.value(labId);
    if (!info)
        return;
    int row = indexOf(labId);
    *info = newInfo;
    emit dataChanged(index(row, 0), index(row, columnCount() - 1));
}

/*============================== Private methods ===========================*/

int LabModel::indexOf(quint64 id) const
{
    if (!id)
        return -1;
    foreach (int i, bRangeD(0, labs.size() - 1)) {
        if (labs.at(i).id() == id)
            return i;
    }
    return -1;
}
