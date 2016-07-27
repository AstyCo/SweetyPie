#include "mgrid_unit.h"
#include "mgrid_scene.h"
#include "mgrid_item.h"


#include "kamemorypart.h"

#include "memory_globalvalues.h"

#include <QPainterPath>
#include <QPainter>

#include <QDebug>

extern MGridScene* mem_scene;

MGridUnit::MGridUnit(QGraphicsItem *parent /*= 0*/)
    : QGraphicsItem(parent)
{
    isEmpty = true;

    m_scene = dynamic_cast<MGridScene*>(scene());
    m_items = &(m_scene->m_items);
    if(!m_scene)
        qDebug() << "Not MemoryScene*";

    setShowBorder(false);
    setAcceptsHoverEvents(true);

    setZValue(1);

    m_borderPen=QPen(QBrush(QColor::fromRgb(200,200,200)), extraSize() ,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);

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

    if(!m_scene->isMouseOverUnit(this))
        return;

    painter->setCompositionMode(QPainter::CompositionMode_Multiply);

    painter->setPen(m_borderPen);
    painter->setOpacity(1);
    painter->drawPath(m_shapeBorder);

}

int MGridUnit::id() const
{
    return m_unitId;
}

void MGridUnit::setId(int unitId)
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
    if(isEmpty)
        return;
    if(start<0)
    {
        m_start = 0;
        return;
    }
    m_start = start;
}

long MGridUnit::finish() const
{
    return m_finish;
}

void MGridUnit::setFinish(long finish)
{
    if(isEmpty)
        return;
    if(finish >= m_scene->memorySize())
    {
        m_finish = m_scene->memorySize()-1;
        return;
    }

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

void MGridUnit::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if(!m_scene)
        return;
    setShowBorder(true);

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

    qreal   top = itemsRect.top(),
            bottom = itemsRect.bottom(),

            utterLeft = itemsRect.right(), // using for search of utter left, right() is MAX
            utterRight = itemsRect.left(); // -||- right, left() is MAX


    MGridItem  *utterLeftItem = m_items->at(m_start),
                *utterRightItem = m_items->at(m_finish);
    utterLeft = utterLeftItem->left();
    utterRight = utterLeftItem->right();

    QPainterPath path;

    qreal halfSpacing = m_scene->spacing() / 2;

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

    path.moveTo(utterLeftItem->geometry().topLeft()+QPointF(-halfSpacing,-halfSpacing));
    path.lineTo(itemsRect.topRight()+QPointF(+halfSpacing,-halfSpacing));

    if(shapingBottom)
    {
        path.lineTo(QPointF(itemsRect.right()+halfSpacing,utterRightItem->top()-halfSpacing));
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
        path.lineTo(QPointF(itemsRect.left()-halfSpacing,utterLeftItem->bottom()+halfSpacing));
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
    prepareGeometryChange();
    m_shapeBorder = shapeBorder;
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
    if(from+count<=m_start)
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
            m_items->at(j)->setUnit(NULL);
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
        m_items->at(j)->setUnit(NULL);
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
        m_items->at(i)->setUnit(this);
    }
    rebuildShape();
}

qreal MGridUnit::extraSize() const
{
    return spacing()/2 + m_scene->itemBorder();
}

KaMemoryPart MGridUnit::toKaMemoryPart() const
{
    return KaMemoryPart(start(),finish(),state(),id());
}


