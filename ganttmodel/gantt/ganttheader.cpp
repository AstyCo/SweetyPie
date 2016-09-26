#include "ganttheader.h"

#include "ganttscene.h"
#include "ganttinfoleaf.h"
#include "ganttinfonode.h"
#include "ganttwidget.h"

#include "ganttgraphicsview.h"


#include <QGraphicsRectItem>

#include <QGraphicsView>
#include <QPainter>
#include <QBrush>
#include <QDebug>

GanttHeader::GanttHeader(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{

    m_scene = NULL;
    m_widget = NULL;
    m_mode = GanttPrecisionMode_count;
    m_headerMode = TimelineMode;

    m_visItemCount = 1;

    m_currentWidth = 0;
    m_stretchFactor = 1;

    m_isEmpty = true;
    setZValue(10);


}

void GanttHeader::setScene(QGraphicsScene *scene)
{
    m_scene = dynamic_cast<GanttScene*>(scene);
    if(!m_scene)
    {
        qWarning("GanttHeader::setScene no m_scene");
        return;
    }
    setVisItemCount((m_scene->width() - MIN_WIDTH_FOR_TIME_VISUALIZING) / MIN_WIDTH_FOR_TIME_VISUALIZING);
    m_widget = dynamic_cast<GanttWidget*>(m_scene->parent());

    init();
    m_scene->addItem(this);

    updateHeader();
}

QRectF GanttHeader::boundingRect() const
{
    return QRectF(0,0,m_currentWidth,DEFAULT_HEADER_HEIGHT);
}

void GanttHeader::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    //

    if(m_headerMode == GanttHeader::TimelineMode)
    {
        QRect rect = boundingRect().toRect();
        QRect headerRect(rect.left(),rect.top(),rect.width(),DEFAULT_HEADER_HEIGHT);
        // Header background
        QLinearGradient linearGradient(QPointF(rect.left(),rect.top()),QPointF(rect.left(),rect.top() + DEFAULT_HEADER_HEIGHT));
        linearGradient.setColorAt(0, SLIDER_COLOR);
        linearGradient.setColorAt(0.4, Qt::white);
        linearGradient.setColorAt(0.6, Qt::white);
        linearGradient.setColorAt(1, SLIDER_COLOR);
        painter->fillRect(headerRect, linearGradient);
        // Center horizontal line
        painter->drawLine(QPointF(0,DEFAULT_ITEM_HEIGHT),QPointF(rect.right(),DEFAULT_ITEM_HEIGHT));
        // Little and big hatches(with text)
        qreal curOffset = MIN_WIDTH_FOR_TIME_VISUALIZING/2;

        UtcDateTime dt = m_startDt;

        if(!m_isEmpty)
        {
            qDebug() <<"painting no empty";
            do
            {
                if(isDrawn(dt,m_mode))
                {
                    painter->drawLine(QPointF(curOffset,DEFAULT_ITEM_HEIGHT),QPointF(curOffset,DEFAULT_ITEM_HEIGHT*(3.0/2))); // big hatch
                    QFont dtFont("Goudy Old Style", 8);
                    painter->setFont(dtFont);
                    painter->drawText(QRectF(-MIN_WIDTH_FOR_TIME_VISUALIZING/2 + curOffset, DEFAULT_ITEM_HEIGHT*(3.0/2),
                                             MIN_WIDTH_FOR_TIME_VISUALIZING,DEFAULT_ITEM_HEIGHT/2)
                                      ,dt.toString(formatForMode(m_mode)),QTextOption(Qt::AlignCenter));
                }
                else
                    painter->drawLine(QPointF(curOffset,DEFAULT_ITEM_HEIGHT),QPointF(curOffset,DEFAULT_ITEM_HEIGHT*(5.0/4))); // little hatch

                dt=dt.addMicroseconds(modeToMicrosecond(m_mode,dt.date()) / modeToSegmentCount(m_mode,dt.date()));
                curOffset=dtToX(dt);
            }
            while(dt <= m_finishDt);
        }
    }

}

void GanttHeader::updateHeader()
{
    if(!m_scene)
    {
        qWarning("GanttHeader::updateHeader no m_scene");
        return;
    }

    if(m_isEmpty)
        return;

    if(m_headerMode == GanttDiagramMode)
    {
        clear();

        initRange();

        UtcDateTime dt = m_startDt;


        qreal curOffset = 0;

        while(dt<m_finishDt)
        {
            UtcDateTime nextDt = nextTopHeaderDt(dt,m_mode);

            qreal topHeaderStartsAt = curOffset;
            QGraphicsRectItemWithText *p_topHeaderItem =  new QGraphicsRectItemWithText( dt.toString(topHeaderFormat(m_mode))
                                                                                         ,QRectF()
                                                                                         ,this);

            while(dt<nextDt)
            {
                QGraphicsRectItemWithText *p_secondHeaderItem = new QGraphicsRectItemWithText( dt.toString(secondHeaderFormat(m_mode))
                                                                                              ,QRectF(curOffset,DEFAULT_ITEM_WIDTH,secondHeaderWidth(),DEFAULT_ITEM_WIDTH)
                                                                                              ,p_topHeaderItem);

                if(m_mode == days1)
                {
                    Qt::DayOfWeek d = (Qt::DayOfWeek)dt.date().dayOfWeek();
                    if (((d == Qt::Saturday) || (d == Qt::Sunday)))
                        p_secondHeaderItem->setTextColor( QColor(230,0,0) );
                }

                m_items.append(p_secondHeaderItem);
                curOffset+=secondHeaderWidth();

                dt = nextSecondHeaderDt(dt,m_mode);
            }

            p_topHeaderItem->setRect(QRectF(topHeaderStartsAt,0,curOffset-topHeaderStartsAt,DEFAULT_ITEM_WIDTH));

            m_items.append(p_topHeaderItem);
        }

        m_scene->updateWidth(curOffset);
    }
    else if(m_headerMode == TimelineMode)
    {
        GanttPrecisionMode newMode = calculateTimeMode(m_minDt,m_maxDt);
        if(setMode(newMode))
        {
            initRange();
            updateStretchFactor();
        }
        else
            initRange();
    }
}

const GanttHeader::GanttPrecisionMode &GanttHeader::mode() const
{
    return m_mode;
}

bool GanttHeader::setMode(const GanttPrecisionMode &mode)
{
    if(m_mode == mode || m_headerMode != TimelineMode)
        return false;

    m_mode = mode;
    updateHeader();
    return true;
}

void GanttHeader::onItemsAddition(const QList<GanttInfoItem *> &items)
{
//    bool newRange = false;
//    foreach(GanttInfoItem *item, items)
//    {
//        newRange|= onItemsAdditionHelper(item);
//    }
//    if(newRange)
    {
        updateHeader();

//        updateWidget();
    }
}

void GanttHeader::onItemsAddition(GanttInfoItem* items)
{
//    if(onItemsAdditionHelper(items))
    {
        updateHeader();

//        updateWidget();
    }
}

void GanttHeader::initRange()
{
    m_startDt = startByDt(m_minDt);
    m_finishDt = finishByDt(m_maxDt);

    setLengthInMicroseconds(m_startDt.microsecondsTo(m_finishDt));

//    if(m_widget)
//        m_widget->repaintDtHeader();
}

UtcDateTime GanttHeader::startByDt(const UtcDateTime &dt,GanttPrecisionMode mode) const
{
    if(m_isEmpty)
        return UtcDateTime();

    if(m_headerMode == GanttDiagramMode)
    {
        switch(mode)
        {
        case seconds1:
            return UtcDateTime(QDate(dt.year(),dt.month(),dt.day()),QTime(dt.hour(),dt.minute()));
        case minutes1:
            return UtcDateTime(QDate(dt.year(),dt.month(),dt.day()),QTime(dt.hour(),0));
        case hours1:
            return UtcDateTime(QDate(dt.year(),dt.month(),dt.day()));
        case days1:
            return UtcDateTime(QDate(dt.year(),dt.month(),1));
        case months1:
            return UtcDateTime(QDate(dt.year(),1,1));
        default:
            return UtcDateTime();
        }
    }
    else if(m_headerMode == TimelineMode)
    {
        switch(mode)
        {
        case seconds1:
            return dt.dateTime();
        case seconds5:
        {
            int secBeg = dt.second()/5 * 5;
            return UtcDateTime(dt.date(),QTime(dt.hour(),dt.minute(),secBeg));
        }
        case seconds15:
        {
            int secBeg = dt.second()/15 * 15;
            return UtcDateTime(dt.date(),QTime(dt.hour(),dt.minute(),secBeg));
        }
        case seconds30:
        {
            int secBeg = dt.second()/30 * 30;
            return UtcDateTime(dt.date(),QTime(dt.hour(),dt.minute(),secBeg));
        }
        case minutes1:
            return UtcDateTime(QDate(dt.year(),dt.month(),dt.day()),QTime(dt.hour(),dt.minute()));
        case minutes5:
        {
            int minBeg = dt.minute()/5 * 5;
            return UtcDateTime(dt.date(),QTime(dt.hour(),minBeg));
        }
        case minutes15:
        {
            int minBeg = dt.minute()/15 * 15;
            return UtcDateTime(dt.date(),QTime(dt.hour(),minBeg));
        }
        case minutes30:
        {
            int minBeg = dt.minute()/30 * 30;
            return UtcDateTime(dt.date(),QTime(dt.hour(),minBeg));
        }
        case hours1:
            return UtcDateTime(QDate(dt.year(),dt.month(),dt.day()),QTime(dt.hour(),0));
        case hours6:
        {
            int hourBeg = dt.hour() / 6 * 6;
            return UtcDateTime(QDate(dt.year(),dt.month(),dt.day()),QTime(hourBeg,0));
        }
        case hours12:
        {
            int hourBeg = dt.hour() / 12 * 12;
            return UtcDateTime(QDate(dt.year(),dt.month(),dt.day()),QTime(hourBeg,0));
        }
        case days1:
            return UtcDateTime(dt.date());
        case months1:
            return UtcDateTime(QDate(dt.year(),dt.month(),1));
        case years1:
            return UtcDateTime(QDate(dt.year(),1,1));
        default:
            qWarning("GanttHeader::startByDt() out of range");
            return UtcDateTime();
        }
    }
    return UtcDateTime();
}

UtcDateTime GanttHeader::startByDt(const UtcDateTime &dt) const
{
    return startByDt(dt,m_mode);
}

UtcDateTime GanttHeader::finishByDt(const UtcDateTime &dt,GanttPrecisionMode mode) const
{
    if(m_isEmpty)
        return UtcDateTime();
    if(m_headerMode == GanttDiagramMode)
    {
        switch(mode)
        {
        case seconds1:
            return UtcDateTime(
                        (QDate(dt.year(),dt.month(),dt.day()))
                        ,QTime(dt.hour(),dt.minute()).addSecs(SECONDS_IN_MINUTE)
                        ).addMicroseconds(-1);
        case minutes1:
            return UtcDateTime(
                        (QDate(dt.year(),dt.month(),dt.day()))
                        ,QTime(dt.hour(),0).addSecs(SECONDS_IN_HOUR)
                        ).addMicroseconds(-1);
        case hours1:
            return UtcDateTime(
                        (QDate(dt.year(),dt.month(),dt.day())).addDays(1)
                        ).addMicroseconds(-1);
        case days1:
            return UtcDateTime(
                        (QDate(dt.year(),dt.month(),1)).addMonths(1)
                        ).addMicroseconds(-1);
        case months1:
            return UtcDateTime(
                        (QDate(dt.year(),1,1)).addYears(1)
                        ).addMicroseconds(-1);
        default:
            return UtcDateTime();
        }
    }
    else if(m_headerMode == TimelineMode)
    {
        switch(mode)
        {
        case seconds1:
            return UtcDateTime(
                        dt.dateTime()
                        );
//            if(minEnd < dt.minute())
//                m_finishDt.addSecs(1);
        case seconds5:
        {
            int secEnd = dt.second()/5 * 5;
            UtcDateTime res = UtcDateTime(
                        dt.date(),QTime(dt.hour(),dt.minute(),secEnd)
                        );
            if(secEnd < dt.second())
                res = res.addSecs(5);
            return res;
        }
        case seconds15:
        {
            int secEnd = dt.second()/15 * 15;
            UtcDateTime res = UtcDateTime(
                        dt.date(),QTime(dt.hour(),dt.minute(),secEnd)
                        );
            if(secEnd < dt.second())
                res = res.addSecs(15);
            return res;
        }
        case seconds30:
        {
            int secEnd = dt.second()/30 * 30;
            UtcDateTime res = UtcDateTime(
                        dt.date(),QTime(dt.hour(),dt.minute(),secEnd)
                        );
            if(secEnd < dt.second())
                res = res.addSecs(30);
            return res;
        }
        case minutes1:
        {
            UtcDateTime res = UtcDateTime(
                        (QDate(dt.year(),dt.month(),dt.day()))
                        ,QTime(dt.hour(),dt.minute())
                        );
            if(res < dt)
                res = res.addSecs(SECONDS_IN_MINUTE);
            return res;
        }
        case minutes5:
        {
            int minEnd = dt.minute()/5 * 5;
            UtcDateTime res = UtcDateTime(
                        dt.date(),QTime(dt.hour(),minEnd)
                        );
            if(minEnd < dt.minute())
                res = res.addSecs(5*SECONDS_IN_MINUTE);
            return res;
        }
        case minutes15:
        {
            int minEnd = dt.minute()/15 * 15;
            UtcDateTime res = UtcDateTime(
                        dt.date(),QTime(dt.hour(),minEnd)
                        );
            if(minEnd < dt.minute())
                res = res.addSecs(15*SECONDS_IN_MINUTE);
            return res;
        }
        case minutes30:
        {
            int minEnd = dt.minute()/30 * 30;
            UtcDateTime res = UtcDateTime(
                        dt.date(),QTime(dt.hour(),minEnd)
                        );
            if(minEnd < dt.minute())
                res = res.addSecs(30*SECONDS_IN_MINUTE);
            return res;
        }
        case hours1:
        {
            UtcDateTime res = UtcDateTime(
                        (QDate(dt.year(),dt.month(),dt.day()))
                        ,QTime(dt.hour(),0)
                        );
            if(res < dt)
                res = res.addSecs(SECONDS_IN_HOUR);
            return res;
        }
        case hours6:
        {
            int hourEnd = dt.hour() / 6 * 6;
            UtcDateTime res = UtcDateTime(
                        (QDate(dt.year(),dt.month(),dt.day()))
                        ,QTime(hourEnd,0)
                        );


            if(hourEnd < dt.hour())
                res = res.addSecs(modeToSecond(m_mode));

            return res;
        }
        case hours12:
        {
            int hourEnd = dt.hour() / 12 * 12;
            UtcDateTime res = UtcDateTime(
                        (QDate(dt.year(),dt.month(),dt.day()))
                        ,QTime(hourEnd,0)
                        );


            if(hourEnd < dt.hour())
                res = res.addSecs(modeToSecond(m_mode));

            return res;
        }
        case days1:
        {
            UtcDateTime res = UtcDateTime(
                        dt.date()
                        );
            if(res < dt)
                res = res.addDays(1);
            return res;
        }
        case months1:
        {
            UtcDateTime res = UtcDateTime(
                        (QDate(dt.year(),dt.month(),1))
                        );
            if(res < dt)
                res = res.addMonths(1);
            return res;
        }
        case years1:
        {
            UtcDateTime res = UtcDateTime(
                        (QDate(dt.year(),1,1))
                        );
            if(res < dt)
                res = res.addYears(1);
            return res;
        }
        default:
            qWarning("GanttHeader::finishByDt() out of range");
            return UtcDateTime();
        }
    }
    return UtcDateTime();
}

UtcDateTime GanttHeader::finishByDt(const UtcDateTime &dt) const
{
    return finishByDt(dt,m_mode);
}

UtcDateTime GanttHeader::nextStart(const UtcDateTime &start,GanttPrecisionMode mode) const
{
    if(!m_widget)
    {
        qWarning("GanttHeader::nextStart : NULL GanttWidget*");
        return start;
    }

    if(m_headerMode == GanttDiagramMode)
    {
        switch(mode)
        {
        case seconds1:
        {
            UtcDateTime res = start.addSecs( 1);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case minutes1:
        {
            UtcDateTime res = start.addSecs( SECONDS_IN_MINUTE);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case hours1:
        {
            UtcDateTime res = start.addSecs( SECONDS_IN_HOUR);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case days1:
        {
            UtcDateTime res = start.addDays(1);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case months1:
        {
            UtcDateTime res = start.addMonths(1);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case years1:
        {
            UtcDateTime res = start.addYears(1);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        default:
            return UtcDateTime();
        }
    }
    else if(m_headerMode == TimelineMode)
    {
        switch(mode)
        {
        case seconds1:
        {
            UtcDateTime res = start.addSecs(1);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case seconds5:
        {
            UtcDateTime res = start.addSecs(5);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case seconds15:
        {
            UtcDateTime res = start.addSecs(15);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case seconds30:
        {
            UtcDateTime res = start.addSecs(30);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case minutes1:
        {
            UtcDateTime res = start.addSecs(1 * SECONDS_IN_MINUTE);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case minutes5:
        {
            UtcDateTime res = start.addSecs(5 * SECONDS_IN_MINUTE);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case minutes15:
        {
            UtcDateTime res = start.addSecs(15 * SECONDS_IN_MINUTE);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case minutes30:
        {
            UtcDateTime res = start.addSecs(30 * SECONDS_IN_MINUTE);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case hours1:
        {
            UtcDateTime res = start.addSecs( 1 * SECONDS_IN_HOUR);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case hours6:
        {
            UtcDateTime res = start.addSecs( 6 * SECONDS_IN_HOUR);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case hours12:
        {
            UtcDateTime res = start.addSecs( 12 * SECONDS_IN_HOUR);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case days1:
        {
            UtcDateTime res = start.addDays(1);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case months1:
        {
            UtcDateTime res = start.addMonths(1);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        case years1:
        {
            UtcDateTime res = start.addYears(1);
            if(res > m_widget->maxDt())
                return start;
            return res;
        }
        default:
            qWarning("GanttHeader::nextStart out of range");
            return start;
        }
    }
    return start;
}

UtcDateTime GanttHeader::nextStart(const UtcDateTime &start) const
{
    return nextStart(start,m_mode);
}

UtcDateTime GanttHeader::prevFinish(const UtcDateTime &finish,GanttPrecisionMode mode) const
{
    if(!m_widget)
    {
        qWarning("GanttHeader::prevFinish : NULL GanttWidget*");
        return finish;
    }

    if(m_headerMode == GanttDiagramMode)
    {
        switch(mode)
        {
        case seconds1:
        {
            UtcDateTime res = finish.addSecs(- 1);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case minutes1:
        {
            UtcDateTime res = finish.addSecs(- SECONDS_IN_MINUTE);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case hours1:
        {
            UtcDateTime res = finish.addSecs(- SECONDS_IN_HOUR);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case days1:
        {
            UtcDateTime res = finish.addDays(-1);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case months1:
        {
            UtcDateTime res = finish.addMonths(-1);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case years1:
        {
            UtcDateTime res = finish.addYears(-1);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        default:
            return finish;
        }
    }
    else if(m_headerMode == TimelineMode)
    {
        switch(mode)
        {
        case seconds1:
        {
            UtcDateTime res = finish.addSecs(-1);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case seconds5:
        {
            UtcDateTime res = finish.addSecs(-5);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case seconds15:
        {
            UtcDateTime res = finish.addSecs(-15);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case seconds30:
        {
            UtcDateTime res = finish.addSecs(-30);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case minutes1:
        {
            UtcDateTime res = finish.addSecs(-1 * SECONDS_IN_MINUTE);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case minutes5:
        {
            UtcDateTime res = finish.addSecs(-5 * SECONDS_IN_MINUTE);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case minutes15:
        {
            UtcDateTime res = finish.addSecs(-15 * SECONDS_IN_MINUTE);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case minutes30:
        {
            UtcDateTime res = finish.addSecs(-30 * SECONDS_IN_MINUTE);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case hours1:
        {
            UtcDateTime res = finish.addSecs(- 1 * SECONDS_IN_HOUR);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case hours6:
        {
            UtcDateTime res = finish.addSecs(- 6 * SECONDS_IN_HOUR);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case hours12:
        {
            UtcDateTime res = finish.addSecs(- 12 * SECONDS_IN_HOUR);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case days1:
        {
            UtcDateTime res = finish.addDays(-1);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case months1:
        {
            UtcDateTime res = finish.addMonths(-1);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        case years1:
        {
            UtcDateTime res = finish.addYears(-1);
            if(res < m_widget->minDt())
                return finish;
            return res;
        }
        default:
            qWarning("GanttHeader::prevFinish out of range");
            return finish;
        }
    }
    return finish;
}

UtcDateTime GanttHeader::prevFinish(const UtcDateTime &finish) const
{
    return prevFinish(finish,m_mode);
}

QString GanttHeader::topHeaderFormat(GanttHeader::GanttPrecisionMode mode)
{
    switch (mode)
    {
    case seconds1:
        return ("ddd d MMM yy, hh:mm");
        break;
    case minutes1:
        return ("ddd d MMM yy, hh:00");
        break;
    case hours1:
        return ("ddd d MMM yy");
        break;
    case days1:
        return ("MMM yyyy");
        break;
    case months1:
        return ("yyyy");
        break;
    default:
        return QString();
    }
}

QString GanttHeader::secondHeaderFormat(GanttHeader::GanttPrecisionMode mode)
{
    switch (mode)
    {
    case seconds1:
        return ("s");
        break;
    case minutes1:
        return ("m");
        break;
    case hours1:
        return ("h");
        break;
    case days1:
        return ("d");
        break;
    case months1:
        return ("MMM");
        break;
    default:
        return QString();
    }
}

UtcDateTime GanttHeader::nextTopHeaderDt(const UtcDateTime &curDt,GanttHeader::GanttPrecisionMode mode)
{
    int i_mode = mode;
    if(i_mode)
        i_mode--;
    return curDt.addMicroseconds(
                modeToMicrosecond(static_cast<GanttPrecisionMode>(i_mode),curDt.date()));
}

UtcDateTime GanttHeader::nextSecondHeaderDt(const UtcDateTime &curDt,GanttHeader::GanttPrecisionMode mode)
{
    return curDt.addMicroseconds(modeToMicrosecond(mode,curDt.date()));
}

qreal GanttHeader::secondHeaderWidth() const
{
    if(m_mode == months1)
        return DEFAULT_MONTH_WIDTH;

    return DEFAULT_ITEM_WIDTH;
}

void GanttHeader::clear()
{
    foreach(QGraphicsRectItemWithText* item, m_items)
    {
        item->setParentItem(NULL);
        m_scene->removeItem(item);
        m_items.removeOne(item);
        delete item;
    }
    m_minDt = UtcDateTime();
    m_maxDt = UtcDateTime();
    m_isEmpty = true;
    m_scene->invalidate();
}

qreal GanttHeader::dtToX(const UtcDateTime &dt) const
{
    if(!m_scene)
        return 0;

    qreal sceneW = m_scene->width();

    return ((m_headerMode==TimelineMode)?(MIN_WIDTH_FOR_TIME_VISUALIZING/2):(0)) +
           ((1.0)*startDt().microsecondsTo(dt))/m_lengthInMicroseconds *
            (sceneW-((m_headerMode==TimelineMode)?(MIN_WIDTH_FOR_TIME_VISUALIZING):(0)));

}

UtcDateTime GanttHeader::xToDt(qreal x) const
{
    if(!m_scene)
    {
        Q_ASSERT(false);
        return m_startDt;
    }
    qreal sceneW;

    if(m_headerMode == TimelineMode)
    {
        sceneW = m_scene->width() - MIN_WIDTH_FOR_TIME_VISUALIZING;
        if(x<MIN_WIDTH_FOR_TIME_VISUALIZING/2)
            return m_startDt;

        x-=MIN_WIDTH_FOR_TIME_VISUALIZING/2;

        if(x>sceneW)
            return m_finishDt;
    }
    else if(m_headerMode == GanttDiagramMode)
    {
        sceneW = m_scene->width();
    }

    return m_startDt.addMicroseconds(m_startDt.microsecondsTo(m_finishDt) * (x / sceneW));
}

bool GanttHeader::onItemsAdditionHelper(GanttInfoItem *item)
{
    GanttInfoLeaf *leaf = dynamic_cast<GanttInfoLeaf*>(item);
    bool newRange = false;
    if(leaf)
    {
        if(m_isEmpty)
        {
            m_isEmpty = false;
            m_minDt = leaf->start();
            m_maxDt = leaf->finish();
        }

        newRange |= verifyBoundsByLeaf(leaf);
    }
    else
    {
        GanttInfoNode *node = dynamic_cast<GanttInfoNode*>(item);
        if(node)
        {
            if(node->hasCalcDt())
            {
                if(m_isEmpty)
                {
                    m_isEmpty = false;
                    m_minDt = node->calcDt();
                    m_maxDt = node->calcDt();
                }

                newRange |= verifyBoundsByNode(node);
            }

            foreach(GanttInfoItem* item, node->m_items)
            {
                newRange |= onItemsAdditionHelper(item);
            }
        }
    }

    return newRange;
}

QString GanttHeader::formatForMode(GanttHeader::GanttPrecisionMode mode)
{
    switch(mode)
    {
    case seconds1:
    case seconds5:
    case seconds15:
    case seconds30:
        return "mm:ss";
    case minutes1:
    case minutes5:
    case minutes15:
    case minutes30:
        return "hh:mm";
    case hours1:
    case hours6:
    case hours12:
        return "hh:00";

    case days1:
        return "dd";
    case months1:
        return "MMM";
    case years1:
        return "yyyy";
    default:
        qWarning("GanttHeader::formatForMode out of range");
        return QString();
    }
}

QString GanttHeader::textForDtStep(int step) const
{
    switch(m_mode)
    {
    case seconds1:
    case seconds5:
    case seconds15:
    case seconds30:
        return m_startDt.addSecs( modeToSecond(m_mode) * step).toString(formatForMode(m_mode));
    case minutes1:
    case minutes5:
    case minutes15:
    case minutes30:
        return m_startDt.addSecs(modeToSecond(m_mode) * step).toString(formatForMode(m_mode));
    case hours1:
    case hours6:
    case hours12:
        return m_startDt.addSecs(modeToSecond(m_mode) * step).toString(formatForMode(m_mode));

    case days1:
        return m_startDt.addDays(step).toString(formatForMode(m_mode));
    case months1:
        return m_startDt.addMonths(step).toString(formatForMode(m_mode));
    case years1:
        return m_startDt.addYears(step).toString(formatForMode(m_mode));
    default:
        qWarning("GanttHeader::textForDtStep out of range");
       return QString();
    }
    return QString();
}

void GanttHeader::updateWidget()
{
    if(!m_widget)
        return;

    m_widget->updateRange(m_minDt,m_maxDt);
}

//void GanttHeader::updateVisItemCount(qreal item_width)
//{
//    setVisItemCount((m_currentWidth-item_width)/item_width);
//}

qreal GanttHeader::calculateVisItemCount(qreal item_width) const
{
    if(!item_width)
        return 1;
    return (m_currentWidth-item_width)/item_width;
}

qreal GanttHeader::getCoef(const UtcDateTime &min, const UtcDateTime &max) const
{
    return (min.microsecondsTo(max) * 1.0 / (m_visItemCount));
}

void GanttHeader::updateStretchFactor()
{
    m_stretchFactor = modeToMicrosecond(m_mode) * 1.0 / getCoef(m_startDt,m_finishDt);
}
void GanttHeader::setEmpty(bool isEmpty)
{
    m_isEmpty = isEmpty;
}

void GanttHeader::setVisItemCount(int visItemCount)
{
    m_visItemCount = qMax(1,visItemCount);
}




const UtcDateTime &GanttHeader::maxDt() const
{
    return m_maxDt;
}

const UtcDateTime &GanttHeader::minDt() const
{
    return m_minDt;
}

void GanttHeader::setLengthInMicroseconds(long long lengthInMicroseconds)
{
    m_lengthInMicroseconds = lengthInMicroseconds;

}

const UtcDateTime &GanttHeader::startDt() const
{
//    if(m_headerMode == TimelineMode)
//        return m_minDt;
//    if(m_headerMode == GanttDiagramMode)
        return m_startDt;

}

const UtcDateTime &GanttHeader::finishDt() const
{
//    if(m_headerMode == TimelineMode)
//        return m_maxDt;
//    if(m_headerMode == GanttDiagramMode)
        return m_finishDt;

}

void GanttHeader::setRange(UtcDateTime min, UtcDateTime max)
{
    if(min>max)
        return;

    m_minDt = min;
    m_maxDt = max;

    updateHeader();
}

bool GanttHeader::verifyBoundsByLeaf(const GanttInfoLeaf *leaf)
{
    if(!leaf)
        return false;
    if(!leaf->start().isValid() || !leaf->finish().isValid())
    {
        qWarning("Leaf\'s time not valid!");
        return false;
    }

    bool newRange = false;
    if(m_minDt > leaf->start())
    {
        m_minDt = leaf->start();
        newRange = true;
    }

    if(m_maxDt < leaf->finish())
    {
        m_maxDt = leaf->finish();
        newRange = true;
    }

    return newRange;
}

bool GanttHeader::verifyBoundsByNode(const GanttInfoNode *node)
{
    if(!node)
        return false;
    if(!node->hasCalcDt())
    {
        return false;
    }

    bool newRange = false;
    if(m_minDt > node->calcDt())
    {
        m_minDt = node->calcDt();
        newRange = true;
    }

    if(m_maxDt < node->calcDt())
    {
        m_maxDt = node->calcDt();
        newRange = true;
    }

    return newRange;
}

long long GanttHeader::modeToMicrosecond(GanttPrecisionMode mode, const QDate& date)
{
    switch(mode)
    {
//    case microseconds1:
//        return 1;
//    case microseconds15:
//        return 15;
//    case microseconds30:
//        return 30;
//    case miliseconds1:
//        return _MICROSECONDS_IN_MILISECOND;
//    case miliseconds15:
//        return 15*_MICROSECONDS_IN_MILISECOND;
//    case miliseconds30:
//        return 30*_MICROSECONDS_IN_MILISECOND;
    case seconds1:
        return _MICROSECONDS_IN_SECOND;
    case seconds5:
        return 5*_MICROSECONDS_IN_SECOND;
    case seconds15:
        return 15*_MICROSECONDS_IN_SECOND;
    case seconds30:
        return 30*_MICROSECONDS_IN_SECOND;
    case minutes1:
        return SECONDS_IN_MINUTE*_MICROSECONDS_IN_SECOND;
    case minutes5:
        return 5*SECONDS_IN_MINUTE*_MICROSECONDS_IN_SECOND;
    case minutes15:
        return 15*SECONDS_IN_MINUTE*_MICROSECONDS_IN_SECOND;
    case minutes30:
        return 30*SECONDS_IN_MINUTE*_MICROSECONDS_IN_SECOND;
    case hours1:
        return ((long long)SECONDS_IN_HOUR)*_MICROSECONDS_IN_SECOND;
    case hours6:
        return 6*((long long)SECONDS_IN_HOUR)*_MICROSECONDS_IN_SECOND;
    case hours12:
        return 12*((long long)SECONDS_IN_HOUR)*_MICROSECONDS_IN_SECOND;
    case days1:
        return ((long long)SECONDS_IN_DAY)*_MICROSECONDS_IN_SECOND;
    case months1:
        if(!date.isValid())
            return 28*((long long)SECONDS_IN_DAY)*_MICROSECONDS_IN_SECOND;
        return date.daysInMonth()*((long long)SECONDS_IN_DAY)*_MICROSECONDS_IN_SECOND;
    case years1:
        if(!date.isValid())
            return 345*((long long)SECONDS_IN_DAY)*_MICROSECONDS_IN_SECOND;
        return date.daysInYear()*((long long)SECONDS_IN_DAY)*_MICROSECONDS_IN_SECOND;
    default:
        qWarning("long long GanttHeader::modeToMicrosecond(GanttHeader::GanttHeaderMode mode) out of range");
        return 0;
    }
}

long long GanttHeader::modeToSecond(GanttHeader::GanttPrecisionMode mode, const QDate& date)
{
    switch(mode)
    {
    case seconds1:
        return 1;
    case seconds5:
        return 5;
    case seconds15:
        return 15;
    case seconds30:
        return 30;
    case minutes1:
        return SECONDS_IN_MINUTE;
    case minutes5:
        return 5*SECONDS_IN_MINUTE;
    case minutes15:
        return 15*SECONDS_IN_MINUTE;
    case minutes30:
        return 30*SECONDS_IN_MINUTE;
    case hours1:
        return ((long long)SECONDS_IN_HOUR);
    case hours6:
        return 6*((long long)SECONDS_IN_HOUR);
    case hours12:
        return 12*((long long)SECONDS_IN_HOUR);
    case days1:
        return ((long long)SECONDS_IN_DAY);
    case months1:
        if(!date.isValid())
            return 28*((long long)SECONDS_IN_DAY);
        return date.daysInMonth()*((long long)SECONDS_IN_DAY);
    case years1:
        if(!date.isValid())
            return 345*((long long)SECONDS_IN_DAY);
        return date.daysInYear()*((long long)SECONDS_IN_DAY);
    default:
        qWarning("GanttHeader::modeToSecond out of range");
        return 0;
    }
}

int GanttHeader::modeToSegmentCount(GanttHeader::GanttPrecisionMode mode, const QDate& date)
{
    switch(mode)
    {
    case seconds1:
        return 1;
    case seconds5:
        return 5;
    case seconds15:
        return 3;
    case seconds30:
        return 6;
    case minutes1:
        return 6;
    case minutes5:
        return 5;
    case minutes15:
        return 3;
    case minutes30:
        return 6;
    case hours1:
        return 6;
    case hours6:
        return 6;
    case hours12:
        return 6;
    case days1:
        return 6;
    case months1:
        if(!date.isValid())
            return 6;
        return date.daysInMonth();
    case years1:
        return 12;
    default:
        qWarning("GanttHeader::modeToSecond out of range");
        return 1;
    }
}

bool GanttHeader::isDrawn(const UtcDateTime &dt, GanttHeader::GanttPrecisionMode mode)
{
    if(!dt.isValid())
    {
        qWarning("GanttHeader::isDrawn dt is not valid");
        return false;
    }

    switch(mode)
    {
    case seconds1:
        return true;
    case seconds5:
        return !(dt.second()%5);
    case seconds15:
        return !(dt.second()%15);
    case seconds30:
        return !(dt.second()%30);
    case minutes1:
        return !(dt.second());
    case minutes5:
        return !(dt.minute()%5);
    case minutes15:
        return !(dt.minute()%15);
    case minutes30:
        return !(dt.minute()%30);
    case hours1:
        return !(dt.minute());
    case hours6:
        return !(dt.hour()%6);
    case hours12:
        return !(dt.hour()%12);
    case days1:
        return !(dt.hour());
    case months1:
        return !(dt.day() - 1);
    case years1:
        return !(-dt.daysTo(UtcDateTime(QDate(dt.year(),1,1))));
    default:
        qWarning("GanttHeader::isDrawn out of range");
        return false;
    }
}

void GanttHeader::setCurrentWidth(int currentWidth)
{
    if(m_currentWidth == currentWidth)
        return;
    m_currentWidth = currentWidth;
    setVisItemCount(calculateVisItemCount(MIN_WIDTH_FOR_TIME_VISUALIZING));

    updateHeader();

}

GanttHeader::GanttHeaderMode GanttHeader::headerMode() const
{
    return m_headerMode;
}

GanttHeader::GanttPrecisionMode GanttHeader::calculateTimeMode(const UtcDateTime &min, const UtcDateTime &max)
{
    if(m_isEmpty || m_headerMode != TimelineMode || m_currentWidth < MIN_WIDTH_FOR_TIME_VISUALIZING)
        return (GanttPrecisionMode)0;

    qreal coef = (min.microsecondsTo(max)*1.0/ (m_visItemCount));

    GanttPrecisionMode mode = GanttPrecisionMode_count;

    for(int i = GanttPrecisionMode_count-1; i >= 0; --i)
    {
        GanttPrecisionMode i_mode = static_cast<GanttPrecisionMode>(i);
        qreal monthCoef;
        if(i_mode == months1)
            monthCoef = (min.microsecondsTo(max)*1.0/ calculateVisItemCount(MIN_WIDTH_FOR_MONTH_VISUALIZING));
        const qreal &usageCoef = (i_mode == months1)?(monthCoef):(coef);

        if(usageCoef <= modeToMicrosecond(i_mode))
        {
            mode = i_mode;
            break;
        }
    }

    if(mode == GanttPrecisionMode_count)
    {
        qWarning("GanttHeader::calculateTimeMode() out of range");

        return (GanttPrecisionMode)0;
    }

    return mode;
}

bool GanttHeader::setHeaderMode(const GanttHeaderMode &headerMode)
{
    if(headerMode == m_headerMode)
        return false;
    m_headerMode = headerMode;

    if(!m_scene)
        return false;

    GanttGraphicsView *p_view;

    if(!m_scene->views().isEmpty())
        p_view = dynamic_cast<GanttGraphicsView*>(m_scene->views()[0]);

    if(headerMode == TimelineMode)
    {
        clear();
    }
    else if(headerMode == GanttDiagramMode)
    {
        if(m_mode == seconds5)
            setMode(seconds1);
        if (m_mode == seconds15 || m_mode == seconds30 || m_mode == minutes5)
            setMode(minutes1);
        else if(m_mode == minutes15 || m_mode == minutes30)
            setMode(hours1);
        else if (m_mode == hours6 || m_mode == hours12)
            setMode(days1);
    }

    init();

    updateHeader();

    return true;
}

void GanttHeader::init()
{
    if(!m_scene)
        return;

    GanttGraphicsView *p_view = NULL;

    if(!m_scene->views().isEmpty())
        p_view = dynamic_cast<GanttGraphicsView*>(m_scene->views()[0]);

    if(!p_view)
        return;

    if(m_headerMode == GanttDiagramMode)
    {
        p_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    else if(m_headerMode == TimelineMode)
    {
        p_view->setMaximumWidth(100000);
        p_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

