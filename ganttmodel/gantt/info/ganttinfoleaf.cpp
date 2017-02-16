#include "ganttinfoleaf.h"
#include "ganttinfonode.h"

#include "gantt/private_extensions/gantt-lib_global_values.h"

GanttInfoLeaf::GanttInfoLeaf(QObject *parent)
    :GanttInfoItem(parent)
{

}

GanttInfoLeaf::GanttInfoLeaf(const QString &title, const UtcDateTime &start, const TimeSpan &ts, const QModelIndex &index
                             , const QColor &color, GanttInfoNode *parentNode, QObject *parent)
    : GanttInfoItem(title,start,ts,index,color,parentNode,parent)
{

}




int GanttInfoLeaf::columnCount() const
{
    return 4;
}

qreal GanttInfoLeaf::height() const
{
    return DEFAULT_ITEM_WIDTH;
}

void GanttInfoLeaf::callForEachItemRecursively(void (*func)(GanttInfoItem *))
{
    if(func)
        (*func)(this);
}

GanttInfoNode *GanttInfoLeaf::node()
{
    if(parent())
        return parent()->node();
    return NULL;
}

void GanttInfoLeaf::disconnectParent()
{
    GanttInfoItem::disconnectParent();
    if(parent()){
        disconnect( parent(), SIGNAL(expanded()), this, SIGNAL(expanded()));
        disconnect( parent(), SIGNAL(collapsed()), this, SIGNAL(collapsed()));
    }
}

void GanttInfoLeaf::connectParent()
{
    GanttInfoItem::connectParent();
    if(parent()){
        connect( parent(), SIGNAL(expanded()), this, SIGNAL(expanded()));
        connect( parent(), SIGNAL(collapsed()), this, SIGNAL(collapsed()));
    }
}



