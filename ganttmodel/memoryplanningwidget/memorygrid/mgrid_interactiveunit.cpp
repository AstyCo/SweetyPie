#ifndef MEMORYINTERACTIVEUNIT_CPP
#define MEMORYINTERACTIVEUNIT_CPP

#include "mgrid_interactiveunit.h"
#include "mgrid_scene.h"
#include "mgrid_item.h"


#include "memorypart.h"

#include "memory_globalvalues.h"

#include <QPainterPath>
#include <QPainter>

#include <QDebug>

extern MGridScene* mem_scene;

MGridInteractiveUnit::MGridInteractiveUnit(MGridScene* scene,QGraphicsItem *parent /*= 0*/)
    : QGraphicsItem(parent,scene)
{
    m_start=m_length=0;
    m_scene=dynamic_cast<MGridScene*>(scene);
    if(!m_scene)
    {
        qDebug() << "MemoryInteractiveUnit no scene";
    }

    m_enabled = false;

    m_items = &(m_scene->m_items);

    m_borderPen=QPen(QBrush(Qt::red), 1 ,Qt::SolidLine ,Qt::SquareCap,Qt::MiterJoin);

    setZValue(1000);
}

void MGridInteractiveUnit::disable()
{
    m_enabled = false;
}

QRectF MGridInteractiveUnit::boundingRect() const
{
    return m_shapeBorder.boundingRect().adjusted(-extraSize(),-extraSize(),+extraSize(),+extraSize());
}

QPainterPath MGridInteractiveUnit::shape() const
{
    return m_shapeBorder;
}

void MGridInteractiveUnit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(!m_enabled)
        return;
    painter->setPen(m_borderPen);
    painter->drawPath(m_shapeBorder);
}

bool MGridInteractiveUnit::inRange(long index) const
{
    return m_enabled&&(index>=m_start && index <m_start+m_length);
}


long MGridInteractiveUnit::start() const
{
    return m_start;
}

void MGridInteractiveUnit::setStart(long start)
{
    m_start = start;
}

//qreal MGridInteractiveUnit::spacing() const
//{
//    MGridScene* p_memScene = dynamic_cast<MGridScene*>(scene());
//    if(!p_memScene)
//        return DEFAULT_SPACING;
//    return p_memScene->spacing();
//}


void MGridInteractiveUnit::rebuildShape()
{
    if(!m_enabled)
        return;
    if(m_length==0)
    {
        setShapeBorder(QPainterPath());
        return;
    }
    if(m_start+m_length>m_items->size())
    {
        qDebug() << "MemoryInteractiveUnit::rebuildShape() out of range";
        return;
    }

//    qDebug() <<"rebuild shape "<< m_start<<' '<<m_finish;

    QRectF itemsRect;

    for(int i = m_start; i < m_start+m_length; ++i)
    {
        itemsRect|=m_items->at(i)->geometry();
    }

    MGridItem  *utterLeftItem = m_items->at(m_start),
                *utterRightItem = m_items->at(m_start+m_length-1);
    qreal   utterLeft = utterLeftItem->left(),
            utterRight = utterRightItem->right();

    QPainterPath path;


    bool shapingTop = false,
         shapingBottom = false,
         shapingSeparate = false;
    if(utterLeftItem->left()!=itemsRect.left())
        shapingTop = true;
    if(utterRightItem->right()!=itemsRect.right())
        shapingBottom = true;
    if(length()<m_scene->itemPerRow()
            && utterLeft>utterRight)
        shapingSeparate = true;

    path.moveTo(utterLeftItem->geometry().topLeft());
    path.lineTo(itemsRect.topRight());

    if(shapingBottom)
    {
        path.lineTo(QPointF(itemsRect.right(),utterRightItem->top()));
        if(shapingSeparate)
        {
            path.lineTo(utterLeftItem->geometry().bottomLeft());
            path.moveTo(utterRightItem->geometry().topRight());
        }
        else
            path.lineTo(utterRightItem->geometry().topRight());
    }

    path.lineTo(utterRightItem->geometry().bottomRight());
    path.lineTo(itemsRect.bottomLeft());

    if(shapingTop)
    {
        path.lineTo(QPointF(itemsRect.left(),utterLeftItem->bottom()));
        if(shapingSeparate)
        {
            path.lineTo(utterRightItem->geometry().topRight());
            path.moveTo(utterLeftItem->geometry().bottomLeft());
        }
        else
            path.lineTo(utterLeftItem->geometry().bottomLeft());
    }

    path.lineTo(utterLeftItem->geometry().topLeft());

    setShapeBorder(path);
}

QPainterPath MGridInteractiveUnit::shapeBorder() const
{
    return m_shapeBorder;
}

void MGridInteractiveUnit::setShapeBorder(const QPainterPath &shapeBorder)
{
    prepareGeometryChange();
    m_shapeBorder = shapeBorder;
}

qreal MGridInteractiveUnit::extraSize() const
{
    if(!m_scene)
        return 0;
    return m_scene->itemBorder();
}

long MGridInteractiveUnit::length() const
{
    return m_length;
}

void MGridInteractiveUnit::setLength(long length)
{
    m_length = length;
}

long MGridInteractiveUnit::memorySize() const
{
    if(!m_items)
        return 0;
    return m_items->size();
}



void MGridInteractiveUnit::setRange(long start, long length)
{
    if(!m_items)
    {
        qDebug() << "MemoryInteractiveUnit:m_items NULL";
        return;
    }
    if(start<0||start+length>memorySize())
    {
        return;
    }

    if(!m_enabled)
        m_enabled = true;

    setStart(start);
    setLength(length);

    rebuildShape();
}

#endif // MEMORYINTERACTIVEUNIT_CPP

