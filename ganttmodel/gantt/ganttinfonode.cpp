#include "ganttinfonode.h"
#include "gantttreemodel.h"

#include "ganttglobalvalues.h"

GanttInfoNode::GanttInfoNode(QObject *parent)
    :GanttInfoItem(parent)
{
    m_isExpanded = false;
}

GanttInfoLeaf *GanttInfoNode::leafAt(int index) const
{
    if(index>=size())
        return NULL;

    return dynamic_cast<GanttInfoLeaf*>(m_items[index]);
}

GanttInfoNode *GanttInfoNode::nodeAt(int index) const
{
    if(index>=size())
        return NULL;

    return dynamic_cast<GanttInfoNode*>(m_items[index]);
}

GanttInfoItem *GanttInfoNode::child(int index) const
{
    if(index>=size())
        return NULL;
    return m_items[index];
}

int GanttInfoNode::size() const
{
    return m_items.size();
}


void GanttInfoNode::append(GanttInfoItem *item)
{
    item->setParent(this);
    return m_items.append(item);
}

void GanttInfoNode::append(const QList<GanttInfoItem *> &items)
{
    foreach(GanttInfoItem* item, items)
        item->setParent(this);
    return m_items.append(items);
}

int GanttInfoNode::columnCount() const
{
    return  1; //Only title  //GanttTreeModel::Fields_count;
}

qreal GanttInfoNode::height() const
{
    if(m_isExpanded)
    {
        qreal childsHeight = 0;
        foreach(GanttInfoItem* item, m_items)
        {
            childsHeight+=item->height();
        }
        return childsHeight + DEFAULT_ITEM_WIDTH;
    }
    else
    {
        return DEFAULT_ITEM_WIDTH;
    }
}

int GanttInfoNode::indexOf(const GanttInfoItem * p_item) const
{
    GanttInfoItem *p = const_cast<GanttInfoItem*>(p_item);
    return m_items.indexOf(p);
}
void GanttInfoNode::setIsExpanded(bool isExpanded)
{
    m_isExpanded = isExpanded;
}

bool GanttInfoNode::isExpanded() const
{
    return m_isExpanded;
}


