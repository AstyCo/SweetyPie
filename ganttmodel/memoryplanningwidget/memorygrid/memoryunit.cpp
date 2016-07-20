#include "mgrid_unit.h"
#include "mgrid_scene.h"

#include "kamemorypart.h"

#include "memory_globalvalues.h"

#include <QPainterPath>
#include <QPainter>

#include <QDebug>

extern MGridScene* mem_scene;

MGridUnit::MGridUnit(QGraphicsItem *parent /*= 0*/)
    : QGraphicsItem(parent)
{
//    setFlag(QGraphicsItem::ItemHasNoContents);

    rand = qrand()%2;
    isEmpty = true;

    m_scene = dynamic_cast<MGridScene*>(scene());
    setItems();
    if(!m_scene)
        qDebug() << "Not MemoryScene*";

    setShowBorder(false);
    setAcceptsHoverEvents(true);

    setZValue(1);

    m_borderPen=QPen(QBrush(/*rand?Qt::red:Qt::blue*/QColor::fromRgb(200,200,200)/*.lighter())*/), extraSize() ,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);

}

QRectF MGridUnit::boundingRect() const
{
    return m_shapeBorder.boundingRect().adjusted(-extraSize(),-extraSize(),extraSize(),extraSize());
}

QPainterPath MGridUnit::shape() const
{
    return m_shapeBorder;
}

void MGridUnit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

int MGridUnit::unitId() const
{
    return m_unitId;
}

void MGridUnit::setUnitId(int unitId)
{
    m_unitId = unitId;
}

Memory::MemoryState MGridUnit::state() const
{
    return m_state;
}

void MGridUnit::setState(const Memory::MemoryState &status)
{
    m_state = status;
}

long MGridUnit::start() const
{
    return m_start;
}

void MGridUnit::setStart(long start)
{
    m_start = start;
}

long MGridUnit::finish() const
{
    return m_finish;
}

void MGridUnit::setFinish(long finish)
{
    m_finish = finish;
}

long MGridUnit::size() const
{
    return m_finish-m_start + 1;
}

void MGridUnit::setSize(long newSize)
{
    setFinish(start()+newSize-1);
}

qreal MGridUnit::spacing() const
{
    MGridScene* p_memScene = dynamic_cast<MGridScene*>(scene());
    if(!p_memScene)
        return DEFAULT_SPACING;
    return p_memScene->spacing();
}

QVariant MGridUnit::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
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

void MGridUnit::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
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

void MGridUnit::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setZValue(0);
    setShowBorder(false);
    return QGraphicsItem::hoverLeaveEvent(event);
}

void MGridUnit::rebuildShape()
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


    MGridtem  *utterLeftItem = NULL,
                *utterRightItem = NULL;

    for(int i = m_start; i<= m_finish; ++i)
    {
        MGridtem* item = m_items->at(i);
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

QPainterPath MGridUnit::shapeBorder() const
{
    return m_shapeBorder;
}

void MGridUnit::setShapeBorder(const QPainterPath &shapeBorder)
{
    m_shapeBorder = shapeBorder;
}

void MGridUnit::setItems()
{
    m_items = &(m_scene->m_items);
}

bool MGridUnit::showBorder() const
{
    return m_unitSelected;
}

void MGridUnit::setShowBorder(bool unitSelected)
{
    m_unitSelected = unitSelected;
}

QColor MGridUnit::color() const
{
    return MemoryState_to_QColor(m_state);
}

void MGridUnit::addItems(long start, long finish)
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

long MGridUnit::removeItems(long from, long count)
{
    if(!m_scene)
        return 0;

    if(from+count-1>m_finish)
        count = m_finish - from + 1;


    if(from>m_start && from+count-1<m_finish)
    {
        MGridUnit* p_memUnit1 = m_scene->newUnit(); // first -to- from-1
        p_memUnit1->setState(m_state);
        p_memUnit1->addItems(m_start,from-1);

        MGridUnit* p_memUnit2 = m_scene->newUnit(); // from+count-1   -to- last
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

void MGridUnit::updateParenthesis()
{
    if(isEmpty)
        return;
    for(int i = m_start; i <= m_finish; ++i)
    {
        m_items->at(i)->setParentItem(this);
    }
    rebuildShape();
}

qreal MGridUnit::extraSize() const
{
    return spacing()/2 + m_scene->itemBorder();
}


