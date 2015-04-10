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

    //m_scale = m_header->zoom();
    //m_headerBegin = m_header->begin();
    //m_headerEnd = m_header->end();


    m_begin = item->commonBegin();
    m_end = item->commonEnd();
    m_duration = m_begin.secsTo(m_end);

    m_color = m_ganttItem->color();
    m_height = 10;
    m_text = "";


    QPen testPen;
    if(m_ganttItem->hasChildren())
    {
        //m_text = "parent";
        m_color.setAlpha(40);
        //testPen.setWidth(-1);
    }
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

    qreal secWidth = m_header->lowWidth();

    m_width = m_duration*secWidth;

    durationTillBegin = durationTillBegin*secWidth;

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
        //TODO размер месяцев зависит от количества дней
        m_width = m_width/(86400*30);
        durationTillBegin = durationTillBegin/(86400*30);
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
    painter->setPen(oldPen);
}


