#include "ganttgraphicsobject.h"
#include "gantt/plot/ganttscene.h"
#include "gantt/info/ganttinfoitem.h"

void GanttGraphicsObject::init()
{
    _current = false;
    _dtline = NULL;
}

GanttGraphicsObject::GanttGraphicsObject(GanttInfoItem*item,QGraphicsItem *parent)
    :QGraphicsObject(parent)
{
    m_info = item;

    init();
}

GanttGraphicsObject::~GanttGraphicsObject()
{
    if(scene()){
        scene()->removeItem(this);
        setParentItem(NULL);
    }

    if(info())
        info()->reduceLinkCnt();
}

GanttInfoItem *GanttGraphicsObject::info() const
{
    return m_info.data();
}


void GanttGraphicsObject::setScene(GanttScene *scene)
{
    if(!scene)
        return;

    scene->addItem(this);

}

void GanttGraphicsObject::updateItemGeometry()
{
    qWarning() << "GanttGraphicsObject::updateItemGeometry using, no override";
}

void GanttGraphicsObject::updateToolTip()
{
    qWarning() << "GanttGraphicsObject::updateToolTip using, no override";
}

void GanttGraphicsObject::setCurrent(bool newValue)
{
    _current = newValue;

    updateZValue();
    update();
}

void GanttGraphicsObject::updateZValue()
{
    if(_current)
        setZValue(600);
    else
        setZValue(inactiveZValue());
}
