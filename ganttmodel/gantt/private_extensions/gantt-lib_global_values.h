/*!
 * \file
 * \~russian
 * \brief Файл с глобальными константами
 * \~englist
 * \brief File contains global constants
 */
#ifndef GANTTLIB_GLOBAL_VALUES_H
#define GANTTLIB_GLOBAL_VALUES_H

#define PLAYER_CURSOR_PRESSED   (Qt::SizeHorCursor)
#define PLAYER_CURSOR_OVER      (Qt::PointingHandCursor)

#define SLIDER_COLOR (QColor(125,179,250))
#define HEADER_COLOR (QColor(32,97,196))
#define PLAYER_COLOR (QColor(Qt::red).lighter(130))

#define GANTTPLAYER_DEFAULT_STEP (40*_MILISECONDS_IN_SECOND)
#define GANTTPLAYER_DEFAULT_FREQUENCY (qreal(25))

#define GANTTSCENE_MIN_WIDTH (300)
#define GANTTGRAPHICSVIEW_MIN_WIDTH (GANTTSCENE_MIN_WIDTH)
#define GANTTTREEVIEW_MIN_WIDTH (200)


#define MIN_VIS_WIDTH 4
#define RECTANGLE_OFFSET 5

#define DEFAULT_ITEM_WIDTH      (qreal(20))
#define DEFAULT_MONTH_WIDTH     (qreal(35))

#define DEFAULT_ITEM_HEIGHT     (20)
#define TOP_HEADER_HEIGHT       (20)
#define BOTTOM_HEADER_HEIGHT    DEFAULT_ITEM_HEIGHT
#define HEADER_HEIGHT    (TOP_HEADER_HEIGHT + BOTTOM_HEADER_HEIGHT)

//the three values below are linked
#define MIN_WIDTH_FOR_MONTH_VISUALIZING (qreal(50))
#define MIN_WIDTH_FOR_TIME_VISUALIZING (qreal(36)) // in pixels
#define LITTLE_HATCH_COUNT (5)
#define MIN_DISTANCE_BETWEEN_HATCHES (qreal(5))

#define HOURS_IN_DAY (24)
#define MINUTS_IN_HOUR (60)
#define SECONDS_IN_MINUTE (60)
#define _MILISECONDS_IN_SECOND (1000)
#define _MICROSECONDS_IN_MILISECOND (1000)

#define _MICROSECONDS_IN_SECOND (1000000)


#define SECONDS_IN_HOUR (3600)
#define SECONDS_IN_DAY (86400)

#endif // GANTTLIB_GLOBAL_VALUES_H
