#include "ganttgraphicsheaderrectitem.h"

//==================konus==================
#include <QBrush>
#include <QPen>

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
            m_text = m_dateTime.toString("ddd d MMM yy, hh:00");
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
}

void GanttGraphicsHeaderRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
/*
    QGraphicsRectItem::paint(painter, option, widget);

    QFont paintFont = painter->font();
    paintFont.setPointSize(TM_DATE_FONT_SIZE);
    painter->setFont(paintFont);

    QRectF bRect = this->rect();
    qreal w = bRect.width();
    qreal hourInterval = w / 24;
    int l;
    bool drawMinTicks = false;
    bool drawZero = true;
    QString dateStr;
    int inc = 1;
    if (hourInterval > 20)
    {
      inc = 1; // вывод часовых засечек, кратных указанному числу
      l = 1; // вывод цифр часов, кратных указанному числу
      drawMinTicks = true;
    }
    else if (hourInterval > 10 && hourInterval <= 20)
    {
      inc = 1;
      l = 3;
    }
    else if (hourInterval > 5 && hourInterval <= 10)
    {
      inc = 3;
      l = 6;
    }
    else
    {
      inc = 6;
      l = 24;
      drawZero = false;
    }

    switch(m_useDateStr)
    {
      case dssDayOnly:
        dateStr = m_dateStrNano;
      break;
      case dssTwoLineDate:
        dateStr = m_dateStrSmall;
      break;
      case dssOneLineDate:
        dateStr = m_dateStrMedium;
        break;
      default:
        dateStr = m_dateStrFull;
    }

    qreal curTickPos = 0;
    qreal minTickPos = hourInterval / 2;
    qreal m_hourY1 = m_dayHeight + 3;

    for (int curHour = 0; curHour <= 24; curHour+=inc)
    {
      painter->drawLine(curTickPos, m_hourY1, curTickPos, m_hourY1 + 10);

      if (curHour == 0)
      {
        if (drawZero)
          painter->drawText(curTickPos + 3, m_hourY1 + 25, "0");
      }
      else if ((curHour % l == 0) && (curHour != 24))
      {
        qreal x = curTickPos;
        // смещение числа часов относительно линии
        x -= (curHour <= 9) ? 3 : 7;
        painter->drawText(x, m_hourY1 + 25, QString::number(curHour));
      }
      curTickPos += hourInterval * inc;

      if (drawMinTicks && curHour < 24)
      {
        painter->drawLine(minTickPos, m_hourY1, minTickPos, m_hourY1 + 7);
        minTickPos += hourInterval;
      }
    }
    // вернуться на крайнюю правую метку (24 часа)
    curTickPos -= hourInterval * inc;

    QPen oldPen = painter->pen();
    QPen newPen(oldPen);
    if (m_dayDelimiter)
    {
      newPen.setStyle(Qt::DashLine);
      painter->setPen(newPen);
      painter->drawLine(curTickPos, m_hourY1, curTickPos, m_hourY1 + 4 * 30);
      newPen.setStyle(Qt::SolidLine);
    }

    newPen.setColor(m_dateColor);
    painter->setPen(newPen);
    bRect.adjust(0, 1, 0, -1);
    painter->drawText(bRect, Qt::AlignCenter, dateStr);
    painter->setPen(oldPen);
*/

    QGraphicsRectItem::paint(painter, option, widget);

    QFont paintFont = painter->font();
    paintFont.setPointSize(TM_DATE_FONT_SIZE);
    painter->setFont(paintFont);

    QRectF bRect = this->rect();
//    qreal w = bRect.width();
//    qreal hourInterval = w / 24;
//    int l;
//    bool drawMinTicks = false;
//    bool drawZero = true;
    QString dateStr;
/*
    int inc = 1;
    if (hourInterval > 20)
    {
      inc = 1; // вывод часовых засечек, кратных указанному числу
      l = 1; // вывод цифр часов, кратных указанному числу
      drawMinTicks = true;
    }
    else if (hourInterval > 10 && hourInterval <= 20)
    {
      inc = 1;
      l = 3;
    }
    else if (hourInterval > 5 && hourInterval <= 10)
    {
      inc = 3;
      l = 6;
    }
    else
    {
      inc = 6;
      l = 24;
      drawZero = false;
    }
*/

    dateStr = m_text;
/*
    qreal curTickPos = 0;
    qreal minTickPos = hourInterval / 2;
    qreal m_hourY1 = m_height + 3;

    for (int curHour = 0; curHour <= 24; curHour+=inc)
    {
      painter->drawLine(curTickPos, m_hourY1, curTickPos, m_hourY1 + 10);

      if (curHour == 0)
      {
        if (drawZero)
          painter->drawText(curTickPos + 3, m_hourY1 + 25, "0");
      }
      else if ((curHour % l == 0) && (curHour != 24))
      {
        qreal x = curTickPos;
        // смещение числа часов относительно линии
        x -= (curHour <= 9) ? 3 : 7;
        painter->drawText(x, m_hourY1 + 25, QString::number(curHour));
      }
      curTickPos += hourInterval * inc;

      if (drawMinTicks && curHour < 24)
      {
        painter->drawLine(minTickPos, m_hourY1, minTickPos, m_hourY1 + 7);
        minTickPos += hourInterval;
      }
    }
    // вернуться на крайнюю правую метку (24 часа)
    curTickPos -= hourInterval * inc;
*/
    QPen oldPen = painter->pen();
    QPen newPen(oldPen);
/*
    if (m_dayDelimiter)
    {
      newPen.setStyle(Qt::DashLine);
      painter->setPen(newPen);
      painter->drawLine(curTickPos, m_hourY1, curTickPos, m_hourY1 + 4 * 30);
      newPen.setStyle(Qt::SolidLine);
    }
*/

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
