#ifndef GANTTGRAPHICSHEADERITEM_H
#define GANTTGRAPHICSHEADERITEM_H

#include <QGraphicsLayoutItem>
#include <QGraphicsItem>

#include <QDate>
#include <QPainter>

enum Zoom // указывает масштаб линейки
{
    ZoomAuto,
    ZoomSecond,
    ZoomMinute,
    ZoomHour,
    ZoomDay,
    ZoomWeek,
    ZoomMonth,
    ZoomUserDefined
};

enum HeaderType
{
    Lower,
    Upper
};

class GanttGraphicsHeaderItem : public QGraphicsLayoutItem, public QGraphicsItem
{

public:
    GanttGraphicsHeaderItem(QDateTime begin, QDateTime end, Zoom zoom,QGraphicsItem * parent = 0);
    ~GanttGraphicsHeaderItem();

    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &geom);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    //=================

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QDateTime begin() const;
    void setBegin(const QDateTime &begin);

    QDateTime end() const;
    void setEnd(const QDateTime &end);

private:
    QDateTime increaseDateTime(QDateTime dateTime, Zoom zoom, int increment);
    int lowItemsCount(QDateTime begin, QDateTime end, Zoom zoom);
    QString formatOverDateTime(Zoom zoom);

    void rectStuff(QDateTime dateTime,
                   HeaderType headerType,
                   Zoom zoom,
                   qreal x, qreal y, qreal w, qreal h);

    //QColor m_color;
    QDateTime m_begin;
    QDateTime m_end;
    Zoom m_zoom;


    QDateTime m_dateTime;
    QString m_text;
    QColor m_color;
    qreal m_height;
    qreal m_width;
signals:

public slots:
    void createHeader();
    void clearHeader();
    void zoom(Zoom zoom);
};

#endif // GANTTGRAPHICSHEADERITEM_H
