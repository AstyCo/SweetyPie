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
    : QGraphicsItem(parent)
{
    if(!scene)
    {
        qDebug() << "MemoryInteractiveUnit no scene";
    }

    m_initialized = false;

    setAcceptsHoverEvents(false);
    setShowBorders(false);

    m_items = &(scene->m_items);

    m_borderPen=QPen(QBrush(Qt::darkRed), spacing()+5 ,Qt::SolidLine ,Qt::SquareCap,Qt::MiterJoin);

}

QRectF MGridInteractiveUnit::boundingRect() const
{
    return m_shapeBorder.boundingRect().adjusted(-spacing()*1.5,-spacing()*1.5,spacing()*1.5,spacing()*1.5);
}

QPainterPath MGridInteractiveUnit::shape() const
{
    return m_shapeBorder;
}

void MGridInteractiveUnit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(!m_showBorders)
        return;



//    QPen pen(QBrush(Qt::blue),DEFAULT_SPACING ,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
    painter->setPen(m_borderPen);
    painter->setOpacity(1);
    painter->drawPath(m_shapeBorder);
}

bool MGridInteractiveUnit::inRange(long index) const
{
    return m_initialized&&(index>=m_start && index <=m_finish);
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

qreal MGridInteractiveUnit::spacing() const
{
    MGridScene* p_memScene = dynamic_cast<MGridScene*>(scene());
    if(!p_memScene)
        return DEFAULT_SPACING;
    return p_memScene->spacing();
}


void MGridInteractiveUnit::rebuildShape()
{
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

    qreal   top = itemsRect.top(),
            bottom = itemsRect.bottom(),

            utterLeft = itemsRect.right(), // using for search of utter left, right() is MAX
            utterRight = itemsRect.left(); // -||- right, left() is MAX


    MGridItem  *utterLeftItem = NULL,
                *utterRightItem = NULL;

    for(int i = m_start; i <= m_finish; ++i)
    {
        MGridItem* item = m_items->at(i);
        qreal   itemTop = item->geometry().top(),
                itemBottom = item->geometry().bottom();

        if(itemTop == top)
        {
            // element of first row
            qreal itemLeft = item->geometry().left();

            if(itemLeft < utterLeft)
            {
                utterLeft = itemLeft;
                utterLeftItem = item;
            }
        }
        if(itemBottom == bottom)
        {
            // element of first row
            qreal itemRight = item->geometry().right();

            if(itemRight > utterRight)
            {
                utterRight = itemRight;
                utterRightItem = item;
            }
        }
    }

    QPainterPath path;
    if(!utterLeftItem||!utterRightItem)
    {
        setShapeBorder(QPainterPath());
        return;
    }

    MGridScene* p_memScene = dynamic_cast<MGridScene*>(scene());
    if(!p_memScene)
    {
        qDebug() << "not MemoryScene";
        return ;
    }

    qreal halfSpacing = p_memScene->spacing() / 2;

    bool shapingTop = false,
         shapingBottom = false,
         shapingSeparate = false;
    if(utterLeftItem->geometry().left()!=itemsRect.left())
        shapingTop = true;
    if(utterRightItem->geometry().right()!=itemsRect.right())
        shapingBottom = true;
    if(size()<p_memScene->itemPerRow()
            && utterLeft>utterRight)
        shapingSeparate = true;

    path.moveTo(utterLeftItem->geometry().topLeft()+QPointF(-halfSpacing,-halfSpacing));
    path.lineTo(itemsRect.topRight()+QPointF(+halfSpacing,-halfSpacing));

    if(shapingBottom)
    {
        path.lineTo(QPointF(itemsRect.right()+halfSpacing,utterRightItem->geometry().top()-halfSpacing));
        if(shapingSeparate)
        {
            path.lineTo(utterLeftItem->geometry().bottomLeft()+QPointF(-halfSpacing,+halfSpacing));
            path.moveTo(utterRightItem->geometry().topRight()+QPointF(+halfSpacing,-halfSpacing));
        }
        else
            path.lineTo(utterRightItem->geometry().topRight()+QPointF(+halfSpacing,-halfSpacing));
    }

    path.lineTo(utterRightItem->geometry().bottomRight()+QPointF(+halfSpacing,+halfSpacing));
    path.lineTo(itemsRect.bottomLeft()+QPointF(-halfSpacing,+halfSpacing));

    if(shapingTop)
    {
        path.lineTo(QPointF(itemsRect.left()-halfSpacing,utterLeftItem->geometry().bottom()+halfSpacing));
        if(shapingSeparate)
        {
            path.lineTo(utterRightItem->geometry().topRight()+QPointF(+halfSpacing,-halfSpacing));
            path.moveTo(utterLeftItem->geometry().bottomLeft()+QPointF(-halfSpacing,+halfSpacing));
        }
        else
            path.lineTo(utterLeftItem->geometry().bottomLeft()+QPointF(-halfSpacing,+halfSpacing));
    }

    path.lineTo(utterLeftItem->geometry().topLeft()+QPointF(-halfSpacing,-halfSpacing));

    setShapeBorder(path);
}

QPainterPath MGridInteractiveUnit::shapeBorder() const
{
    return m_shapeBorder;
}

void MGridInteractiveUnit::setShapeBorder(const QPainterPath &shapeBorder)
{
    m_shapeBorder = shapeBorder;
}

void MGridInteractiveUnit::setShowBorders(bool value)
{
    m_showBorders = value;
    update();
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
        setShowBorders(false);
        return;
    }
    setStart(start);
    setFinish(finish);

    setShowBorders(true);

    rebuildShape();
}

#endif // MEMORYINTERACTIVEUNIT_CPP

