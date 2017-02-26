#include "gantttextgraphicsobject.h"

#include "gantt/private_extensions/gantt-lib_global_values.h"

#include <QPainter>

int GanttTextGraphicsObject::_spacing = 10;
QFont GanttTextGraphicsObject::_captureFont;
QFontMetrics GanttTextGraphicsObject::_fontMetrics(GanttTextGraphicsObject::_captureFont);

GanttTextGraphicsObject::GanttTextGraphicsObject(GanttInfoItem *item, QGraphicsItem *parent)
    : GanttGraphicsObject( item, parent)
{
//    connect( item, SIGNAL(expanded()), this, SLOT(onExpansionChange()));
//    connect( item, SIGNAL(collapsed()), this, SLOT(onExpansionChange()));
}

void GanttTextGraphicsObject::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem */*option*/,
                                QWidget */*widget*/)
{
    if(!textLeft().isEmpty() || !textRight().isEmpty()){
        painter->save();

        painter->setPen(Qt::black);
        painter->setFont(_captureFont);
        if(!textLeft().isEmpty())
            painter->drawText(textLeftRect(), textLeft(), QTextOption(Qt::AlignCenter));
        if(!textRight().isEmpty())
            painter->drawText(textRightRect(), textRight(), QTextOption(Qt::AlignCenter));

        painter->restore();
    }
}

QRectF GanttTextGraphicsObject::boundingRect() const
{
    QRectF res = shape().controlPointRect();
    if(res.height() < DEFAULT_ITEM_HEIGHT)
        res.setHeight( DEFAULT_ITEM_HEIGHT);
    if(res.y() != 0)
        res.moveTop(0);

    if(!textLeft().isEmpty())
        res.adjust( -_fontMetrics.boundingRect(textLeft()).width() - _spacing, 0, 0, 0);
    if(!textRight().isEmpty())
        res.adjust( 0, 0, _fontMetrics.boundingRect(textRight()).width() + _spacing, 0);
    return res;
}

QString GanttTextGraphicsObject::textRight() const{
    return QString();
}

QString GanttTextGraphicsObject::textLeft() const{
    return QString();
}

QString GanttTextGraphicsObject::textIn() const{
    return QString();
}

QRect GanttTextGraphicsObject::textLeftRect() const
{
    int textWidth = _fontMetrics.boundingRect(textLeft()).width();
    return QRect( -textWidth + shape().controlPointRect().left() - _spacing, 0,
                  textWidth, boundingRect().height());
}

QRect GanttTextGraphicsObject::textRightRect() const
{
    int textWidth = _fontMetrics.boundingRect(textRight()).width();

    return QRect( shape().controlPointRect().right() + _spacing, 0,
             textWidth, boundingRect().height());
}

void GanttTextGraphicsObject::onExpansionChange()
{
    qDebug() << "onExp";
    prepareGeometryChange();
}
