#ifndef GANTTDATETIMEGRID_H
#define GANTTDATETIMEGRID_H

#include <QGraphicsItemGroup>

class GanttDateTimeGrid : public QGraphicsItem
{
public:
    GanttDateTimeGrid();
    //boundingRect();

    class DateTextFormatter {
    public:
        virtual QString format( const QDateTime& dt ) = 0;
        virtual QRect   textRect( qreal x, qreal offset, qreal dayWidth, const QRectF& headerRect, const QDateTime& dt ) = 0;
    };

    enum HeaderType {
        HeaderHour,
        HeaderDay,
        HeaderWeek,
        HeaderMonth,
        HeaderYear
    };

//    void paintHeader(QPainter* painter,
//                     const QRectF& headerRect, const QRectF& exposedRect,
//                     qreal offset, QWidget* widget,
//                     HeaderType headerType,
//                     DateTextFormatter *formatter );
};

#endif // GANTTDATETIMEGRID_H
