/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию MyUtcDateTimeInterval class.
 * \~englist
 * \brief File contains realization of MyUtcDateTimeInterval class.
 */
#include "myutcdatetimeinterval.h"

MyUtcDateTimeInterval::MyUtcDateTimeInterval()
{

}

MyUtcDateTimeInterval::MyUtcDateTimeInterval(const UtcDateTime &min, const TimeSpan &ts):
    _min(min), _timeSpan(ts)
{

}

MyUtcDateTimeInterval &MyUtcDateTimeInterval::operator|=(const MyUtcDateTimeInterval &interval)
{
    if(intersects(interval)){
        UtcDateTime thisEnd = _min + _timeSpan,
                    anotherEnd = interval._min + interval._timeSpan;
        _min = qMin(_min, interval._min);
        _timeSpan = qMax(thisEnd, anotherEnd) - _min;
    }
    return *this;
}

bool MyUtcDateTimeInterval::intersects(const MyUtcDateTimeInterval &another) const
{
    bool thisBeginsLater = _min > another._min;
    const MyUtcDateTimeInterval &outer = (thisBeginsLater
                             ? another
                             : *this ),
                   &inner = (thisBeginsLater
                             ? *this
                             : another );

    return inner._min < outer.end();
}

MyUtcDateTimeInterval MyUtcDateTimeInterval::intersection(const MyUtcDateTimeInterval &another) const
{
    bool thisBeginsLater = _min > another._min;
    const MyUtcDateTimeInterval &outer = (thisBeginsLater
                             ? another
                             : *this ),
                   &inner = (thisBeginsLater
                             ? *this
                             : another );
    if(inner._min < outer.end())
        return MyUtcDateTimeInterval(inner._min, qMin(inner.end(), outer.end()) - inner._min);
    return MyUtcDateTimeInterval();
}

QDebug operator<<(QDebug d, const MyUtcDateTimeInterval &u)
{
    return d << "MyUtcDateTimeInterval(" << u.min() << u.timeSpan() << ')';
}

QDataStream &operator<<(QDataStream &ds, const MyUtcDateTimeInterval &i)
{
    long long mcsecs = i._timeSpan.totalMicroseconds();
    long long mcsecsFromEpoch = i._min.toMicrosecondsSinceEpoch();

    ds << mcsecs;
    ds << mcsecsFromEpoch;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, MyUtcDateTimeInterval &i)
{
    long long mcsecs;
    long long mcsecsFromEpoch;

    ds >> mcsecs;
    ds >> mcsecsFromEpoch;

    i._timeSpan = TimeSpan::fromMicroseconds(mcsecs);
    i._min = UtcDateTime::fromMicrosecondsSinceEpoch(mcsecsFromEpoch);

    return ds;
}
