
#include "mgrid_scene.h"
#include "memory_globalvalues.h"
#include "mgrid_item.h"
#include "mgrid_unit.h"


#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>


#include <QApplication>
#include <QDebug>

MGridItem::MGridItem(long index,QGraphicsScene *scene,qreal edgeLength,qreal borderWidth,QGraphicsItem *parent/* = 0*/)
    :  QGraphicsItem(parent,scene)
{
    setIndex(index);

    m_unit = dynamic_cast<MGridUnit*>(parent);
        // NULL if not MemoryUnit*

    m_scene = dynamic_cast<MGridScene*>(scene);

    setEdgeLength(edgeLength);
    setBorderWidth(borderWidth);

    setAcceptsHoverEvents(true);
    enableToolTip();

}

MGridItem::~MGridItem()
{

}

QRectF MGridItem::geometry() const
{
    return QRectF(pos(),boundingRect().size());
}

qreal MGridItem::left() const
{
    return pos().x();
}

qreal MGridItem::right() const
{
    return pos().x()+boundingRect().width();
}

qreal MGridItem::top() const
{
    return pos().y();
}

qreal MGridItem::bottom() const
{
    return pos().y()+boundingRect().height();
}




void MGridItem::paint(QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
    Q_UNUSED(widget);

    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->setPen(Qt::NoPen);

    if(isHighlighted())
        drawHighlighted(painter,option,widget);
    else
        drawBlurred(painter,option,widget);

}

void MGridItem::drawHighlighted(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QColor fillColor = (option->state & QStyle::State_Selected) ? color().dark(150) : color();


    if(borderWidth())
    {
        QRectF extSquareItem( 0, 0, edgeLength()+2*borderWidth(), edgeLength()+2*borderWidth());
        QColor extSquareColor = GLOBAL_BORDER_COLOR;
        QPainterPath externalFigure;
        externalFigure.addRect(extSquareItem);
        painter->setOpacity(0.2);
        painter->fillPath(externalFigure, extSquareColor);
        painter->drawPath(externalFigure);
        painter->setOpacity(1);
    }



    QRectF itemRect( borderWidth(), borderWidth(), edgeLength(), edgeLength());
    QColor squareColor = fillColor;

//    const QBrush brush(fillColor);
//    qDrawShadePanel(painter,itemRect.toRect(),scene()->palette(),true,2,&brush);


    QPainterPath path;
    path.addRect(itemRect);
    painter->fillPath(path, squareColor);
    painter->drawPath(path);
}

void MGridItem::drawBlurred(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);


    if(borderWidth())
    {
        QRectF extSquareItem( 0, 0, edgeLength()+2*borderWidth(), edgeLength()+2*borderWidth());
        QColor extSquareColor = GLOBAL_BORDER_COLOR;
        QPainterPath externalFigure;
        externalFigure.addRect(extSquareItem);
        painter->setOpacity(0.2);
        painter->fillPath(externalFigure, extSquareColor);
        painter->drawPath(externalFigure);
        painter->setOpacity(1);
    }


    QColor fillColor = (option->state & QStyle::State_Selected) ? color().dark(150) : color();
//    if (option->state & QStyle::State_MouseOver)
//    {
//        fillColor = fillColor.light(150);
//    }

    QRectF squareItem( borderWidth(), borderWidth(), edgeLength(), edgeLength());
    QColor squareColor = fillColor;
    QPainterPath path;
    path.addRect(squareItem);
    painter->fillPath(path, squareColor);
    painter->drawPath(path);
}

MGridUnit *MGridItem::unit() const
{
    return m_unit;
}

void MGridItem::setUnit(MGridUnit *unit)
{
    m_unit = unit;
}


QString MGridItem::toolTip() const
{
    return QGraphicsItem::toolTip();
}

void MGridItem::setToolTip(const QString &toolTip)
{
    return QGraphicsItem::setToolTip(toolTip);
}

void MGridItem::enableToolTip()
{
    setToolTip(QString("Dec: ")+fixedNumPresentation(index(),10,m_scene->memorySize())+'\n'
               +QString("Hex: 0x")+fixedNumPresentation(index(),16,m_scene->memorySize())+'\n'
               +QString("Bin:  ")+fixedNumPresentation(index(),2,m_scene->memorySize()));
}

void MGridItem::disableToolTip()
{
    setToolTip(QString());
}

void MGridItem::setUnitSelected(bool selected)
{
    if(!m_unit)
    {
        return;
    }

    m_unit->setShowBorder(selected);
    m_unit->update();
}

MemoryState MGridItem::state() const
{
    if(m_unit)
    {
        return m_unit->state();
    }

    return Memory::Empty;
}

QColor MGridItem::color() const
{
    bool highlightedItem = isHighlighted();

    if(m_unit)
        return MemoryState_to_QColor(m_unit->state(),highlightedItem);

    return MemoryState_to_QColor(Memory::Empty,highlightedItem);
}

//int MemoryItem::parentUnitId() const
//{
//    if(m_parentUnit)
//    {
//        return m_parentUnit->unitId();
//    }

//    return 0;
//}

//long MemoryItem::parentUnitStart() const
//{
//    if(!m_parentUnit)
//        return -1;
//    return m_parentUnit->start();
//}

//long MemoryItem::parentUnitFinish() const
//{
//    if(!m_parentUnit)
//        return -1;
//    return m_parentUnit->finish();
//}


long MGridItem::index() const
{
    return m_index;
}

void MGridItem::setIndex(long index)
{
    m_index = index;
}
qreal MGridItem::edgeLength() const
{
    return m_edgeLength;
}

void MGridItem::setEdgeLength(qreal edgeLength)
{
    if(m_edgeLength == edgeLength)
        return;
    prepareGeometryChange();
    m_edgeLength = edgeLength;
}
qreal MGridItem::borderWidth() const
{
    return m_borderWidth;
}

void MGridItem::setBorderWidth(const qreal &borderWidth)
{
    if(borderWidth==m_borderWidth)
        return;
    prepareGeometryChange();
    m_borderWidth = borderWidth;
}

bool MGridItem::isHighlighted() const
{
    return m_scene->inHighlightRange(index());
}


QRectF MGridItem::boundingRect() const
{
    return QRectF( 0, 0, edgeLength() + 2*borderWidth(), edgeLength() + 2*borderWidth());
    //    return QRectF(QPointF(0,0), geometry().size());
}

//QPainterPath MemoryItem::shape() const
//{
//    QPainterPath path;
//    path.addRect(boundingRect());
// //    path.addRect(borderWidth(),borderWidth(),edgeLength(),edgeLength());
//    return path;
//}

//void MGridItem::setGeometry(const QRectF &geom)
//{
//    prepareGeometryChange();
//    QGraphicsLayoutItem::setGeometry(geom);
//    setPos(geom.topLeft());
//}

//QRectF MemoryItem::geometry() const
//{
//    return QGraphicsLayoutItem::geometry();
//}

//QSizeF MGridItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
//{
//    switch (which) {
//    case Qt::MinimumSize:
//    case Qt::PreferredSize:
//    case Qt::MaximumSize:
//        return QSizeF(edgeLength() + 2*borderWidth(),edgeLength() + 2*borderWidth());
//    default:
//        break;
//    }
//    return constraint;
//}


