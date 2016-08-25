#include "ganttinfoitem.h"
#include "ganttinfonode.h"

#include "ganttglobalvalues.h"

GanttInfoItem::GanttInfoItem(QObject *parent)
    :QObject(parent)
{
    m_parent = NULL;

    connect(this,SIGNAL(indexChanged()),this,SIGNAL(changed()));
    connect(this,SIGNAL(titleChanged()),this,SIGNAL(changed()));
    connect(this,SIGNAL(parentChanged()),this,SIGNAL(changed()));
}

QString GanttInfoItem::title() const
{
    return m_title;
}

void GanttInfoItem::setTitle(const QString &title)
{
    if(title == m_title)
        return;
    m_title = title;
    emit titleChanged();
}

int GanttInfoItem::row() const
{
    if(m_parent)
        return m_parent->indexOf(this);

    return 0;
}

int GanttInfoItem::indexOf(const GanttInfoItem* p_item) const
{
    Q_UNUSED(p_item);
    return -1;
}

QModelIndex GanttInfoItem::index() const
{
    return m_index;
}

void GanttInfoItem::setIndex(const QModelIndex &index)
{
    if(index == m_index)
        return;

    m_index = index;
    emit indexChanged();
}

void GanttInfoItem::setParent(GanttInfoNode *parent)
{
    if(parent == m_parent)
        return;

    m_parent = parent;
    emit parentChanged();
}

qreal GanttInfoItem::pos() const
{
    GanttInfoNode * p_parent = parent();

    if(!p_parent)
        return 0.0; // m_root

    qreal base = p_parent->pos();


    if(!p_parent->isExpanded())
        return base;

    qreal offset = (p_parent->parent())?(DEFAULT_ITEM_WIDTH):(0); // if m_root don't need extra offset
    const QList<GanttInfoItem*>& items = p_parent->m_items;
    // looking for this item, stops when found
    for(int i = 0; i < items.size(); ++i)
    {
        if( items[i] == this)
            break;

        offset += items[i]->height();
    }

    return base + offset;
}


GanttInfoNode *GanttInfoItem::parent() const
{
    return m_parent;
}

