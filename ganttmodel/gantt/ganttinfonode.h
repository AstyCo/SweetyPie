#ifndef GANTTINFONODE_H
#define GANTTINFONODE_H

#include "ganttinfoitem.h"
#include "ganttinfoleaf.h"

#include <QList>
#include <QModelIndex>

class GanttHeader;
class GanttScene;

class GanttInfoNode : public GanttInfoItem
{
public:
    GanttInfoNode(QObject *parent = NULL);

    GanttInfoLeaf *leafAt(int index) const;
    GanttInfoNode *nodeAt(int index) const;
    GanttInfoItem *child(int index) const;

    int size() const;

    void append(GanttInfoItem* item);
    void append(const QList<GanttInfoItem*>& items);

    int columnCount() const;

    qreal height() const;

    bool isExpanded() const;
    void setIsExpanded(bool isExpanded);

protected:
    int indexOf(const GanttInfoItem * p_item) const;

private:
    QList<GanttInfoItem*> m_items;

    bool m_isExpanded;

    friend class GanttInfoItem;
    friend class GanttHeader;
    friend class GanttScene;
    friend class GanttWidget;
};

#endif // GANTTINFO_H
