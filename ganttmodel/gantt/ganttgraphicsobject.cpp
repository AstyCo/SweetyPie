#include "ganttgraphicsobject.h"
#include "ganttscene.h"
#include "ganttinfoitem.h"


GanttGraphicsObject::GanttGraphicsObject(GanttInfoItem*item,QGraphicsItem *parent)
    :QGraphicsObject(parent)
{
    m_info = item;
}

GanttInfoItem *GanttGraphicsObject::info() const
{
    return m_info.data();
}

void GanttGraphicsObject::setHeader(const QPointer<GanttHeader> &header)
{
    m_header = header;
}

void GanttGraphicsObject::setScene(const QPointer<GanttScene> &scene)
{
    if(scene.isNull())
        return;
    m_scene = scene;
    m_scene->addItem(this);
}
