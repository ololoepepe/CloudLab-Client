#include "labsmodel.h"

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
================================ LabsModel ===================================
============================================================================*/

/*============================== Static public methods =====================*/

LabsModel *LabsModel::instance()
{
    if (!minstance)
        minstance = new LabsModel;
    return minstance;
}

/*============================== Public constructors =======================*/

LabsModel::LabsModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

/*============================== Public methods ============================*/

int LabsModel::rowCount(const QModelIndex &) const
{
    return mlabs.size();
}

int LabsModel::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant LabsModel::data(const QModelIndex &index, int role) const
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

QVariant LabsModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void LabsModel::insertLab(const TLabInfo &l)
{
    TLabInfoList list;
    list << l;
    insertLabs(list);
}

void LabsModel::insertLabs(const TLabInfoList &list)
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

void LabsModel::removeLab(quint64 id)
{
    if (!id || !mlabsMap.contains(id))
        return;
    TLabInfo *l = mlabsMap.take(id);
    int ind = mlabs.indexOf(*l);
    beginRemoveRows(QModelIndex(), ind, ind);
    mlabs.removeAt(ind);
    endRemoveRows();
}

void LabsModel::removeLabs(const QList<quint64> &list)
{
    foreach (const quint64 &s, list)
        removeLab(s);
}

void LabsModel::clear()
{
    if (mlabs.isEmpty())
        return;
    beginRemoveRows(QModelIndex(), 0, mlabs.size() - 1);
    mlabsMap.clear();
    mlabs.clear();
    endRemoveRows();
}

const TLabInfo *LabsModel::lab(int index) const
{
    return ( index >= 0 && index < mlabs.size() ) ? &mlabs.at(index) : 0;
}

const TLabInfo *LabsModel::lab(quint64 id) const
{
    return id ? mlabsMap.value(id) : 0;
}

const TLabInfoList *LabsModel::labs() const
{
    return &mlabs;
}

quint64 LabsModel::indexAt(int row) const
{
    const TLabInfo *l = lab(row);
    return l ? l->id() : 0;
}

bool LabsModel::isEmpty() const
{
    return mlabs.isEmpty();
}

/*============================== Private slots =============================*/

void LabsModel::retranslateUi()
{
    headerDataChanged(Qt::Horizontal, 1, 2);
}

/*============================== Static private members ====================*/

LabsModel *LabsModel::minstance = 0;
