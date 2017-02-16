#ifndef GANTTTREEMODEL_H
#define GANTTTREEMODEL_H

#include "ganttproject_global.h"
#include "gantt/info/ganttinfoleaf.h"
#include "gantt/info/ganttinfonode.h"
#include "interfaces/iganttmodel.h"

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
    GanttTreeModel(GanttInfoNode *root = NULL,QObject * parent = 0);
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

    void addItems(const QList<GanttInfoItem*>& items);
    void addNode(GanttInfoNode *node);
//    void addItems(GanttInfoItem* item, bool inner = false);
    void insertItem(GanttInfoItem *item, GanttInfoNode *parent = NULL);
    bool canFetchMore(const QModelIndex &/*parent*/) const{
        return true;
    }

    GanttInfoItem *itemForName(const QString& iGanttTitle) const;


    GanttInfoNode *root() const;


    GanttInfoItem* itemForIndex(const QModelIndex& iGanttIndex) const;
    GanttInfoLeaf* leafForIndex(const QModelIndex& iGanttIndex) const;
    GanttInfoNode* nodeForIndex(const QModelIndex& iGanttIndex) const;


signals:
    void itemsAdded(GanttInfoItem*);
    void itemsAdded();
    void needCollapse(GanttInfoNode*);

public slots:
    void clear();

private:

    void setIndex(GanttInfoItem* item);
    GanttInfoItem *itemForNameHelper(const QString& iGanttTitle,GanttInfoNode* node) const;

private:

    GanttInfoNode *m_root;

    friend class GanttWidget;

};

#endif // GANTTTREEMODEL_H
