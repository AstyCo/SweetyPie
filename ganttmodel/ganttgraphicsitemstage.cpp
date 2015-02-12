#include "ganttgraphicsitemstage.h"

#include <QBrush>
#include <QPen>
#include <QPainter>

#include <QDebug>

/*
GanttGraphicsItemStage::GanttGraphicsItemStage(QDateTime begin, QDateTime end, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    //QGraphicsRectItem(x, y, w, h, parent)
{
    setGraphicsItem(this);

    m_begin = begin;
    m_end = end;
    m_duration = m_begin.secsTo(m_end);

    qreal secWidth = 20;
    m_width = m_duration*secWidth;
    m_height = h;

    m_text = "";

    this->setToolTip("omfg");

    m_color = QColor(206,206,206);
    this->setBrush(QBrush(m_color));

    //TODO x
   //getThisFkingXposition();

   QGraphicsRectItem(x, y, m_width, h, parent);
}
*/

GanttGraphicsItemStage::GanttGraphicsItemStage(GanttItem *item, Scale scale, qreal durationTillBegin, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)//: QGraphicsLayoutItem()//, QGraphicsItem(parent)
{
    setGraphicsItem(this);

    m_begin = item->commonBegin();
    m_end = item->commonEnd();
    m_duration = item->commonDuration();

    qreal secWidth = 20;
    m_width = m_duration*secWidth;

    durationTillBegin = durationTillBegin*secWidth;

    switch (scale) {
    case ScaleSecond:
        //m_width = m_width;
        break;
    case ScaleMinute:
        m_width = m_width/60;
        durationTillBegin = durationTillBegin/60;
        break;
    case ScaleHour:
        m_width = m_width/3600;
        durationTillBegin = durationTillBegin/3600;
        break;
    case ScaleDay:
        m_width = m_width/86400;
        durationTillBegin = durationTillBegin/86400;
        break;
    case ScaleMonth:
        //m_width = m_width/86400;
        break;
    default:
        break;
    }


    m_height = 15;
    m_text = "text";
    this->setToolTip("tip");

    //QTime midnight(0,0,0);
    //qsrand(midnight.secsTo(QTime::currentTime()));

    m_color = QColor(qrand()/*Qt::green*/);
    //m_color = QColor(qrand(),qrand(),qrand()/*Qt::green*/);
    //m_color = QColor(206,206,206);
    this->setBrush(QBrush(m_color));

    setRect(durationTillBegin,0,m_width,m_height);
}

GanttGraphicsItemStage::~GanttGraphicsItemStage()
{

}

void GanttGraphicsItemStage::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}

QSizeF GanttGraphicsItemStage::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
        // Do not allow a size smaller than the pixmap with two frames around it.
        return QSize(m_width, m_height);
    case Qt::MaximumSize:
        return QSizeF(1000,1000);
    default:
        break;
    }
    return constraint;
}

//QRectF GanttGraphicsItemStage::boundingRect() const
//{
//    return QRectF(QPointF(0,0), geometry().size());
//}

void GanttGraphicsItemStage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint(painter, option, widget);

    //painter->drawRect(0, 0, m_width, 15);

    QFont paintFont = painter->font();
    paintFont.setPointSize(TM_DATE_FONT_SIZE);
    painter->setFont(paintFont);

    QRectF bRect = this->rect();
    //qreal w = bRect.width();
    QString dateStr;

    dateStr = m_text;

    QPen oldPen = painter->pen();
    QPen newPen(oldPen);

    newPen.setColor(QColor(Qt::black));
    painter->setPen(newPen);
    bRect.adjust(0, 1, 0, -1);
    painter->drawText(bRect, Qt::AlignCenter/*Qt::AlignVCenter|Qt::AlignLeft*/, dateStr);
    painter->setPen(oldPen);
}
