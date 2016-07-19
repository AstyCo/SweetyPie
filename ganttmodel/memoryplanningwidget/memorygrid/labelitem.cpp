
#include "labelitem.hpp"
#include "globalvalues.hpp"

#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

#include <QGraphicsScene>
#include <QFont>

#include <QDebug>

LabelItem::LabelItem(const QString& label,
                     QGraphicsItem *parent/* = 0*/,
                     qreal edgeLength /*= DEFAULT_EDGELENGTH*/,
                     qreal borderWidth /*= DEFAULT_BORDERWIDTH*/,
                     const QColor& color)
    : QGraphicsLayoutItem(), QGraphicsItem(parent)
{
    setGraphicsItem(this);
    setEdgeLength(edgeLength);
    setBorderWidth(borderWidth);
    setColor(color);
    setLabel(label);
}

LabelItem::~LabelItem()
{

}


void LabelItem::paint(QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);




    QRectF squareItem( m_borderWidth, m_borderWidth, m_edgeLength, m_edgeLength);
    QColor squareColor = Qt::white;//scene()->backgroundBrush().color();
    if(getLabel().isEmpty())
        squareColor = GLOBAL_EMPTY_LABEL_COLOR;
    if(getLabel().isEmpty())
        qDebug()<<"lb: "<<boundingRect();

//    painter->setRenderHint(QPainter::Antialiasing);
    if(m_borderWidth)
    {
        QRectF extSquareItem( 0, 0, m_edgeLength+2*m_borderWidth, m_edgeLength+2*m_borderWidth);
        QColor extSquareColor = GLOBAL_BORDER_COLOR;
        QPainterPath externalFigure;
        externalFigure.addRect(extSquareItem);
        painter->setPen(Qt::NoPen);
        painter->fillPath(externalFigure, extSquareColor);
        painter->drawPath(externalFigure);
    }
    QPainterPath path;
    path.addRect(squareItem);
    painter->setPen(Qt::NoPen);
    painter->fillPath(path, squareColor);
    painter->drawPath(path);

//DRAW TEXT
    if(!m_label.isEmpty())
    {
        painter->setRenderHint(QPainter::TextAntialiasing,false);
        QFont font = painter->font();
        font.setPixelSize(8);
        painter->setFont(font);


        QPen textPen(Qt::black);
        painter->setPen(textPen);
        painter->drawText(boundingRect(),Qt::AlignCenter,m_label);
    }
}
QString LabelItem::getLabel() const
{
    return m_label;
}

void LabelItem::setLabel(const QString &value)
{
    m_label = value;
}

QColor LabelItem::getColor() const
{
    return m_color;
}

void LabelItem::setColor(const QColor &color)
{
    m_color = color;
}
int LabelItem::getEdgeLength() const
{
    return m_edgeLength;
}

void LabelItem::setEdgeLength(int edgeLength)
{
    m_edgeLength = edgeLength;
}
qreal LabelItem::getBorderWidth() const
{
    return m_borderWidth;
}

void LabelItem::setBorderWidth(const qreal &borderWidth)
{
    m_borderWidth = borderWidth;
}




QRectF LabelItem::boundingRect() const
{
    return QRectF( 0, 0, m_edgeLength + 2*m_borderWidth, m_edgeLength + 2*m_borderWidth);
}

//QPainterPath LabelItem::shape() const
//{
//    QPainterPath path;
//    path.addRect(boundingRect());
// //    path.addRect(m_borderWidth,m_borderWidth,m_edgeLength,m_edgeLength);
//    return path;
//}

void LabelItem::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}

QSizeF LabelItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
    case Qt::MaximumSize:
        return QSizeF(m_edgeLength + 2*m_borderWidth,m_edgeLength + 2*m_borderWidth);
    default:
        break;
    }
    return constraint;
}
