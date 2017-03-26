/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию TimeSpan class.
 * \~englist
 * \brief File contains realisation of TimeSpan class.
 */
#include "timespan.h"

#include <cmath>

#include <QObject>
#include <QDebug>

const long long TimeSpan::MICROSECONDS_IN_MILLISECONDS = 1000;
const long long TimeSpan::MICROSECONDS_IN_SECOND = 1000 * TimeSpan::MICROSECONDS_IN_MILLISECONDS;
const long long TimeSpan::MICROSECONDS_IN_MINUTE = 60 * TimeSpan::MICROSECONDS_IN_SECOND;
const long long TimeSpan::MICROSECONDS_IN_HOUR = 60 * TimeSpan::MICROSECONDS_IN_MINUTE;
const long long TimeSpan::MICROSECONDS_IN_DAY = 24 * TimeSpan::MICROSECONDS_IN_HOUR;

const int TimeSpan::MAX_SECOND_FRAC_DIGITS = 6;

TimeSpan::TimeSpan()
{
  m_microseconds = 0;
}

TimeSpan::TimeSpan(long long microseconds)
{
  m_microseconds = microseconds;
}

TimeSpan::TimeSpan(int hours, int minutes, int seconds)
{
  m_microseconds = hours * MICROSECONDS_IN_HOUR + minutes * MICROSECONDS_IN_MINUTE
                    + seconds * MICROSECONDS_IN_SECOND;
}

TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds)
{
  m_microseconds = days * MICROSECONDS_IN_DAY + hours * MICROSECONDS_IN_HOUR
                   + minutes * MICROSECONDS_IN_MINUTE + seconds * MICROSECONDS_IN_SECOND;
}

TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds, int microseconds)
{
  m_microseconds = days * MICROSECONDS_IN_DAY + hours * MICROSECONDS_IN_HOUR
      + minutes * MICROSECONDS_IN_MINUTE + seconds * MICROSECONDS_IN_SECOND + microseconds;
}

TimeSpan::TimeSpan(const TimeSpan &other)
{
  m_microseconds = other.m_microseconds;
}

TimeSpan::TimeSpan(double val, TimeSpan::TimeIntervalTypes intType)
{
  m_microseconds = toMicroseconds(val, intType);
}

double TimeSpan::totalDays() const
{
  long long days = m_microseconds / MICROSECONDS_IN_DAY;
  double frac = double(m_microseconds % MICROSECONDS_IN_DAY) / MICROSECONDS_IN_DAY;

  return days + frac;
}

double TimeSpan::totalHours() const
{
  long long hours = m_microseconds / MICROSECONDS_IN_HOUR;
  double frac = double(m_microseconds % MICROSECONDS_IN_HOUR) / MICROSECONDS_IN_HOUR;

  return hours + frac;
}

double TimeSpan::totalMinutes() const
{
  long long minutes = m_microseconds / MICROSECONDS_IN_MINUTE;
  double frac = double(m_microseconds % MICROSECONDS_IN_MINUTE) / MICROSECONDS_IN_MINUTE;

  return minutes + frac;
}

double TimeSpan::totalSeconds() const
{
  long long seconds = m_microseconds / MICROSECONDS_IN_SECOND;
  double frac = double(m_microseconds % MICROSECONDS_IN_SECOND) / MICROSECONDS_IN_SECOND;

  return seconds + frac;
}

double TimeSpan::totalMilliseconds() const
{
  long long milSec = m_microseconds / MICROSECONDS_IN_MILLISECONDS;
  double frac = double(m_microseconds % MICROSECONDS_IN_MILLISECONDS) / MICROSECONDS_IN_MILLISECONDS;
  return milSec + frac;
}

TimeSpan TimeSpan::fromDays(double val)
{
  long long days = (long long)val;
  double frac = (val - days) * MICROSECONDS_IN_DAY;
  return TimeSpan(days * MICROSECONDS_IN_DAY + (long long)round(frac));
}

TimeSpan TimeSpan::fromHours(double val)
{
  long long hours = (long long)val;
  double frac = (val - hours) * MICROSECONDS_IN_HOUR;
  return TimeSpan(hours * MICROSECONDS_IN_HOUR + (long long)round(frac));
}

TimeSpan TimeSpan::fromMinutes(double val)
{
  long long mins = (long long)val;
  double frac = (val - mins) * MICROSECONDS_IN_MINUTE;
  return TimeSpan(mins * MICROSECONDS_IN_MINUTE + (long long)round(frac));
}

TimeSpan TimeSpan::fromSeconds(double val)
{
  long long secs = (long long)val;
  double frac = (val - secs) * MICROSECONDS_IN_SECOND;
  return TimeSpan(secs * MICROSECONDS_IN_SECOND + (long long)round(frac));
}

TimeSpan TimeSpan::fromMilliseconds(double val)
{
  long long milSec = (long long)val;
  double frac = (val - milSec) * MICROSECONDS_IN_MILLISECONDS;
  return TimeSpan(milSec * MICROSECONDS_IN_MILLISECONDS + (long long)round(frac));
}

double TimeSpan::totalTime(TimeIntervalTypes to)
{
  switch (to)
  {
    case tiDays: return totalDays();
    case tiHours: return totalHours();
    case tiMinutes: return totalMinutes();
    case tiSeconds: return totalSeconds();
    default: return 0;
  }
}
QString TimeSpan::toStringImpl(int prec, bool shortcut, int maxWordCount) const
{
  if (m_microseconds == 0)
    return _tr("-");

  if ((prec < 0) || (prec > MAX_SECOND_FRAC_DIGITS))
  {
    prec = MAX_SECOND_FRAC_DIGITS;
  }

  TimeSpan newInt(duration());
  double ms = qAbs(double(newInt.microseconds()) / MICROSECONDS_IN_SECOND);
  double rez = QString::number(ms, 'f', prec).toDouble();
  if (rez >= 1.0)
  {
    newInt += TimeSpan(TimeSpan::MICROSECONDS_IN_SECOND);
    ms = 0;
  }
  int d = newInt.days();
  int h = newInt.hours();
  int m = newInt.minutes();
  int s = newInt.seconds();

  QString msStr;
  if (prec > 0)
    msStr = QString::number(ms,'f', prec).right(prec + 1);

  QString res;
  int wordCount = 0;
  if (shortcut)
  {
    if ((d > 0) && (wordCount < maxWordCount))
    {
      res = _tr("%1 сут. ").arg(d);
      wordCount++;
    }
    if ((h > 0) && (wordCount < maxWordCount))
    {
      res = res + _tr("%1 ч ").arg(h);
      wordCount++;
    }
    if ((m > 0) && (wordCount < maxWordCount))
    {
      res = res + _tr("%1 мин. ").arg(m);
      wordCount++;
    }
    if ((s > 0) && (wordCount < maxWordCount))
    {
      res = res + _tr("%1%2 c").arg(s).arg(msStr);
      wordCount++;
    }
  }
  else
  {
    if (d > 0)
      res = _tr("%1 сут. ").arg(d);

    res = res + _tr("%1 ч ").arg(h, 2, 10, QChar('0'));
    res = res + _tr("%1 мин. ").arg(m, 2, 10, QChar('0'));
    res = res + _tr("%1%2 c").arg(s, 2, 10, QChar('0')).arg(msStr);
  }

  if (m_microseconds < 0)
    res.prepend('-');

  return res;
}

QString TimeSpan::toString(int prec, bool shortcut)
{
    return toStringImpl(prec, shortcut, 4);
}

QString TimeSpan::toString(int prec, bool shortcut) const
{
    return toStringImpl(prec, shortcut, 4);
}

QString TimeSpan::toStringLimitationParts(int maxWordCount)
{   
  return toStringImpl(0, true, maxWordCount);
}

bool TimeSpan::operator ==(const TimeSpan &other) const
{
  return (m_microseconds == other.m_microseconds);
}

bool TimeSpan::operator <(const TimeSpan &other) const
{
  return (m_microseconds < other.m_microseconds);
}

TimeSpan &TimeSpan::operator +=(const TimeSpan &other)
{
  this->m_microseconds += other.m_microseconds;

  return *this;
}

TimeSpan &TimeSpan::operator -=(const TimeSpan &other)
{
  this->m_microseconds -= other.m_microseconds;
  return *this;
}

void TimeSpan::showTimeSpanProperties()
{
  qDebug() << this->toString();
  qDebug() << QString("Days: %1 TotalDays: %2").arg(this->days(), 20, 10).arg(this->totalDays(), 20,'g', 20);
  qDebug() << QString("Hours: %1 TotalHours: %2").arg(this->hours(), 20, 10).arg(this->totalHours(), 20,'g', 20);
  qDebug() << QString("Minutes: %1 TotalMinutes: %2").arg(this->minutes(), 20, 10).arg(this->totalMinutes(), 20,'g', 20);
  qDebug() << QString("Seconds: %1 TotalSeconds: %2").arg(this->seconds(), 20, 10).arg(this->totalSeconds(), 20,'g', 20);
  qDebug() << QString("Millisec: %1 TotalMillisec: %2").arg(this->milliseconds(), 20, 10).arg(this->totalMilliseconds(), 20,'g', 20);
  qDebug() << QString("Microsec: %1").arg(this->totalMicroseconds());
}


long long TimeSpan::toMicroseconds(double time, TimeIntervalTypes from)
{
  switch (from)
  {
    case tiDays:
      return (long long)round(time * MICROSECONDS_IN_DAY);

  case tiHours:
      return (long long)round(time * MICROSECONDS_IN_HOUR);

  case tiMinutes:
      return (long long)round(time * MICROSECONDS_IN_MINUTE);

  case tiSeconds:
      return (long long)round(time * MICROSECONDS_IN_SECOND);

  default:
    return 0;
  }
}


TimeSpan operator -(const TimeSpan &left, const TimeSpan &right)
{
  return TimeSpan(left.totalMicroseconds() - right.totalMicroseconds());
}

TimeSpan operator +(const TimeSpan &left, const TimeSpan &right)
{
  return TimeSpan(left.totalMicroseconds() + right.totalMicroseconds());
}

QDebug operator<<(QDebug dbg, const TimeSpan &ts)
{
  dbg.nospace() << ts.toString();

  return dbg.space();
}

