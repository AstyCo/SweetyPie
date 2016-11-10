#include "mgrid_unit.h"
#include "mgrid_scene.h"
#include "mgrid_item.h"
#include "memoryview.h"

#include "memory_globalvalues.h"

#include <QPainterPath>
#include <QPainter>

#include <QDebug>

extern MGridScene* mem_scene;

//MGridUnit::MGridUnit(QGraphicsScene *scene, QGraphicsItem *parent)
//    :QGraphicsItem(parent,scene)
//{
//    initialize();
//    setScene(dynamic_cast<MGridScene*>(scene));
//}

MGridUnit::MGridUnit(QSharedPointer<KaMemoryPart> memoryPart,QGraphicsScene *scene,QGraphicsItem *parent /*= 0*/)
    :QGraphicsObject(parent)
{
    initialize();

    setScene(dynamic_cast<MGridScene*>(scene));
    setKaMemoryPart(memoryPart);
}

void MGridUnit::initialize()
{
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
    m_scene->addItem(this);
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

    if(!m_scene || !m_unitSelected)
        return;

//    qDebug() << "paint" << QString::number(zValue()) << m_shapeBorder;
//    qDebug() << "size  items "<< QString::number(size()) +" " + QString::number(m_scene->itemPerRow());
//    qDebug() << "size  items "<<QString::number(m_items->at(start())->left())+' '+QString::number(m_items->at(finish())->right());

    painter->setPen(m_borderPen);
    painter->drawPath(m_shapeBorder);

}

int MGridUnit::id() const
{
    Q_ASSERT(!m_kaMemoryPart.isNull());
    return m_kaMemoryPart->id();
}

void MGridUnit::setId(int unitId)
{
    Q_ASSERT(!m_kaMemoryPart.isNull());
    m_kaMemoryPart->setId(unitId);
}

KaMemoryPart::KaMemoryState MGridUnit::state() const
{
    Q_ASSERT(!m_kaMemoryPart.isNull());
    return m_kaMemoryPart->state();
}

void MGridUnit::setState(const KaMemoryPart::KaMemoryState &status)
{
    Q_ASSERT(!m_kaMemoryPart.isNull());
    m_kaMemoryPart->setState(status);
}

long MGridUnit::start() const
{
    Q_ASSERT(!m_kaMemoryPart.isNull());
    return m_kaMemoryPart->start();
}

void MGridUnit::setStart(long start)
{
    Q_ASSERT(!m_kaMemoryPart.isNull());
    if(start<0)
    {
        start=0;
    }
    m_kaMemoryPart->setStart(start);
}

long MGridUnit::length() const
{
    Q_ASSERT(!m_kaMemoryPart.isNull());
    return m_kaMemoryPart->length();
}

void MGridUnit::setLength(long newSize)
{
    Q_ASSERT(!m_kaMemoryPart.isNull());
    m_kaMemoryPart->setLength(newSize);
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

    emit hoverEnter();
    return QGraphicsItem::hoverEnterEvent(event);
}

void MGridUnit::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setZValue(0);
    setShowBorder(false);

    emit hoverLeave();
    return QGraphicsItem::hoverLeaveEvent(event);
}


void MGridUnit::rebuildShape()
{
    if(!m_items)
        return;
    if(length()==0)
    {
        setShapeBorder(QPainterPath());
        return;
    }

    if(start()+length()>m_items->size())
    {
        qDebug() << "MemoryInteractiveUnit::rebuildShape() out of range";
        return;
    }

    QRectF itemsRect;

    for(int i = start(); i <start()+length(); ++i)
    {
        itemsRect|=m_items->at(i)->geometry();
    }

    MGridItem  *utterLeftItem = m_items->at(start()),
                *utterRightItem = m_items->at(start()+length()-1);
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

QPainterPath MGridUnit::shapeBorder() const
{
    return m_shapeBorder;
}

void MGridUnit::setShapeBorder(const QPainterPath &shapeBorder)
{
    prepareGeometryChange();
    m_shapeBorder = shapeBorder;
}

QSharedPointer<KaMemoryPart> MGridUnit::kaMemoryPart() const
{
    return m_kaMemoryPart;
}

void MGridUnit::setKaMemoryPart(const QSharedPointer<KaMemoryPart> &kaMemoryPart)
{
    m_kaMemoryPart = kaMemoryPart;
    if(m_kaMemoryPart.isNull() || m_scene == NULL)
        return;
    addItems(m_kaMemoryPart->start(),m_kaMemoryPart->length());
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
    return MemoryState_to_QColor(state(),true);
}

void MGridUnit::addItems(long start, long length)
{
    if(!m_items)
        return;
    if(start<0 || length==0 ||  start+length>m_scene->memorySize())
        return;

    setStart(start);
    setLength(length);

    updateParenthesis();
}

long MGridUnit::removeItems(long from, long count)
{
    if(!m_scene||!m_items)
        return 0;
    if(from+count<=start())
        return 0;
    if(count>length())
        count = length();


    if(from>start() && count<length())
    {
        MGridUnit* p_memUnit1 = m_scene->newUnit(); // first -to- from-1
        p_memUnit1->setState(state());
        p_memUnit1->addItems(start(),from-start());

        MGridUnit* p_memUnit2 = m_scene->newUnit(); // from+count-1   -to- last
        p_memUnit2->setState(state());
        p_memUnit2->addItems(from+count,start()+length()-from-count);

        m_scene->addUnit(p_memUnit1);
        m_scene->addUnit(p_memUnit2);

        for(int i = 0, j = from; i<count; ++i,++j)
        {
            m_items->at(j)->setUnit(NULL);
        }

        m_scene->removeUnit(this);
        return count;
    }

    while(from<start())
    {
        from++;
        if(count--<=0)
            return 0;
    }



    for(int i = 0, j = from; i<count; ++i,++j)
    {
        m_items->at(j)->setUnit(NULL);
    }

    if(from!=start())
        setLength(from-start());
    else if(from+count<start()+length())
    {
        setLength(length()-count);
        setStart(from+count);
    }
    else
    {
        m_scene->removeUnit(this);
    }
    rebuildShape();

    return count;
}

void MGridUnit::updateParenthesis()
{
    if(!m_items)
        return;
    for(int i = start(); i < start()+length(); ++i)
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


