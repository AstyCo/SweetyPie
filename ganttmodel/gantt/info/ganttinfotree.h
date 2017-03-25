/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление GanttInfoTree class.
 * \~englist
 * \brief File contains definition of GanttInfoTree class.
 */
#ifndef GANTTINFOTREE_H
#define GANTTINFOTREE_H

#include "extensions-gui/interfaces/abstractganttfactory.h"
#include "utcdatetime.h"

#include <QObject>
#include <QAbstractItemModel>
#include <QTreeView>

//#include <QtConcurrentRun>

/*!
 * \~russian
 * \brief Класс GanttInfoTree class содержит информацию по всем событиям
 * \~english
 * \brief The GanttInfoTree class class contains information of all events
 */
class GanttInfoTree : public QObject
{
    Q_OBJECT
    void init();
public:
    explicit GanttInfoTree(QObject *parent = 0);

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);
    void setFactory(AbstractGanttFactory *factory);

    void connectTreeView(QTreeView *view);
    void disconnectTreeView(QTreeView *view);
//  (Migrated to GanttFactory)
    GanttInfoItem *infoForIndex(const QModelIndex &index) const;
    QModelIndex indexForInfo(const GanttInfoItem *item) const;
    GanttInfoItem *nextStart(const UtcDateTime &dt) const;
    GanttInfoItem *prevFinish(const UtcDateTime &dt) const;

    GanttInfoItem *root() const;
    GanttInfoItem *infoForVPos(int vpos);
    int height() const;


signals:
    void treeReset();
    void limitsChanged(const UtcDateTime &min, const TimeSpan &ts);
    void rowsInserted(GanttInfoItem *parent, int from, int to);
    void endRemoveItems();
    void itemAdded(GanttInfoItem *item);
    void itemRemoved(GanttInfoItem *item);
    void currentChanged(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);
    void currentChanged(GanttInfoItem *item);
    void needExpand(const QModelIndex &index);
    void needCollapse(const QModelIndex &index);
    void expanded(GanttInfoItem *which);
    void collapsed(GanttInfoItem *which);

public slots:
    void onCurrentItemChanged(const GanttInfoItem *item);
    void onNodeExpanded();
    void onNodeCollapsed();
    void onClicked(const QModelIndex &index);
    void onExpanded(const QModelIndex &index);
    void onCollapsed(const QModelIndex &index);

    void clear();
    void reset();
private slots:
//    void removeFromHash();
//    void addToHash();
    void onDataChanged(const QModelIndex &from, const QModelIndex &to);
    void onRowsInserted(const QModelIndex &parent, int start, int end);
    void onColumnsInserted(const QModelIndex &parent, int start, int end);
    void onRowsRemoved(const QModelIndex &parent, int start, int end);
    void onColumnsRemoved(const QModelIndex &parent, int start, int end);
    void onItemAboutToBeDeleted();

    void connectNewItem(GanttInfoItem *item);
    void updateLimits();
    void updateLimitsByItem(GanttInfoItem *item);
    void emitLimitsChanged();
    void collapseAll();

private:
    void clearLimits();
    void fillByModelIndex(const QModelIndex &parent = QModelIndex());
    void setLimits(const QPair<UtcDateTime, UtcDateTime> &newLimits);
    int  heightH(GanttInfoItem *item) const;
    void onAnyAddition();
    GanttInfoItem *lookupForVPos(int vpos, GanttInfoItem *node);
    void fill(GanttInfoItem *node, const QModelIndex &index, int from, int to);
    GanttInfoItem *makeInfoItem(const QModelIndex &index);
    void disconnectLastModel();
    void connectNewModel();
    void clearInfoForDtHelper(QMap<UtcDateTime, GanttInfoItem*> &map,
                              const UtcDateTime &dt,
                              GanttInfoItem *item);
private:
    QAbstractItemModel *_model;
    GanttInfoItem *_root;
    AbstractGanttFactory *_factory;


    bool _limitsChanged;                                            // caches Limits
    QPair<UtcDateTime, UtcDateTime> _limits;                        // caches Limits
    QMap<UtcDateTime,GanttInfoItem*> _infoForStart, _infoForFinish; // caches
};

#endif // GANTTINFOTREE_H
