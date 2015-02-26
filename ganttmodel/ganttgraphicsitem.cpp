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

GanttGraphicsItem::GanttGraphicsItem(GanttItem *item, Scale scale, QDateTime headerBegin, QDateTime headerEnd, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    //setOrientation(Qt::Horizontal);

    m_scale = scale;

    m_headerBegin = headerBegin;
    m_headerEnd = headerEnd;

    m_ganttItem = item;
    //setGraphicsStages();

    qreal durationTillBegin = m_headerBegin.secsTo(m_ganttItem->commonBegin());





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
    m_text = "";
    this->setToolTip("tip");

    //QTime midnight(0,0,0);
    //qsrand(midnight.secsTo(QTime::currentTime()));

    m_color = QColor(qrand()%255, qrand()%255, qrand()%255);
    //m_color = QColor(qrand(),qrand(),qrand()/*Qt::green*/);
    //m_color = QColor(206,206,206);
    this->setBrush(QBrush(m_color));

    setRect(durationTillBegin,0,m_width,m_height);
}

GanttGraphicsItem::~GanttGraphicsItem()
{
    //delete m_ganttItem;

}


void GanttGraphicsItem::setGraphicsStages()
{
//        qreal durationTillBegin = m_headerBegin.secsTo(m_ganttItem->commonBegin());
//        m_graphicsStage = new GanttGraphicsItemStage(m_ganttItem, m_scale, durationTillBegin);
//        addItem(m_graphicsStage);

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
    QPen newPen(oldPen);

    newPen.setColor(QColor(Qt::black));
    painter->setPen(newPen);
    bRect.adjust(0, 1, 0, -1);
    painter->drawText(bRect, Qt::AlignCenter/*Qt::AlignVCenter|Qt::AlignLeft*/, dateStr);
    painter->setPen(oldPen);
}
