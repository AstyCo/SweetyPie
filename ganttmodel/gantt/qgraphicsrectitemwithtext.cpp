#include "qgraphicsrectitemwithtext.h"

#include <QPainter>

#include <QBrush>

#include <QDebug>

QGraphicsRectItemWithText::QGraphicsRectItemWithText(const QString& text, const QRectF & rect, QGraphicsItem * parent,QGraphicsScene *scene) :
    QGraphicsRectItem(rect,parent)
{
    m_text = text;
    m_textColor = Qt::black;
    setBrush(QBrush((QColor(Qt::gray).lighter(135))));
}

void QGraphicsRectItemWithText::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint(painter,option,widget);

    painter->setPen(m_textColor);
    painter->drawText(rect(),m_text,QTextOption(Qt::AlignCenter));
}
QColor QGraphicsRectItemWithText::textColor() const
{
    return m_textColor;
}

void QGraphicsRectItemWithText::setTextColor(const QColor &textColor)
{
    m_textColor = textColor;
}


