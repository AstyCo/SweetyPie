#include "memoryitem.hpp"
#include "memoryscene.hpp"
#include "globalvalues.hpp"


#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>


#include <QApplication>
#include <QDebug>

MemoryItem::MemoryItem(long index,qreal edgeLength,qreal borderWidth,QGraphicsItem *parent/* = 0*/)
    : QGraphicsLayoutItem(), QGraphicsItem(parent)
{
    setGraphicsItem(this);
    setIndex(index);

    m_parentUnit = dynamic_cast<MemoryUnit*>(parent);
        // NULL if not MemoryUnit*

    m_scene = dynamic_cast<MemoryScene*>(scene());
        // NULL if not MemoryScene*
    if(!m_scene)
        qDebug() << "not MemoryScene*";

    setEdgeLength(edgeLength);
    setBorderWidth(borderWidth);
    disableSizeModify();

//    setFlags(ItemIsSelectable);
    setAcceptsHoverEvents(true);

    enableToolTip();

}

MemoryItem::~MemoryItem()
{

}


void MemoryItem::paint(QPainter *painter,
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

void MemoryItem::drawHighlighted(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
//        fillColor = fillColor.lighter(115);
//    }

    QRectF itemRect( borderWidth(), borderWidth(), edgeLength(), edgeLength());
    QColor squareColor = fillColor;

//    const QBrush brush(fillColor);
//    qDrawShadePanel(painter,itemRect.toRect(),scene()->palette(),true,2,&brush);


    QPainterPath path;
    path.addRect(itemRect);
    painter->fillPath(path, squareColor);
    painter->drawPath(path);
}

void MemoryItem::drawBlurred(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

MemoryUnit *MemoryItem::parentUnit() const
{
    return m_parentUnit;
}

void MemoryItem::setParentUnit(MemoryUnit *parentUnit)
{
    if(!parentUnit)
        setParentItem(m_scene->widget());
    m_parentUnit = parentUnit;
}



void MemoryItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if(!m_scene)
        return QGraphicsItem::hoverEnterEvent(event);



    m_scene->setItemInfo(QString::number(index()));

//    m_scene->showInteractiveRange(index(),index()+100);
    if(m_scene->interactiveHighlight())
        m_scene->setStartHighlight(index());


    return QGraphicsItem::hoverEnterEvent(event);
}

void MemoryItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
//    if(!m_scene)
//        return QGraphicsItem::hoverLeaveEvent(event);

//    m_scene->hideInteractiveRange();
//    m_scene->clearShownUnits();
//    m_scene->update();

    return QGraphicsItem::hoverLeaveEvent(event);
}

void MemoryItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug()<<"MemoryItem::mousePressEvent";

    if(!m_scene->interactiveHighlight())
        return;

    m_scene->setInteractiveHighlight(false);

    if(QApplication::keyboardModifiers()&Qt::ShiftModifier)
        m_scene->setEmpty(index(),100);
    else if (QApplication::keyboardModifiers()&Qt::ControlModifier)
        m_scene->setWritten(index(),100);
    else if (QApplication::keyboardModifiers()&Qt::AltModifier)
        m_scene->setAvailable(index(),100);
    else
        m_scene->setRead(index(),100);


    return QGraphicsItem::mousePressEvent(event);
}

QString MemoryItem::toolTip() const
{
    return QGraphicsItem::toolTip();
}

void MemoryItem::setToolTip(const QString &toolTip)
{
    return QGraphicsItem::setToolTip(toolTip);
}

void MemoryItem::enableToolTip()
{
    setToolTip(QString("Dec: ")+fixedNumPresentation(index(),10,2047)+'\n'
               +QString("Hex: 0x")+fixedNumPresentation(index(),16,2047)+'\n'
               +QString("Bin:  ")+fixedNumPresentation(index(),2,2047));
}

void MemoryItem::disableToolTip()
{
    setToolTip(QString());
}

QVariant MemoryItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemParentHasChanged)
    {

        QGraphicsItem * newParentItem = value.value<QGraphicsItem*>();
        if(!newParentItem)
        {
            m_parentUnit = NULL;
            return QGraphicsItem::itemChange(change,value);
        }

        MemoryUnit* newParent = dynamic_cast<MemoryUnit*>(newParentItem);
        if(!newParent)
        {
            return QGraphicsItem::itemChange(change,value);
        }
        m_parentUnit = newParent;
        return QGraphicsItem::itemChange(change,value);
    }

    return QGraphicsItem::itemChange(change,value);
}

void MemoryItem::setParentUnitSelected(bool selected)
{
    if(!m_parentUnit)
    {
        return;
    }

    m_parentUnit->setShowBorder(selected);
    m_parentUnit->update();
}

MemoryState MemoryItem::state() const
{
    if(m_parentUnit)
    {
        return m_parentUnit->state();
    }

    return Memory::Empty;
}

QColor MemoryItem::color() const
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


long MemoryItem::index() const
{
    return m_index;
}

void MemoryItem::setIndex(long index)
{
    m_index = index;
}
qreal MemoryItem::edgeLength() const
{
    if(m_sizeModifying)
    {
        return m_edgeLength*m_sizeModify;
    }
    return m_edgeLength;
}

void MemoryItem::setEdgeLength(qreal edgeLength)
{
    if(m_edgeLength == edgeLength)
        return;
    m_edgeLength = edgeLength;
    updateGeometry();
}
qreal MemoryItem::borderWidth() const
{
    if(m_sizeModifying)
    {
        return m_borderWidth*m_sizeModify;
    }
    return m_borderWidth;
}

void MemoryItem::setBorderWidth(const qreal &borderWidth)
{
    if(borderWidth==m_borderWidth)
        return;
    m_borderWidth = borderWidth;
    updateGeometry();
}

void MemoryItem::disableSizeModify()
{
    m_sizeModifying = false;
    m_sizeModify = 1.0;
}

qreal MemoryItem::sizeModify() const
{
    return m_sizeModify;
}

void MemoryItem::setSizeModify(qreal sizeModify)
{
    m_sizeModifying = true;
    m_sizeModify = sizeModify;
    updateGeometry();
}

bool MemoryItem::isHighlighted() const
{
    return (!(m_scene->m_highlightMode))
            || m_scene->inHighlightRange(index());
}


QRectF MemoryItem::boundingRect() const
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

void MemoryItem::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}

//QRectF MemoryItem::geometry() const
//{
//    return QGraphicsLayoutItem::geometry();
//}

QSizeF MemoryItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
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


