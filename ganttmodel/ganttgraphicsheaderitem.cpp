#include "ganttgraphicsheaderitem.h"

#include "QDebug"

GanttGraphicsHeaderItem::GanttGraphicsHeaderItem(QDateTime begin, QDateTime end, Zoom zoom, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    setGraphicsItem(this);
    m_begin = begin;
    m_end = end;
    m_zoom = zoom;
    m_height = 40;
    m_width = 0;
}

GanttGraphicsHeaderItem::~GanttGraphicsHeaderItem()
{

}

QRectF GanttGraphicsHeaderItem::boundingRect() const
{
    return QRectF(QPointF(0,0), geometry().size());
}

void GanttGraphicsHeaderItem::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}

QSizeF GanttGraphicsHeaderItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
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

void GanttGraphicsHeaderItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //=========================
    QFont paintFont = painter->font();
    paintFont.setPointSize(8);
    painter->setFont(paintFont);

    QPen myPen;// = pen();
    myPen.setWidth(1);
    myPen.setColor(QColor(Qt::black));
    painter->setPen(myPen);

    int lowHeight = 20;
    int lowWidth = 20; //TODO //почему при ZoomMinute и lowWidth>15 происходит наложение блоков
    int lowCount = lowItemsCount(m_begin, m_end, m_zoom);

    int lowPerUpCount = 0;
    QDateTime currentDT = m_begin;
    QDateTime bufferDT;
    qreal xLowPos = 0;
    qreal xUpPos = 0;
    qreal yPos = 20;
    for(int i = 0; i < lowCount; i++)
    {
        QString lowText;
        switch (m_zoom) {
        case ZoomSecond:
            lowText = currentDT.toString("ss");
            break;
        case ZoomMinute:
            lowText = currentDT.toString("mm");
            break;
        case ZoomHour:
            lowText = currentDT.toString("hh");
            break;
        case ZoomDay:
            lowText = currentDT.toString("dd");
            break;
        case ZoomMonth:
            lowText = currentDT.toString("MMM").left(1).toUpper();
            break;
        default:
            break;
        }

        QRectF lowRect(xLowPos, yPos, lowWidth, lowHeight);
        painter->setBrush(QBrush(QColor(206,206,206)));
        painter->drawRect(lowRect);

        Qt::DayOfWeek d = (Qt::DayOfWeek)currentDT.date().dayOfWeek();
        if (((d == Qt::Saturday) || (d == Qt::Sunday)) && (m_zoom == ZoomDay))
          m_color = QColor(230,0,0);
        else
          m_color = QColor(Qt::black);

        QPen oldPen = painter->pen();
        QPen newPen(oldPen);
        newPen.setColor(m_color);
        painter->setPen(newPen);
        lowRect.adjust(0, 1, 0, -1);
        painter->drawText(lowRect, Qt::AlignCenter, lowText);
        xLowPos += lowWidth;
        painter->setPen(oldPen);


        lowPerUpCount++;
        bufferDT = increaseDateTime(currentDT, m_zoom, 1);
        int a = currentDT.toString(formatOverDateTime(m_zoom)).toInt();
        int b = bufferDT.toString(formatOverDateTime(m_zoom)).toInt();
        if ((b!=a) || (i == lowCount-1))
        {
            QString upText;
            switch (m_zoom) {
            case ZoomSecond:
                upText = currentDT.toString("ddd d MMM yy, hh:mm");
                break;
            case ZoomMinute:
                upText = currentDT.toString("ddd d MMM yy, hh:00");
                break;
            case ZoomHour:
                upText = currentDT.toString("ddd d MMM yy");
                break;
            case ZoomDay:
                upText = currentDT.toString("MMM yyyy");
                break;
            case ZoomMonth:
                upText = currentDT.toString("yyyy");
                break;
            default:
                break;
            }

            QRectF upRect(xUpPos, yPos-20, lowWidth * lowPerUpCount, lowHeight);
            painter->drawRect(upRect);
            painter->drawText(upRect, Qt::AlignCenter, upText);
            lowPerUpCount = 0;
            xUpPos = xLowPos;
        }
        currentDT = bufferDT;
    }
    m_width = lowWidth * lowCount;
    //qDebug()<<"width"<<m_width;

}

QDateTime GanttGraphicsHeaderItem::increaseDateTime(QDateTime dateTime, Zoom zoom, int increment)
{
    switch (zoom) {
    case ZoomSecond:
        dateTime = dateTime.addSecs(increment);
        break;
    case ZoomMinute:
        dateTime = dateTime.addSecs(increment*60);
        break;
    case ZoomHour:
        dateTime = dateTime.addSecs(increment*60*60);
        break;
    case ZoomDay:
        dateTime = dateTime.addDays(increment);
        break;
    case ZoomMonth:
        dateTime = dateTime.addMonths(increment);
        break;
    default:
        break;
    }
    return dateTime;
}

int GanttGraphicsHeaderItem::lowItemsCount(QDateTime begin, QDateTime end, Zoom zoom)
{

    int count = 0;
    switch (zoom) {
    case ZoomSecond:
        count = begin.secsTo(end);
        break;
    case ZoomMinute:
        count = begin.secsTo(end)/60;
        break;
    case ZoomHour:
        count = begin.secsTo(end)/(60*60);
        break;
    case ZoomDay:
        count = begin.daysTo(end);
        break;
    case ZoomMonth:
        {
        //TODO
        //считает неправильно
        int a = begin.toString("MM").toInt();
        int b = end.toString("MM").toInt();
        count = b - a;
        if (count == 0)
            count = 1;
        break;
        }
    default:
        break;
    }
    //count++;
    return count;
}

QString GanttGraphicsHeaderItem::formatOverDateTime(Zoom zoom)
{
    QString format;
    switch (zoom) {
    case ZoomSecond:
        format = "mm";
        break;
    case ZoomMinute:
        format = "hh";
        break;
    case ZoomHour:
        format = "dd";
        break;
    case ZoomDay:
        format = "MM";
        break;
    case ZoomMonth:
        format = "yyyy";
        break;
    default:
        format = "yyyy";
        break;
    }
    return format;
}



void GanttGraphicsHeaderItem::clearHeader()
{
//    for(int i = lower->count()-1; i >= 0; --i)
//    {
//        delete lower->itemAt(i);
//    }
//    for(int j = upper->count()-1; j >= 0; --j)
//    {
//        delete upper->itemAt(j);
//    }
}

void GanttGraphicsHeaderItem::zoom(Zoom zoom)
{
    m_zoom = zoom;
}

QDateTime GanttGraphicsHeaderItem::end() const
{
    return m_end;
}

void GanttGraphicsHeaderItem::setEnd(const QDateTime &end)
{
    m_end = end.addDays(2);
}

QDateTime GanttGraphicsHeaderItem::begin() const
{
    return m_begin;
}

void GanttGraphicsHeaderItem::setBegin(const QDateTime &begin)
{
    m_begin = begin;
}
