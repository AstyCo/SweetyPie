#ifndef GANTTGRAPHICSHEADERRECTITEM_H
#define GANTTGRAPHICSHEADERRECTITEM_H


//#include "ganttgraphicsheader.h"

//===========konus==================
#include <QGraphicsRectItem>
#include "QGraphicsLayoutItem"
#include <QDate>
#include <QPainter>

#define TM_DATE_FONT_SIZE 8

enum DateStrSize
{
  dssDayOnly,
  dssTwoLineDate,
  dssOneLineDate,
  dssOneLineDateWithDayOfWeek
};
//===============================

enum Scale // указывает масштаб линейки
{
    ScaleAuto,
    ScaleSecond,
    ScaleMinute,
    ScaleHour,
    ScaleDay,
    ScaleWeek,
    ScaleMonth,
    ScaleUserDefined
};

class GanttGraphicsHeaderRectItem : public QGraphicsLayoutItem, public QGraphicsRectItem
{
public:
    enum HeaderType
    {
        Lower,
        Upper
    };

    GanttGraphicsHeaderRectItem(QDateTime dateTime,
                                GanttGraphicsHeaderRectItem::HeaderType headerType,
                                Scale scale,
                                qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);

    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &geom);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    //=================



    //=============konus============
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        QDate GetDate() { return m_date; }
        void setDayDelimiterVisible(bool visible);
        /*static*/ void SetDateStrSize(DateStrSize dateSize);

      private:
        QDate m_date;
        QString m_dateStrNano;
        QString m_dateStrSmall;
        QString m_dateStrMedium;
        QString m_dateStrFull;
        QColor m_dateColor;
        int m_dayHeight;
        bool m_dayDelimiter;

        /*static */ DateStrSize m_useDateStr;

        QString GetDayName(Qt::DayOfWeek d);
    //========================================

private:
        QDateTime m_dateTime;
        QString m_text;
        QColor m_color;
        qreal m_height;
        qreal m_width;

        bool m_delimiter;


};

#endif // GANTTGRAPHICSHEADERRECTITEM_H
