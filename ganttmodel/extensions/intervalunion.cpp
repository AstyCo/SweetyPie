#include "intervalunion.h"
#include "extensions/timespan_extension.h"

#include <QTransform>

IntervalUnion::IntervalUnion()
{
}

IntervalUnion::IntervalUnion(const QList<MyUtcDateTimeInterval > &intervals)
    : _intervals(intervals)
{

}

void IntervalUnion::join(const MyUtcDateTimeInterval  &newInterval)
{
//    qDebug() << "to" << *this
//             << "join" << newInterval;
    if(!newInterval.isValid())
        return;
//    qDebug() << "isValid";
    for(int i = 0; i < _intervals.size(); ++i){
        if(_intervals[i].intersects(newInterval)){
//            qDebug() << "before"<<_intervals[i]<<"|="<<newInterval;
            _intervals[i] |= newInterval;
//            qDebug() << "after"<<_intervals[i];
            join(i,i+1);
            return;
        }
    }
//    qDebug() << "IntervalUnion::join NO INTERSECTION, ADD NEW";
    // NO INTERSECTION, ADD NEW
    _intervals.append(newInterval);
}





IntervalUnion &IntervalUnion::operator|=(const MyUtcDateTimeInterval &Interval)
{
//    qDebug() << "IntervalUnion::operator|=(const MyUtcDateTimeInterval &Interval)";
    join(Interval);
    return *this;
}

IntervalUnion &IntervalUnion::operator*=(qreal coef)
{
    IntervalUnion res = *this * coef;
    return *this = res;
}

void IntervalUnion::join(int index, int i)
{
    if(index >= i){
        qCritical("critical:: IntervalUnion::join index >= i");
        return;
    }
    for(; i < _intervals.size(); ++i){
        if(_intervals[i].intersects(_intervals[index])){
            _intervals[index] |= _intervals[i];
            _intervals.removeAt(i);
            --i;
        }
    }
}

QDebug operator<<(QDebug d, const IntervalUnion &u)
{
    d << QString("IntervalUnion( sz=%1,").arg(QString::number(u.size()));
    foreach(const MyUtcDateTimeInterval &Interval, u.intervals())
       d << Interval;
    d << ')';
    return d;
}

IntervalUnion operator*(const IntervalUnion &f, qreal coef)
{
    IntervalUnion res(f);

    foreach(const MyUtcDateTimeInterval &interval, f.intervals())
        res._intervals.append(MyUtcDateTimeInterval(interval.min(), interval.timeSpan() * coef));
    return res;
}

IntervalUnion operator*(qreal coef, const IntervalUnion &f)
{
    return f * coef;
}

