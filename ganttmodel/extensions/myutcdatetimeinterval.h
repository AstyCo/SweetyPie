#ifndef MYUTCDATETIMEINTERVAL_H
#define MYUTCDATETIMEINTERVAL_H

#include "ganttproject_global.h"
#include "utcdatetime.h"
#include <QPair>


class MyUtcDateTimeInterval
{
public:
    MyUtcDateTimeInterval();
    MyUtcDateTimeInterval(const UtcDateTime &min, const TimeSpan &ts);

    MyUtcDateTimeInterval &operator|=(const MyUtcDateTimeInterval &interval);

    bool intersects(const MyUtcDateTimeInterval &another) const;
    MyUtcDateTimeInterval intersection(const MyUtcDateTimeInterval &another) const;
    inline UtcDateTime end() const;
    inline bool isValid() const;

    inline const UtcDateTime &min() const;
    inline void setMin(const UtcDateTime &newValue);
    inline const TimeSpan &timeSpan() const;
    inline void setTimeSpan(const TimeSpan &newValue);

private:
    UtcDateTime _min;
    TimeSpan _timeSpan;
};

GANTTMODELSHARED_EXPORT QDebug operator<<(QDebug d, const MyUtcDateTimeInterval &u);
// INLINE FUNCTIONS

inline UtcDateTime MyUtcDateTimeInterval::end() const { return _min + _timeSpan;}

inline bool MyUtcDateTimeInterval::isValid() const { return _timeSpan.totalMicroseconds()!=0;}

inline const UtcDateTime &MyUtcDateTimeInterval::min() const { return _min;}

inline void MyUtcDateTimeInterval::setMin(const UtcDateTime &newValue) { _min = newValue;}

inline const TimeSpan &MyUtcDateTimeInterval::timeSpan() const { return _timeSpan;}

inline void MyUtcDateTimeInterval::setTimeSpan(const TimeSpan &newValue) { _timeSpan = newValue;}


#endif // MYUTCDATETIMEINTERVAL_H
