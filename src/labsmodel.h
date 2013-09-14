#ifndef LABSMODEL_H
#define LABSMODEL_H

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
================================ LabsModel ===================================
============================================================================*/

class LabsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    static LabsModel *instance();
public:
    explicit LabsModel(QObject *parent = 0);
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
    static LabsModel *minstance;
private:
    TLabInfoList mlabs;
    QMap<quint64, TLabInfo *> mlabsMap;
private:
    Q_DISABLE_COPY(LabsModel)
};

#endif // LABSMODEL_H
