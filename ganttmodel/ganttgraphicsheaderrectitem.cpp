#include "ganttgraphicsheaderrectitem.h"

//==================konus==================
#include <QBrush>
#include <QPen>

#include <QDebug>

GanttGraphicsHeaderRectItem::GanttGraphicsHeaderRectItem(QDateTime dateTime, HeaderType headerType, Scale scale, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent) :
    QGraphicsRectItem(x, y, w, h, parent)
{
//    m_date = date;
//    Qt::DayOfWeek d = (Qt::DayOfWeek)date.dayOfWeek();

//    m_dateStrNano = QString::number(m_date.day());
//    m_dateStrSmall = m_date.toString("yyyy\nd.MM");
//    m_dateStrMedium = m_date.toString("d.MM.yyyy");
//    m_dateStrFull = QString("%1 (%2)").arg(m_dateStrMedium,
//                                      GetDayName(d));

//    if ((d == Qt::Saturday) || (d == Qt::Sunday))
//      m_dateColor = QColor(230,0,0);
//    else
//      m_dateColor = QColor(Qt::black);

//    this->setBrush(QBrush(QColor(206,206,206)));
//    this->setToolTip(m_dateStrFull);
//    m_dayHeight = 30;
//    m_dayDelimiter = true;

    setGraphicsItem(this);


    m_dateTime = dateTime;
    Qt::DayOfWeek d = (Qt::DayOfWeek)dateTime.date().dayOfWeek();

    switch (headerType) {
    case Lower:
        switch (scale) {
        case ScaleSecond:
            m_text = m_dateTime.toString("ss");
            break;
        case ScaleMinute:
            m_text = m_dateTime.toString("mm");
            break;
        case ScaleHour:
            m_text = m_dateTime.toString("hh");
            break;
        case ScaleDay:
            m_text = m_dateTime.toString("dd");
            break;
        case ScaleMonth:
            m_text = m_dateTime.toString("MMM").left(1).toUpper();
            break;
        default:
            break;
        }
        break;
    case Upper:
        switch (scale) {
        case ScaleSecond:
            m_text = m_dateTime.toString("ddd d MMM yy, hh:mm");
            break;
        case ScaleMinute:
            m_text = m_dateTime.toString("ddd d MMM yy, hh" + QObject::trUtf8(" час"));
            break;
        case ScaleHour:
            m_text = m_dateTime.toString("ddd d MMM yy");
            break;
        case ScaleDay:
            m_text = m_dateTime.toString("MMM yyyy");
            break;
        case ScaleMonth:
            m_text = m_dateTime.toString("yyyy");
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    if (((d == Qt::Saturday) || (d == Qt::Sunday)) && (headerType == Lower) && (scale == ScaleDay))
      m_color = QColor(230,0,0);
    else
      m_color = QColor(Qt::black);

    this->setBrush(QBrush(QColor(206,206,206)));
    this->setToolTip(m_dateTime.toString("ddd d MMM yy, hh:mm:ss"));
    m_width = w;
    m_height = h;
    m_delimiter = true;

    //qDebug()<<"text"<<m_text;

    //setRect(0,0,m_width,m_height);
}

void GanttGraphicsHeaderRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{


    QGraphicsRectItem::paint(painter, option, widget);

    QFont paintFont = painter->font();
    paintFont.setPointSize(TM_DATE_FONT_SIZE);
    painter->setFont(paintFont);

    QRectF bRect = this->rect();

    QString dateStr;


    dateStr = m_text;

    QPen oldPen = painter->pen();
    QPen newPen(oldPen);


    //qDebug()<<"dateStr"<<dateStr;

    newPen.setColor(m_color);
    painter->setPen(newPen);
    bRect.adjust(0, 1, 0, -1);
    painter->drawText(bRect, Qt::AlignCenter/*Qt::AlignVCenter|Qt::AlignLeft*/, dateStr);
    painter->setPen(oldPen);
}

void GanttGraphicsHeaderRectItem::setDayDelimiterVisible(bool visible)
{
    m_dayDelimiter = visible;
}

void GanttGraphicsHeaderRectItem::SetDateStrSize(DateStrSize dateSize)
{
    m_useDateStr = dateSize;
}

QString GanttGraphicsHeaderRectItem::GetDayName(Qt::DayOfWeek d)
{
    switch (d)
    {
      case Qt::Monday: return QObject::tr("Пн");
      case Qt::Tuesday: return QObject::tr("Вт");
      case Qt::Wednesday: return QObject::tr("Ср");
      case Qt::Thursday: return QObject::tr("Чт");
      case Qt::Friday: return QObject::tr("Пт");
      case Qt::Saturday: return QObject::tr("Сб");
      case Qt::Sunday: return QObject::tr("Вс");
    }
    return QString();
}
//======================================


void GanttGraphicsHeaderRectItem::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}


QSizeF GanttGraphicsHeaderRectItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
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
