#include "mgrid_unit.h"
#include "mgrid_scene.h"
#include "mgrid_item.h"
#include "memoryview.h"

#include "memory_globalvalues.h"

#include <QPainterPath>
#include <QPainter>

#include <QDebug>

extern MGridScene* mem_scene;

MGridUnit::MGridUnit(QGraphicsScene *scene,QGraphicsItem *parent /*= 0*/)
    : QGraphicsItem(parent,scene)
{
    isEmpty = true;


    setScene(dynamic_cast<MGridScene*>(scene));

    setShowBorder(false);
    setAcceptsHoverEvents(true);

    setZValue(1);
}

void MGridUnit::setScene(MGridScene* scene)
{
    m_scene = scene;
    if(!m_scene)
    {
        qCritical("not MGridScene :: setScene");
        return;
    }
    m_items = &(m_scene->m_items);

    m_borderPen=QPen(QBrush(QColor(80,80,80)), extraSize() ,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);

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

//    qDebug() << "paint" << QString::number(zValue()) << m_shapeBorder;
//    qDebug() << "size  items "<< QString::number(size()) +" " + QString::number(m_scene->itemPerRow());
//    qDebug() << "size  items "<<QString::number(m_items->at(m_start)->left())+' '+QString::number(m_items->at(m_finish)->right());

    painter->setPen(m_borderPen);
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

MemoryPart::MemoryState MGridUnit::state() const
{
    return m_state;
}

void MGridUnit::setState(const MemoryPart::MemoryState &status)
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

//qreal MGridUnit::spacing() const
//{
//    MGridScene* p_memScene = dynamic_cast<MGridScene*>(scene());
//    if(!p_memScene)
//        return DEFAULT_SPACING;
//    return p_memScene->spacing();
//}

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

    qDebug() << "MGridUnit::rebuildShape";
    QRectF itemsRect;

    for(int i = m_start; i <= m_finish; ++i)
    {
        itemsRect|=m_items->at(i)->geometry();
    }

    MGridItem  *utterLeftItem = m_items->at(m_start),
                *utterRightItem = m_items->at(m_finish);
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

QColor MGridUnit::color()
{
    return MemoryState_to_QColor(m_state,true);
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
    if(!m_scene)
        return 0;
    return m_scene->itemBorder();
}

MemoryPart MGridUnit::toKaMemoryPart() const
{
    return MemoryPart(start(),finish(),state(),id());
}


