#ifndef LABSPROXYMODEL_H
#define LABSPROXYMODEL_H

class LabsModel;

class TLabInfo;

class QVariant;
class QModelIndex;
class QString;
class QAbstractItemModel;

#include <QSortFilterProxyModel>
#include <QStringList>

/*============================================================================
================================ LabsProxyModel ==============================
============================================================================*/

class LabsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit LabsProxyModel(QObject *parent = 0);
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
    Q_DISABLE_COPY(LabsProxyModel)
};

#endif // LABSPROXYMODEL_H
