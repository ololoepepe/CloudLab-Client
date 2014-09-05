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

#include <TUserInfo>
#include <TLabInfo>
#include <TLabInfoList>

#include <BApplication>

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QString>
#include <QVariantMap>

#include <QDebug>

/*============================================================================
================================ LabModel ====================================
============================================================================*/

/*============================== Static public methods =====================*/

LabModel *LabModel::instance()
{
    if (!minstance)
        minstance = new LabModel;
    return minstance;
}

/*============================== Public constructors =======================*/

LabModel::LabModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

/*============================== Public methods ============================*/

int LabModel::rowCount(const QModelIndex &) const
{
    return mlabs.size();
}

int LabModel::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant LabModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || Qt::DisplayRole != role)
        return QVariant();
    const TLabInfo *l = lab( index.row() );
    if (!l)
        return QVariant();
    switch ( index.column() )
    {
    case 0:
        return l->id();
    case 1:
        return l->title();
    case 2:
        return l->sender().login();
    default:
        return QVariant();
    }
}

QVariant LabModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::Horizontal != orientation || Qt::DisplayRole != role)
        return QVariant();
    switch (section)
    {
    case 1:
        return tr("Title", "headerData");
    case 2:
        return tr("Sender", "headerData");
    default:
        return QVariant();
    }
}

void LabModel::insertLab(const TLabInfo &l)
{
    TLabInfoList list;
    list << l;
    insertLabs(list);
}

void LabModel::insertLabs(const TLabInfoList &list)
{
    TLabInfoList nlist = list;
    foreach (int i, bRangeR(nlist.size() - 1, 0))
    {
        const TLabInfo &l = nlist.at(i);
        if ( !l.isValid(TLabInfo::GeneralContext) )
            nlist.removeAt(i);
        else if ( mlabsMap.contains( l.id() ) )
            removeLab( l.id() );
    }
    if (nlist.isEmpty())
        return;
    int ind = mlabs.size();
    beginInsertRows(QModelIndex(), ind, ind + nlist.size() - 1);
    foreach (const TLabInfo &l, nlist)
    {
        mlabs.append(l);
        mlabsMap.insert( l.id(), &mlabs.last() );
    }
    endInsertRows();
}

void LabModel::removeLab(quint64 id)
{
    if (!id || !mlabsMap.contains(id))
        return;
    TLabInfo *l = mlabsMap.take(id);
    int ind = mlabs.indexOf(*l);
    beginRemoveRows(QModelIndex(), ind, ind);
    mlabs.removeAt(ind);
    endRemoveRows();
}

void LabModel::removeLabs(const QList<quint64> &list)
{
    foreach (const quint64 &s, list)
        removeLab(s);
}

void LabModel::clear()
{
    if (mlabs.isEmpty())
        return;
    beginRemoveRows(QModelIndex(), 0, mlabs.size() - 1);
    mlabsMap.clear();
    mlabs.clear();
    endRemoveRows();
}

const TLabInfo *LabModel::lab(int index) const
{
    return ( index >= 0 && index < mlabs.size() ) ? &mlabs.at(index) : 0;
}

const TLabInfo *LabModel::lab(quint64 id) const
{
    return id ? mlabsMap.value(id) : 0;
}

const TLabInfoList *LabModel::labs() const
{
    return &mlabs;
}

quint64 LabModel::indexAt(int row) const
{
    const TLabInfo *l = lab(row);
    return l ? l->id() : 0;
}

bool LabModel::isEmpty() const
{
    return mlabs.isEmpty();
}

/*============================== Private slots =============================*/

void LabModel::retranslateUi()
{
    headerDataChanged(Qt::Horizontal, 1, 2);
}

/*============================== Static private members ====================*/

LabModel *LabModel::minstance = 0;
