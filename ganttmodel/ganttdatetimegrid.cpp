#include "ganttdatetimegrid.h"

#include "QStyle"

GanttDateTimeGrid::GanttDateTimeGrid()
{
}

//void GanttDateTimeGrid::paintHeader( QPainter* painter,
//                                         const QRectF& headerRect, const QRectF& exposedRect,
//                                         qreal offset, QWidget* widget,
//                                         HeaderType headerType,
//                                         DateTextFormatter *formatter )
//{
//    QStyle* style = widget?widget->style():QApplication::style();

//    const qreal left = exposedRect.left() + offset;
//    const qreal right = exposedRect.right() + offset;

//    // Paint a section for each hour
//    QDateTime dt = chartXtoDateTime( left );
//    dt = adjustDateTimeForHeader( dt, headerType );
//    // Determine the time step per grid line
//    int offsetSeconds = 0;
//    int offsetDays = 0;
//    int offsetMonths = 0;

//    switch ( headerType ) {
//        case Private::HeaderHour:
//            offsetSeconds = 60*60;
//            break;
//        case Private::HeaderDay:
//            offsetDays = 1;
//            break;
//        case Private::HeaderWeek:
//            offsetDays = 7;
//            break;
//        case Private::HeaderMonth:
//            offsetMonths = 1;
//            break;
//        case Private::HeaderYear:
//            offsetMonths = 12;
//            break;
//        default:
//            // Other scales cannot be painted with this method!
//            assert( false );
//            break;
//    }

//    for ( qreal x = dateTimeToChartX( dt ); x < right;
//          dt = dt.addSecs( offsetSeconds ), dt = dt.addDays( offsetDays ), dt = dt.addMonths( offsetMonths ),
//          x = dateTimeToChartX( dt ) ) {
//        QStyleOptionHeader opt;
//        if ( widget ) opt.init( widget );
//        opt.rect = formatter->textRect( x, offset, dayWidth, headerRect, dt );
//        opt.text = formatter->format( dt );
//        opt.textAlignment = Qt::AlignCenter;
//        style->drawControl(QStyle::CE_Header, &opt, painter, widget);
//    }
//}
