
#include "mgrid_scene.h"
#include "memory_globalvalues.h"
#include "mgrid_item.h"
#include "mgrid_widget.h"
#include "mgrid_unit.h"


#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>


#include <QApplication>
#include <QDebug>

MGridItem::MGridItem(long index,qreal edgeLength,qreal borderWidth,QGraphicsItem *parent/* = 0*/)
    : QGraphicsLayoutItem(), QGraphicsItem(parent)
{
    setGraphicsItem(this);
    setIndex(index);

    m_parentUnit = dynamic_cast<MGridUnit*>(parent);
        // NULL if not MemoryUnit*

    m_scene = dynamic_cast<MGridScene*>(scene());
        // NULL if not MemoryScene*
    if(!m_scene)
        qDebug() << "not MemoryScene*";

    setEdgeLength(edgeLength);
    setBorderWidth(borderWidth);
    disableSizeModify();

    setAcceptsHoverEvents(true);

    enableToolTip();

}

MGridItem::~MGridItem()
{

}


void MGridItem::paint(QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
    Q_UNUSED(widget);

    if(zValue())
        qDebug() <<zValue();

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

MGridUnit *MGridItem::parentUnit() const
{
    return m_parentUnit;
}

void MGridItem::setParentUnit(MGridUnit *parentUnit)
{
    if(!parentUnit)
        setParentItem(m_scene->widget());
    m_parentUnit = parentUnit;
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
    setToolTip(QString("Dec: ")+fixedNumPresentation(index(),10,2047)+'\n'
               +QString("Hex: 0x")+fixedNumPresentation(index(),16,2047)+'\n'
               +QString("Bin:  ")+fixedNumPresentation(index(),2,2047));
}

void MGridItem::disableToolTip()
{
    setToolTip(QString());
}

QVariant MGridItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemParentHasChanged)
    {

        QGraphicsItem * newParentItem = value.value<QGraphicsItem*>();
        if(!newParentItem)
        {
            m_parentUnit = NULL;
            return QGraphicsItem::itemChange(change,value);
        }

        MGridUnit* newParent = dynamic_cast<MGridUnit*>(newParentItem);
        if(!newParent)
        {
            return QGraphicsItem::itemChange(change,value);
        }
        m_parentUnit = newParent;
        return QGraphicsItem::itemChange(change,value);
    }

    return QGraphicsItem::itemChange(change,value);
}

void MGridItem::setParentUnitSelected(bool selected)
{
    if(!m_parentUnit)
    {
        return;
    }

    m_parentUnit->setShowBorder(selected);
    m_parentUnit->update();
}

MemoryState MGridItem::state() const
{
    if(m_parentUnit)
    {
        return m_parentUnit->state();
    }

    return Memory::Empty;
}

QColor MGridItem::color() const
{
    bool highlightedItem = isHighlighted();

    if(m_parentUnit)
        return MemoryState_to_QColor(m_parentUnit->state(),highlightedItem);

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
    if(m_sizeModifying)
    {
        return m_edgeLength*m_sizeModify;
    }
    return m_edgeLength;
}

void MGridItem::setEdgeLength(qreal edgeLength)
{
    if(m_edgeLength == edgeLength)
        return;
    m_edgeLength = edgeLength;
    updateGeometry();
}
qreal MGridItem::borderWidth() const
{
    if(m_sizeModifying)
    {
        return m_borderWidth*m_sizeModify;
    }
    return m_borderWidth;
}

void MGridItem::setBorderWidth(const qreal &borderWidth)
{
    if(borderWidth==m_borderWidth)
        return;
    m_borderWidth = borderWidth;
    updateGeometry();
}

void MGridItem::disableSizeModify()
{
    m_sizeModifying = false;
    m_sizeModify = 1.0;
}

qreal MGridItem::sizeModify() const
{
    return m_sizeModify;
}

void MGridItem::setSizeModify(qreal sizeModify)
{
    m_sizeModifying = true;
    m_sizeModify = sizeModify;
    updateGeometry();
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

void MGridItem::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}

//QRectF MemoryItem::geometry() const
//{
//    return QGraphicsLayoutItem::geometry();
//}

QSizeF MGridItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
    case Qt::MaximumSize:
        return QSizeF(edgeLength() + 2*borderWidth(),edgeLength() + 2*borderWidth());
    default:
        break;
    }
    return constraint;
}


