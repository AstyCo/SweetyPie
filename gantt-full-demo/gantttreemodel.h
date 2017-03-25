#ifndef GANTTTREEMODEL_H
#define GANTTTREEMODEL_H

#include "gantt/info/ganttinfoitem.h"

#include <QAbstractItemModel>

class GanttWidget;

class GanttTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Field
    {
        titleField,
        startField,
        finishField,
        durationField,

        Fields_count

    };


public:
    GanttTreeModel(GanttInfoItem *root = NULL, QObject * parent = 0);
    ~GanttTreeModel();

    QVariant data(const QModelIndex &iGanttIndex, int role) const;
    Qt::ItemFlags flags(const QModelIndex &iGanttIndex) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &iGanttIndex) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool setData(const QModelIndex &iGanttIndex, const QVariant &value,
                 int role = Qt::EditRole);

    void addItems(const QList<GanttInfoItem*>& items, GanttInfoItem *destNode = NULL);
    void addNode(GanttInfoItem *node);
//    void addItems(GanttInfoItem* item, bool inner = false);
    void insertItem(GanttInfoItem *item, GanttInfoItem *parent);
    void addInner(GanttInfoItem *item, GanttInfoItem *targetItem);
    void addAfter(GanttInfoItem *item, GanttInfoItem *targetItem);
    void addBefore(GanttInfoItem *item, GanttInfoItem *targetItem);
    bool canFetchMore(const QModelIndex &/*parent*/) const{
        return true;
    }

    GanttInfoItem *itemForName(const QString& iGanttTitle, GanttInfoItem *parent = NULL) const;


    GanttInfoItem *root() const;


    GanttInfoItem* itemForIndex(const QModelIndex& iGanttIndex) const;

    void printTree() const;

    void serialize(const QString &filename);
    void deserialize(const QString &filename);


signals:
    void itemsAdded(GanttInfoItem*);
    void itemsAdded();
    void needCollapse(GanttInfoItem*);

public slots:
    void clear();

private:
    GanttInfoItem *infoForIndex(const QModelIndex &index) const;
    QModelIndex indexForInfo(const GanttInfoItem *item) const;
//    void setIndexR(GanttInfoItem* item);
//    void setIndex(GanttInfoItem* item);
    GanttInfoItem *itemForNameHelper(const QString& iGanttTitle, GanttInfoItem *node) const;

private:

    GanttInfoItem *_root;

    friend QDataStream &operator<<(QDataStream &ds, const GanttTreeModel &tm);
    friend QDataStream &operator>>(QDataStream &ds, GanttTreeModel &tm);

};

QDataStream &operator<<(QDataStream &ds, const GanttTreeModel &tm);
QDataStream &operator>>(QDataStream &ds, GanttTreeModel &tm);


#endif // GANTTTREEMODEL_H
