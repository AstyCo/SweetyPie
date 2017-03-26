/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление UtcDateTimeInterval class.
 * \~englist
 * \brief File contains definition of UtcDateTimeInterval class.
 */
#ifndef UTCDATETIMEINTERVAL_H
#define UTCDATETIMEINTERVAL_H

#include "interval.h"
#include "utcdatetime.h"
#include "timespan.h"

/*!
 * \~russian
 * \brief Класс UtcDateTimeInterval class представляет UTCDateTime class интервал
 * \~english
 * \brief The UtcDateTimeInterval class class represents UTCDateTime class interval
 */
class EXTENSIONS_LIBRARY_EXPORT UtcDateTimeInterval: public Interval<UtcDateTime, TimeSpan>
{
public:
  UtcDateTimeInterval()
  { }

  UtcDateTimeInterval(const UtcDateTime &begin, const UtcDateTime &end)
    : Interval<UtcDateTime, TimeSpan>(begin, end)
  { }

  UtcDateTimeInterval(const UtcDateTime &begin, const TimeSpan &length)
    : Interval<UtcDateTime, TimeSpan>(begin, begin + length)
  { }

  UtcDateTimeInterval(const UtcDateTimeInterval &other)
    : Interval<UtcDateTime, TimeSpan>(other.begin(), other.end())
  { }

  UtcDateTimeInterval(const Interval<UtcDateTime, TimeSpan> &other)
    : Interval<UtcDateTime, TimeSpan>(other)
  { }

  bool isValid() const { return ((! isEmpty()) && begin().isValid() && end().isValid()); }

  UtcDateTimeInterval operator+(const UtcDateTimeInterval &other) const
  {
    UtcDateTimeInterval temp(*this);
    temp += other;
    return temp;
  }
};


#endif // UTCDATETIMEINTERVAL_H
