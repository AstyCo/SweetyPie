#ifndef INTERVALUNION_H
#define INTERVALUNION_H


#include "gantt-static-lib-global.h"

#include "extensions/myutcdatetimeinterval.h"
#include "utcdatetime.h"
#include <QPainter>
#include <QList>
#include <QDebug>

class GANTTSTATICLIB_EXPORT IntervalUnion
{
    friend IntervalUnion operator*(const IntervalUnion &f, qreal coef);
public:
    IntervalUnion();
    IntervalUnion(const QList<MyUtcDateTimeInterval> &intervals);

//    void fillRects(QPainter *painter, const QBrush &brush);
    void join(const MyUtcDateTimeInterval &newInterval);
    inline bool isEmpty() const;
    inline int size() const;
    inline void clear();
    inline const QList<MyUtcDateTimeInterval> &intervals() const;
    IntervalUnion &operator|=(const MyUtcDateTimeInterval &rect);
    IntervalUnion &operator*=(qreal coef);
private:
    void join(int index, int i = 0);

    QList<MyUtcDateTimeInterval> _intervals;
};

// INLINE FUNCTIONS
bool IntervalUnion::isEmpty() const { return _intervals.isEmpty();}

int IntervalUnion::size() const { return _intervals.size();}

void IntervalUnion::clear() { _intervals.clear();}

const QList<MyUtcDateTimeInterval> &IntervalUnion::intervals() const { return _intervals;}


GANTTSTATICLIB_EXPORT QDebug operator<<(QDebug, const IntervalUnion&);
GANTTSTATICLIB_EXPORT IntervalUnion operator*(const IntervalUnion &f, qreal coef);
GANTTSTATICLIB_EXPORT IntervalUnion operator*(qreal coef, const IntervalUnion &f);

#endif // INTERVALUNION_H
