#include "ganttgraphicsitem.h"

#include <QDebug>

//#include "QMetaType"

/*
GanttGraphicsItem::GanttGraphicsItem()
{
    //qRegisterMetaType<GanttGraphicsItem>("GanttGraphicsItem");
    setOrientation(Qt::Horizontal);
}

GanttGraphicsItem::GanttGraphicsItem(QList<GanttGraphicsItemStage *> stages)
{
    //qRegisterMetaType<GanttGraphicsItem>("GanttGraphicsItem");
    setOrientation(Qt::Horizontal);
    m_stageList = stages;
    int i = 0;
    foreach (m_stage, m_stageList) {
        m_stage = m_stageList.at(i);
        this->addItem(m_stage);
        i++;
    }
}
*/

GanttGraphicsItem::GanttGraphicsItem(GanttItem *item, GanttGraphicsHeader * header, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    setGraphicsItem(this);

    m_header = header;
    m_ganttItem = item;

    m_begin = item->commonBegin();
    m_end = item->commonEnd();
    m_duration = m_begin.secsTo(m_end);

    m_color = m_ganttItem->color();
    m_height = 10;
    m_text = "";


    QPen testPen;
//    if(m_ganttItem->hasChildren())
//    {
//        //m_text = "parent";
//        m_color.setAlpha(40);
//        //testPen.setWidth(-1);
//    }
    this->setToolTip(m_ganttItem->name());
    this->setPen(testPen);
    this->setBrush(QBrush(m_color));

    connect(m_header, SIGNAL(zoomChanged(Scale)),this,SLOT(calcSizeSlot()));
    calcSizeSlot();
}

GanttGraphicsItem::~GanttGraphicsItem()
{
    //delete m_ganttItem;

}

void GanttGraphicsItem::calcSizeSlot()
{
    qreal durationTillBegin = m_header->begin().secsTo(m_ganttItem->commonBegin());

    qreal lowWidth = m_header->lowWidth();

    m_width = m_duration*lowWidth;

    durationTillBegin = durationTillBegin*lowWidth;

    switch (m_header->zoom()) {
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
//        if((m_begin.date().year() == m_end.date().year()) &&
//           (m_begin.date().month() == m_end.date().month()))
//        {
//            m_width = (lowWidth*m_begin.daysTo(m_end))/m_begin.date().daysInMonth();
//        }
//        else
//        {
//            int monthDiff;
//            if(m_begin.date().year() == m_end.date().year())
//            {
//                monthDiff = m_end.date().month()-m_begin.date().month();
//            }
//            else
//            {
//                monthDiff = (12-m_begin.date().month()) +
//                            (m_end.date().year()-m_begin.date().year()-1) +
//                             m_end.date().month();
//            }

//            m_width = lowWidth*((m_begin.date().daysInMonth()-m_begin.date().day())/m_begin.date().daysInMonth()
//                    + monthDiff + m_end.date().day()/m_end.date().daysInMonth());
//        }
//        //----------------
//        if((m_begin.date().year() == m_header->begin().date().year()) &&
//           (m_begin.date().month() == m_header->begin().date().month()))
//        {
//            durationTillBegin = (lowWidth*m_header->begin().daysTo(m_begin))/m_header->begin().date().daysInMonth();
//        }
//        else
//        {
//            int monthDiff;
//            if(m_begin.date().year() == m_header->begin().date().year())
//            {
//                monthDiff = m_begin.date().month()-m_header->begin().date().month()-1;
//            }
//            else
//            {
//                monthDiff = (12-m_header->begin().date().month()) +
//                            (m_begin.date().year()-m_header->begin().date().year()-1) +
//                             m_begin.date().month();
//            }

//            durationTillBegin = lowWidth*((m_header->begin().date().daysInMonth()-m_header->begin().date().day())/m_header->begin().date().daysInMonth()
//                    + monthDiff + m_begin.date().day()/m_begin.date().daysInMonth());
//        }

        //TODO размер месяцев зависит от количества дней
        m_width = m_width/(86400*(365/12));
        durationTillBegin = durationTillBegin/(86400*(365/12));
        break;
    default:
        break;
    }

    setRect(m_header->m_fullHeaderRect.x()+durationTillBegin,0,m_width,m_height);

    update();
}


void GanttGraphicsItem::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}

QSizeF GanttGraphicsItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
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

void GanttGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
    oldPen.setWidth(12);
    QPen newPen(oldPen);

    newPen.setColor(QColor(Qt::black));
    painter->setPen(newPen);
    bRect.adjust(0, 1, 0, -1);
    painter->drawText(bRect, Qt::AlignCenter/*Qt::AlignVCenter|Qt::AlignLeft*/, dateStr);
    //painter->setPen(oldPen);

    if(m_ganttItem->hasChildren())
    {
        newPen.setWidth(1);
        painter->setPen(newPen);
        painter->setBrush(QBrush(Qt::black));

        QPointF pointsLeft[4] = {
            QPointF(bRect.topLeft()),
            QPointF(bRect.bottomLeft().x(), bRect.bottomLeft().y()+5),
            QPointF(bRect.bottomLeft().x()+5, bRect.bottomLeft().y()),
            QPointF(bRect.topLeft().x()+5, bRect.topLeft().y())
        };
        painter->drawPolygon(pointsLeft,4);

        QPointF pointsRight[4] = {
            QPointF(bRect.topRight()),
            QPointF(bRect.bottomRight().x(), bRect.bottomRight().y()+5),
            QPointF(bRect.bottomRight().x()-5, bRect.bottomRight().y()),
            QPointF(bRect.topRight().x()-5, bRect.topRight().y())
        };
        painter->drawPolygon(pointsRight,4);
    }

    painter->setPen(oldPen);
}


