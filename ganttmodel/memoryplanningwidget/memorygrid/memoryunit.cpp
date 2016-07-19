#include "memoryunit.hpp"
#include "memoryscene.hpp"

#include "kamemorypart.h"

#include "globalvalues.hpp"

#include <QPainterPath>
#include <QPainter>

#include <QDebug>

extern MemoryScene* mem_scene;

MemoryUnit::MemoryUnit(QGraphicsItem *parent /*= 0*/)
    : QGraphicsItem(parent)
{
//    setFlag(QGraphicsItem::ItemHasNoContents);

    rand = qrand()%2;
    isEmpty = true;

    m_scene = dynamic_cast<MemoryScene*>(scene());
    setItems();
    if(!m_scene)
        qDebug() << "Not MemoryScene*";

    setShowBorder(false);
    setAcceptsHoverEvents(true);

    setZValue(1);

    m_borderPen=QPen(QBrush(/*rand?Qt::red:Qt::blue*/QColor::fromRgb(200,200,200)/*.lighter())*/), extraSize() ,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);

}

QRectF MemoryUnit::boundingRect() const
{
    return m_shapeBorder.boundingRect().adjusted(-extraSize(),-extraSize(),extraSize(),extraSize());
}

QPainterPath MemoryUnit::shape() const
{
    return m_shapeBorder;
}

void MemoryUnit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(!showBorder())
        return;


    painter->setCompositionMode(QPainter::CompositionMode_Multiply);

    painter->setPen(m_borderPen);
    painter->setOpacity(1);
    painter->drawPath(m_shapeBorder);

}

int MemoryUnit::unitId() const
{
    return m_unitId;
}

void MemoryUnit::setUnitId(int unitId)
{
    m_unitId = unitId;
}

Memory::MemoryState MemoryUnit::state() const
{
    return m_state;
}

void MemoryUnit::setState(const Memory::MemoryState &status)
{
    m_state = status;
}

long MemoryUnit::start() const
{
    return m_start;
}

void MemoryUnit::setStart(long start)
{
    m_start = start;
}

long MemoryUnit::finish() const
{
    return m_finish;
}

void MemoryUnit::setFinish(long finish)
{
    m_finish = finish;
}

long MemoryUnit::size() const
{
    return m_finish-m_start + 1;
}

void MemoryUnit::setSize(long newSize)
{
    setFinish(start()+newSize-1);
}

qreal MemoryUnit::spacing() const
{
    MemoryScene* p_memScene = dynamic_cast<MemoryScene*>(scene());
    if(!p_memScene)
        return DEFAULT_SPACING;
    return p_memScene->spacing();
}

QVariant MemoryUnit::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
//    MemoryItem* p_mem = dynamic_cast<MemoryItem*>(value.value<QGraphicsItem*>());
//    if(!p_mem)
//    {
//        return QGraphicsItem::itemChange(change,value);
//    }

//    if(change == QGraphicsItem::ItemChildAddedChange)
//    {
//        if(m_unitId)
//        {
////            qDebug() << "append to "+QString::number(m_unitId);
//        }
//        m_items.append(p_mem);
//        update();
//    }
//    else if (change == QGraphicsItem::ItemChildRemovedChange)
//    {
//        if(m_unitId)
//        {
////            qDebug() << "remove from "+QString::number(m_unitId);
//        }
//        m_items.removeOne(p_mem);
//        update();
//    }

//    rebuildShape();

    return QGraphicsItem::itemChange(change,value); // MAYBE change to QVariant()
}

void MemoryUnit::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if(!m_scene)
        return;
    setShowBorder(true);

    m_scene->setUnitInfo(QString(   QObject::tr("Unit Group Id: ")
                                    +QString::number(unitId())
                                    +QString(QObject::tr("  Unit State: "))
                                    +state()
                                    +QObject::tr(" Unit Memory: ")
                                    +"0x"+fixedNumPresentation(start(),16,2047)
                                    +" - 0x"+fixedNumPresentation(finish(),16,2047)
                         ));

    setZValue(100);

    update();

    return QGraphicsItem::hoverEnterEvent(event);
}

void MemoryUnit::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setZValue(0);
    setShowBorder(false);
    return QGraphicsItem::hoverLeaveEvent(event);
}

void MemoryUnit::rebuildShape()
{
    if(isEmpty)
    {
        setShapeBorder(QPainterPath());
        return ;
    }

    QPainterPath itemsPath;

    for(int i = m_start; i<= m_finish; ++i)
    {
        itemsPath.addRect(m_items->at(i)->geometry());
    }

    qreal   top = itemsPath.boundingRect().top(),
            bottom = itemsPath.boundingRect().bottom(),

            utterLeft = itemsPath.boundingRect().right(), // using for search of utter left, right() is MAX
            utterRight = itemsPath.boundingRect().left(); // -||- right, left() is MAX


    MemoryItem  *utterLeftItem = NULL,
                *utterRightItem = NULL;

    for(int i = m_start; i<= m_finish; ++i)
    {
        MemoryItem* item = m_items->at(i);
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

    if(!m_scene)
    {
        return ;
    }

    qreal halfSpacing = m_scene->spacing() / 2;

    bool shapingTop = false,
         shapingBottom = false,
         shapingSeparate = false;
    if(utterLeftItem->geometry().left()!=itemsPath.boundingRect().left())
        shapingTop = true;
    if(utterRightItem->geometry().right()!=itemsPath.boundingRect().right())
        shapingBottom = true;
    if(size()<m_scene->itemPerRow()
            && utterLeft>utterRight)
        shapingSeparate = true;

    path.moveTo(utterLeftItem->geometry().topLeft()+QPointF(-halfSpacing,-halfSpacing));
    path.lineTo(itemsPath.boundingRect().topRight()+QPointF(+halfSpacing,-halfSpacing));

    if(shapingBottom)
    {
        path.lineTo(QPointF(itemsPath.boundingRect().right()+halfSpacing,utterRightItem->geometry().top()-halfSpacing));
        if(shapingSeparate)
        {
            path.lineTo(utterLeftItem->geometry().bottomLeft()+QPointF(-halfSpacing,+halfSpacing));
            path.moveTo(utterRightItem->geometry().topRight()+QPointF(+halfSpacing,-halfSpacing));
        }
        else
            path.lineTo(utterRightItem->geometry().topRight()+QPointF(+halfSpacing,-halfSpacing));
    }

    path.lineTo(utterRightItem->geometry().bottomRight()+QPointF(+halfSpacing,+halfSpacing));
    path.lineTo(itemsPath.boundingRect().bottomLeft()+QPointF(-halfSpacing,+halfSpacing));

    if(shapingTop)
    {
        path.lineTo(QPointF(itemsPath.boundingRect().left()-halfSpacing,utterLeftItem->geometry().bottom()+halfSpacing));
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

QPainterPath MemoryUnit::shapeBorder() const
{
    return m_shapeBorder;
}

void MemoryUnit::setShapeBorder(const QPainterPath &shapeBorder)
{
    m_shapeBorder = shapeBorder;
}

void MemoryUnit::setItems()
{
    m_items = &(m_scene->m_items);
}

bool MemoryUnit::showBorder() const
{
    return m_unitSelected;
}

void MemoryUnit::setShowBorder(bool unitSelected)
{
    m_unitSelected = unitSelected;
}

QColor MemoryUnit::color() const
{
    return MemoryState_to_QColor(m_state);
}

void MemoryUnit::addItems(long start, long finish)
{
    if(!m_items)
        return;
    if(start<0 || finish>=m_scene->memorySize() || start>finish)
        return;
    isEmpty = false;

    setStart(start);
    setFinish(finish);

    updateParenthesis();
}

long MemoryUnit::removeItems(long from, long count)
{
    if(!m_scene)
        return 0;

    if(from+count-1>m_finish)
        count = m_finish - from + 1;


    if(from>m_start && from+count-1<m_finish)
    {
        MemoryUnit* p_memUnit1 = m_scene->newUnit(); // first -to- from-1
        p_memUnit1->setState(m_state);
        p_memUnit1->addItems(m_start,from-1);

        MemoryUnit* p_memUnit2 = m_scene->newUnit(); // from+count-1   -to- last
        p_memUnit2->setState(m_state);
        p_memUnit2->addItems(from+count,m_finish);

        m_scene->addUnit(p_memUnit1);
        m_scene->addUnit(p_memUnit2);

        for(int i = 0, j = from; i<count; ++i,++j)
        {
            m_items->at(j)->setParentUnit(NULL);
        }

        m_scene->removeUnit(this);
        return count;
    }

    while(from<m_start)
    {
        from++;
        if(count--<=0)
            return 0;
    }



    for(int i = 0, j = from; i<count; ++i,++j)
    {
        m_items->at(j)->setParentUnit(NULL);
    }

    if(from!=m_start)
        setFinish(from-1);
    else if(from+count<=m_finish)
        setStart(from+count);
    else
    {
        m_scene->removeUnit(this);
    }
    rebuildShape();

    return count;
}

void MemoryUnit::updateParenthesis()
{
    if(isEmpty)
        return;
    for(int i = m_start; i <= m_finish; ++i)
    {
        m_items->at(i)->setParentItem(this);
    }
    rebuildShape();
}

qreal MemoryUnit::extraSize() const
{
    return spacing()/2 + m_scene->itemBorder();
}


