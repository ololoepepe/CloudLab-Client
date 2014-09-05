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

class QStringList;
class QVariant;

#include <TLabInfoList>

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QString>
#include <QList>
#include <QMap>
#include <QVariantMap>

#define sModel LabsModel::instance()

/*============================================================================
================================ LabModel ====================================
============================================================================*/

class LabModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    static LabModel *instance();
public:
    explicit LabModel(QObject *parent = 0);
public:
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void insertLab(const TLabInfo &l);
    void insertLabs(const TLabInfoList &list);
    void removeLab(quint64 id);
    void removeLabs(const QList<quint64> &list);
    void clear();
    const TLabInfo* lab(int index) const;
    const TLabInfo* lab(quint64 id) const;
    const TLabInfoList *labs() const;
    quint64 indexAt(int row) const;
    bool isEmpty() const;
private slots:
    void retranslateUi();
private:
    static LabModel *minstance;
private:
    TLabInfoList mlabs;
    QMap<quint64, TLabInfo *> mlabsMap;
private:
    Q_DISABLE_COPY(LabModel)
};

#endif // LABMODEL_H
