#ifndef MEMORYINTERACTIVEUNIT_CPP
#define MEMORYINTERACTIVEUNIT_CPP

#include "mgrid_interactiveunit.h"
#include "mgrid_scene.h"
#include "mgrid_item.h"


#include "kamemorypart.h"

#include "memory_globalvalues.h"

#include <QPainterPath>
#include <QPainter>

#include <QDebug>

extern MGridScene* mem_scene;

MGridInteractiveUnit::MGridInteractiveUnit(MGridScene* scene,QGraphicsItem *parent /*= 0*/)
    : QGraphicsItem(parent,scene)
{
    m_scene=dynamic_cast<MGridScene*>(scene);
    if(!m_scene)
    {
        qDebug() << "MemoryInteractiveUnit no scene";
    }

    m_enabled = false;

    m_items = &(m_scene->m_items);

    m_borderPen=QPen(QBrush(Qt::darkRed), m_scene->itemBorder() ,Qt::SolidLine ,Qt::SquareCap,Qt::MiterJoin);

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

    qDebug() << "paint";
    if(!m_enabled)
        return;
    qDebug() <<"success";
//    QPen pen(QBrush(Qt::blue),DEFAULT_SPACING ,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
    painter->setPen(m_borderPen);
    qDebug() << m_borderPen;
    qDebug() << m_shapeBorder;
    painter->drawPath(m_shapeBorder);
//    painter->fillPath(m_shapeBorder,QBrush(Qt::blue));
}

bool MGridInteractiveUnit::inRange(long index) const
{
    return m_enabled&&(index>=m_start && index <=m_finish);
}


long MGridInteractiveUnit::start() const
{
    return m_start;
}

void MGridInteractiveUnit::setStart(long start)
{
    m_start = start;
}

long MGridInteractiveUnit::finish() const
{
    return m_finish;
}

void MGridInteractiveUnit::setFinish(long finish)
{
    m_finish = finish;
}

long MGridInteractiveUnit::size() const
{
    return m_finish-m_start + 1;
}

void MGridInteractiveUnit::setSize(long newSize)
{
    setFinish(start()+newSize-1);
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
    if(m_finish>m_items->size())
    {
        qDebug() << "MemoryInteractiveUnit::rebuildShape() out of range";
        return;
    }

    QRectF itemsRect;

    for(int i = m_start; i <= m_finish; ++i)
    {
        itemsRect|=m_items->at(i)->geometry();
    }

    MGridItem  *utterLeftItem = m_items->at(m_start),
                *utterRightItem = m_items->at(m_finish);
    qreal   utterLeft = utterLeftItem->left(),
            utterRight = utterLeftItem->right();

    QPainterPath path;


    bool shapingTop = false,
         shapingBottom = false,
         shapingSeparate = false;
    if(utterLeftItem->left()!=itemsRect.left())
        shapingTop = true;
    if(utterRightItem->right()!=itemsRect.right())
        shapingBottom = true;
    if(size()<m_scene->itemPerRow()
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

long MGridInteractiveUnit::memorySize() const
{
    if(!m_items)
        return 0;
    return m_items->size();
}



void MGridInteractiveUnit::setRange(long start, long finish)
{
    if(!m_items)
    {
        qDebug() << "MemoryInteractiveUnit:m_items NULL";
        return;
    }
    if(start<0||finish>=memorySize())
    {
        return;
    }

    if(!m_enabled)
        m_enabled = true;
    setStart(start);
    setFinish(finish);

    rebuildShape();
}

#endif // MEMORYINTERACTIVEUNIT_CPP

