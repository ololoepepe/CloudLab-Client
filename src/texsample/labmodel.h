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

#ifndef LABMODEL_H
#define LABMODEL_H

class TLabInfo;

class QVariant;

#include <TIdList>
#include <TLabInfoList>

#include <QAbstractTableModel>
#include <QDateTime>
#include <QList>
#include <QMap>
#include <QModelIndex>

/*============================================================================
================================ LabModel ====================================
============================================================================*/

class LabModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    TLabInfoList labs;
    QMap<quint64, TLabInfo *> map;
    QDateTime mlastUpdateDateTime;
public:
    explicit LabModel(QObject *parent = 0);
public:
    void addLab(const TLabInfo &lab);
    void addLabs(const TLabInfoList &labList);
    void clear();
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    quint64 labIdAt(int index) const;
    TLabInfo labInfo(quint64 id) const;
    TLabInfo labInfoAt(int index) const;
    QDateTime lastUpdateDateTime() const;
    void removeLab(quint64 id);
    void removeLabs(const TIdList &idList);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    void update(const TLabInfoList &newLabs, const TIdList &deletedLabs,
                const QDateTime &requestDateTime = QDateTime());
    void update(const TLabInfoList &newLabs, const QDateTime &requestDateTime = QDateTime());
    void updateLab(quint64 labId, const TLabInfo &newInfo);
public:
    int indexOf(quint64 id) const;
private:
    Q_DISABLE_COPY(LabModel)
};

#endif // LABMODEL_H
