#ifndef TIMESPAN_EXTENSION_H
#define TIMESPAN_EXTENSION_H

#include "gantt-static-lib-global.h"
#include "utcdatetime.h"

bool timeSpanIsValid(const TimeSpan &ts);

TimeSpan GANTTSTATICLIB_EXPORT operator*(const TimeSpan &ts, const qreal d);
TimeSpan GANTTSTATICLIB_EXPORT operator*(const qreal d, const TimeSpan &ts);
TimeSpan GANTTSTATICLIB_EXPORT operator/(const TimeSpan &ts, const qreal d);


#endif // TIMESPAN_EXTENSION_H
