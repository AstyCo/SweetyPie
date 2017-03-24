/*!
 * \file
 * \~russian
 * \brief Файл содержит расширения TimeSpan class
 * \~englist
 * \brief File contains extensions of TimeSpan class
 */
#ifndef TIMESPAN_EXTENSION_H
#define TIMESPAN_EXTENSION_H

#include "ganttproject_global.h"
#include "utcdatetime.h"

bool timeSpanIsValid(const TimeSpan &ts);

TimeSpan GANTTMODELSHARED_EXPORT operator*(const TimeSpan &ts, const qreal d);
TimeSpan GANTTMODELSHARED_EXPORT operator*(const qreal d, const TimeSpan &ts);
TimeSpan GANTTMODELSHARED_EXPORT operator/(const TimeSpan &ts, const qreal d);


#endif // TIMESPAN_EXTENSION_H
