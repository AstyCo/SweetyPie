/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию GanttIntervalSlider class.
 * \~englist
 * \brief File contains realization of GanttIntervalSlider class.
 */
#include "ganttintervalslider.h"
#include "utcdatetime.h"
#include "timespan.h"
#include "extensions/timespan_extension.h"
#include "gantt/private_extensions/gantt-lib_global_values.h"

#include <QPainter>
#include <QLinearGradient>
#include <QBrush>
#include <QFrame>
#include <QPen>

#include <QMouseEvent>

#include <QDebug>

void GanttIntervalSlider::init()
{
    _blockChangeSavedLimits = false;
    m_drawCurrentDt = false;
    m_shiftRange = 0;
    m_minTimeSize = 30*_MICROSECONDS_IN_SECOND;
    setOffsetV(-3);
    setHandleSize(6);
    setSliderV(20);

    setCurrentTimeRectWidth(4);
    setCurrentTimeRectColor(Qt::red);

    setStyleSheet("background-color: rgb(21, 168, 194)");

    setMouseTracking(true);

    connect(this,SIGNAL(beginMovedManually(long long)),this,SLOT(emitRangeChangedManually()));
    connect(this,SIGNAL(endMovedManually(long long)),this,SLOT(emitRangeChangedManually()));

}

GanttIntervalSlider::GanttIntervalSlider(QWidget *parent )
    : IntervalSlider(parent)
{
    init();
}

void GanttIntervalSlider::drawHandle(QPainter *painter, const QRect &handleRect, bool is_selected) const
{
    painter->setPen(Qt::black);
    qreal penWidth = 1.0;
    QColor color = (is_selected)?(HEADER_COLOR):(SLIDER_COLOR);
    QPen pen(Qt::black,penWidth,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
    painter->setPen(pen);

    painter->setBrush(QBrush(color));
    painter->drawRoundedRect(handleRect.adjusted(1,1,-1,-1)
                             ,1,1);


}

void GanttIntervalSlider::drawSliderLine(QPainter *painter, const QRect &sliderRect) const
{
    int top = sliderRect.y() + (m_offsetV>0?m_offsetV:0),
            width = sliderRect.width() - handleSize();

    int beginRectLeft = sliderRect.x() + halfHandleSize(),
            beginRectWidth = valueToPoint(m_beginValue,BeginHandle) - halfHandleSize() - sliderRect.x(),
            innerRectLeft = beginRectLeft + beginRectWidth,
            innerRectWidth = valueToPoint(m_endValue,EndHandle) - beginRectWidth - handleSize()/2,
            endRectLeft = innerRectLeft + innerRectWidth,
            endRectWidth = width - valueToPoint(m_endValue,EndHandle);
    QLinearGradient foregroundGradient(QPoint(0,top),QPoint(0,top + m_sliderV));
    QColor sliderColor = SLIDER_COLOR;
    foregroundGradient.setColorAt(0,sliderColor);
    foregroundGradient.setColorAt(0.5,sliderColor.lighter(180));
    foregroundGradient.setColorAt(1,sliderColor);

    painter->setPen(Qt::gray);

    painter->fillRect(QRect(innerRectLeft,
                            top,
                            innerRectWidth,
                            m_sliderV
                            ), foregroundGradient);

    painter->drawRect(QRect(beginRectLeft,
                            top,
                            width,
                            m_sliderV
                            ));


    if(beginRectWidth > 0)
    {
        QRect beforeBeginRect(beginRectLeft+1,
                              top+1,
                              beginRectWidth,
                              m_sliderV-1);

        painter->fillRect(beforeBeginRect,  QBrush(sliderColor.lighter(190)));

    }


    if(endRectWidth > 0)
    {
        QRect afterEndRect(endRectLeft,
                        top+1,
                        endRectWidth-1,
                        m_sliderV-1);

        painter->fillRect(afterEndRect,  QBrush(sliderColor.lighter(190)));
    }

    if(!outOfLimits(m_currentTime))
        drawCurrentTime(painter,sliderRect);
}

void GanttIntervalSlider::drawCurrentTime(QPainter *painter, const QRect &sliderRect) const
{
    if(!m_drawCurrentDt)
        return;

    QRect currentTimeRect(valueToPoint(dtToVal(m_currentTime),NoHandle)
                          - (m_currentTimeRectWidth)/2,
                          sliderRect.y(),
                          m_currentTimeRectWidth,
                          intervalSliderHeight()
                          );

    painter->fillRect(currentTimeRect.adjusted(0,0,0,-1), QBrush(m_currentTimeRectColor));
}

void GanttIntervalSlider::mouseDoubleClickEvent(QMouseEvent *e)
{
    _blockChangeSavedLimits = true;
    setLimits(_savedLimits.min(), _savedLimits.timeSpan());
    _blockChangeSavedLimits = true;
    IntervalSlider::mouseDoubleClickEvent(e);
}

long long GanttIntervalSlider::minTimeSize() const
{
    return m_minTimeSize;
}

void GanttIntervalSlider::setBeginHandle(long long beginHandle, bool manually)
{
    if(endHandle() - beginHandle < minTimeSize())
        beginHandle = endHandle() - minTimeSize();
    IntervalSlider::setBeginHandle(beginHandle, manually);

}

void GanttIntervalSlider::setEndHandle(long long endHandle, bool manually)
{
    if(endHandle - beginHandle() < minTimeSize())
        endHandle = beginHandle() + minTimeSize();
    IntervalSlider::setEndHandle(endHandle, manually);
}

void GanttIntervalSlider::reset()
{
    setLimits(m_minValue,m_maxValue);
}

UtcDateTime GanttIntervalSlider::left() const
{
    return valToDt(m_beginValue);
}

UtcDateTime GanttIntervalSlider::right() const
{
    return valToDt(m_endValue);
}

TimeSpan GanttIntervalSlider::timeSpan() const
{
    return right() - left();
}

UtcDateTime GanttIntervalSlider::min() const
{
    return valToDt(m_minValue);
}

UtcDateTime GanttIntervalSlider::max() const
{
    return valToDt(m_maxValue);
}

void GanttIntervalSlider::setLeft(const UtcDateTime &dt)
{
    setBeginHandle(dtToVal(dt));
}

void GanttIntervalSlider::setRight(const UtcDateTime &dt)
{
    setEndHandle(dtToVal(dt));

}

void GanttIntervalSlider::setMin(const UtcDateTime &dt)
{
    setLimits(dtToVal(dt),m_maxValue);
}

void GanttIntervalSlider::setTimeSpan(const TimeSpan &ts)
{
    setLimits(m_minValue,dtToVal(ts + valToDt(m_minValue)));
}

void GanttIntervalSlider::setRange(const UtcDateTime &min, const TimeSpan &ts)
{
    setBeginHandle(dtToVal(min));
    setEndHandle(dtToVal(min+ts));
}

void GanttIntervalSlider::setRangeWithExpansion(const UtcDateTime &min, const TimeSpan &ts)
{
    long long newMin, newMax;
    bool needChangeLimits = false;

    if(min < valToDt(m_minValue)){
        needChangeLimits = true;
        newMin = dtToVal(min);
    }
    else
        newMin = m_minValue;

    if(min + ts > valToDt(m_maxValue)){
        needChangeLimits = true;
        newMax = dtToVal(min + ts);
    }
    else
        newMax = m_maxValue;

    if(needChangeLimits){
        _blockChangeSavedLimits = true;
        setLimits(newMin, newMax);
        _blockChangeSavedLimits = false;
    }
    setRange(min, ts);
}

void GanttIntervalSlider::setLimits(long long min, long long max)
{
    if(min > max)
        return;
    if(max - min < m_minTimeSize)
        max = min + m_minTimeSize;
    IntervalSlider::setLimits(min,max);

    if(!_blockChangeSavedLimits){
        const UtcDateTime tmpMin = valToDt(min);
        _savedLimits = MyUtcDateTimeInterval(tmpMin, valToDt(max) - tmpMin);
    }
}

void GanttIntervalSlider::setMinTimeSize(long long minTimeSize)
{
    m_minTimeSize = minTimeSize;
}

UtcDateTime GanttIntervalSlider::valToDt(long long val) const
{
    return UtcDateTime::fromMicrosecondsSinceEpoch(val);
}

long long GanttIntervalSlider::dtToVal(const UtcDateTime &dt) const
{
    return dt.toMicrosecondsSinceEpoch();
}

UtcDateTime GanttIntervalSlider::beginDt() const
{
    return valToDt(beginHandle());
}

UtcDateTime GanttIntervalSlider::endDt() const
{
    return valToDt(endHandle());
}


bool GanttIntervalSlider::outOfLimits(const UtcDateTime &dt) const
{
    return dt<valToDt(m_minValue) || dt > valToDt(m_maxValue);
}

void GanttIntervalSlider::emitRangeChanged()
{
    emit rangeChanged(left(),timeSpan());
}

void GanttIntervalSlider::emitRangeChangedManually()
{
    emit rangeChangedManually(left(),timeSpan());
}

void GanttIntervalSlider::setLimits(const UtcDateTime &min, const TimeSpan &ts)
{
    setLimits(dtToVal(min),dtToVal(min + ts));
}

void GanttIntervalSlider::setLimitsWithOffset(const UtcDateTime &min, const TimeSpan &ts)
{
    static const qreal coef = 0.1;
    TimeSpan deltaTs = ts * coef;
    setLimits(min - deltaTs, ts + 2 * deltaTs);
}

void GanttIntervalSlider::setCurrentTimeRectColor(const QColor &currentTimeRectColor)
{
    m_currentTimeRectColor = currentTimeRectColor;

    update();
}

void GanttIntervalSlider::setCurrentTime(const UtcDateTime &dt)
{
    m_currentTime  = dt;

    update();
}

void GanttIntervalSlider::setDrawCurrentDt(bool draw)
{
    m_drawCurrentDt = draw;

    update();
}

void GanttIntervalSlider::setCurrentTimeRectWidth(const qreal &currentTimeRectWidth)
{
    m_currentTimeRectWidth = currentTimeRectWidth;

    update();
}

