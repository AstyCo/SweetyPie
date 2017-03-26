/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию UtcDateTime class.
 * \~englist
 * \brief File contains realisation of UtcDateTime class.
 */
#include "utcdatetime.h"

#include <cmath>

const QString dtFormat = "dd.MM.yyyy HH:mm:ss";
const QString dtParseFormat = "yyyy MM dd HH mm ss";

const int UtcDateTime::MAX_SECOND_FRAC_DIGITS = 6;

UtcDateTime::UtcDateTime()
{
  m_dateTime.setTimeSpec(Qt::UTC);
  m_microsecond = 0;
}

UtcDateTime::UtcDateTime(const UtcDateTime &dtm)
{
  m_dateTime = dtm.m_dateTime;
  m_microsecond = dtm.m_microsecond;
}

UtcDateTime::UtcDateTime(const QDate &date)
{
  m_dateTime.setTimeSpec(Qt::UTC);
  m_dateTime.setDate(date);
  m_dateTime.setTime(QTime(0,0));
  m_microsecond = 0;
}

UtcDateTime::UtcDateTime(const QDate &date, const QTime &time, int secPartMicroseconds)
{
  m_dateTime.setDate(date);
  QTime newTime(time.hour(), time.minute(), time.second());
  m_dateTime.setTime(newTime);

  m_dateTime.setTimeSpec(Qt::UTC);
  this->setSecPartMicroseconds(secPartMicroseconds);
}

UtcDateTime::UtcDateTime(const QDateTime dt, int secPartMicroseconds)
{
  this->setDateTime(dt);
  this->setSecPartMicroseconds(secPartMicroseconds);
}

UtcDateTime::UtcDateTime(int year, int month, int day, int hour, int min, double secondsWithFraction)
{
  int second = (int)secondsWithFraction;
  double micSec = (secondsWithFraction - second) * TimeSpan::MICROSECONDS_IN_SECOND;
  int microsecond = int(round(micSec)); // округляем до ближайшей микросекунды

  normalizeDtVals(year, month, day, hour, min, second, microsecond);
  this->setDateTime(QDateTime(QDate(year, month, day), QTime(hour, min, second), Qt::UTC));
  m_microsecond = microsecond;
}

UtcDateTime::UtcDateTime(int year, int month, int day, int hour, int min, int second, int microsecond)
{
  normalizeDtVals(year, month, day, hour, min, second, microsecond);
  this->setDateTime(QDateTime(QDate(year, month, day), QTime(hour, min, second), Qt::UTC));
  m_microsecond = microsecond;
}

QDate UtcDateTime::date() const
{
  return m_dateTime.date();
}

QTime UtcDateTime::time() const
{
  return m_dateTime.time();
}

QDateTime UtcDateTime::dateTime() const
{
  return m_dateTime;
}

long long UtcDateTime::toMicrosecondsSinceEpoch() const
{
  return (m_dateTime.toTime_t() * TimeSpan::MICROSECONDS_IN_SECOND + m_microsecond);
}

uint UtcDateTime::toBshvTime() const
{
  return m_dateTime.toTime_t();
}

uint UtcDateTime::toBshvTime2000() const
{
    QDateTime dt;
    dt.setTimeSpec(Qt::UTC);
    dt.setDate(QDate(2000,1,1));
    dt.setTime(QTime(0,0,0));
    return m_dateTime.toTime_t()-dt.toUTC().toTime_t();
}


void UtcDateTime::setDateTime(const QDateTime dt)
{
  m_dateTime.setDate(dt.date());
  // в компоненте времени нужно обнулить миллисекунды
  QTime time = dt.time();
  m_dateTime.setTime(QTime(time.hour(), time.minute(), time.second()));
  m_dateTime.setTimeSpec(Qt::UTC);
  m_microsecond = 0;
}

void UtcDateTime::setMicrosecondsSinceEpoch(long long microsecondsSinceEpoch)
{
  *this = UtcDateTime::fromMicrosecondsSinceEpoch(microsecondsSinceEpoch);
}

void UtcDateTime::setBshvTime(uint seconds)
{
  m_dateTime = QDateTime::fromTime_t(seconds);
  m_dateTime = m_dateTime.toUTC();
  m_microsecond = 0;
}

void UtcDateTime::setBshvTime2000(uint seconds)
{
    QDateTime dt;
    dt.setTimeSpec(Qt::UTC);
    dt.setDate(QDate(2000,1,1));
    dt.setTime(QTime(0,0,0));
    m_dateTime = QDateTime::fromTime_t(dt.toUTC().toTime_t()+seconds);
    m_dateTime = m_dateTime.toUTC();
    m_microsecond = 0;
}


void UtcDateTime::setSecPartMicroseconds(int secPartMicroSec)
{
  if ((secPartMicroSec < 0) || (secPartMicroSec >= TimeSpan::MICROSECONDS_IN_SECOND))
  {
    m_microsecond = 0;
  }
  else
    m_microsecond = secPartMicroSec;
}

// Округление с точностью
double roundTo (double x, int precision)
{
      QString msStr = QString::number(x, 'f', precision);
      double rez = msStr.toDouble();
      return rez;
}

QString UtcDateTime::toStdString(int prec) const
{
  // при выводе долей секунды необходимо правильно округлять выводимое значение
  // в ситуациях когда например:
  // 12 ч 12 м 12 с и 0.9999990 долей секунды
  // при выводе с точностью 6 будет: 12.12.12.999999
  // а с точностью 5 уже: 12.12.13.00000

  if (!m_dateTime.isValid())
    return QString();

  if (prec < 0)
    prec = 0;
  else if (prec > MAX_SECOND_FRAC_DIGITS)
    prec = MAX_SECOND_FRAC_DIGITS;

  QDateTime tmpDT = m_dateTime;
  double secFrac = double(m_microsecond) / TimeSpan::MICROSECONDS_IN_SECOND;
  if(roundTo(secFrac,prec) >= 1.0)
      tmpDT = tmpDT.addSecs(1);

  QString ret = tmpDT.toString(dtFormat);
  QString msStr = QString::number(secFrac, 'f', prec);
  return ret + msStr.remove(0,1);
}

QString UtcDateTime::toString(QString format) const
{
  // добавить возможность выводить дату время в формате, поддерживаемом QDateTime
  // с произвольным количеством цифр долей секунды
  // "dd.MM.yyyy HH:mm:ss.zzz zzz" => 12.12.2014 12:12:12.123 456
  // не за быть про правильное округление
    if (!m_dateTime.isValid())
      return QString();

    if(format.isEmpty())
        return toStdString();

    int prec = format.count("z");
    QDateTime tmpDT = m_dateTime;
    double secFrac = double(m_microsecond) / TimeSpan::MICROSECONDS_IN_SECOND;
    if(roundTo(secFrac,prec) >= 1.0)
      tmpDT = tmpDT.addSecs(1);

    QString tmpFormat = format;
    tmpFormat.remove("z");
    QString rezStr = tmpDT.toString(tmpFormat);
    QString msStr = QString::number(secFrac, 'f', prec);

    int j = 0;
    for(int i=0; i<format.length(); i++)
    {
        if(format[i]==QChar('z'))
        {
            rezStr.insert(i,msStr[j+2]); // +2 потому что "0."
            j++;
        }
    }
    return rezStr;
}

QString UtcDateTime::toParsingString(int prec) const
{
  if (!m_dateTime.isValid())
    return QString();

  if (prec < 0)
    prec = 0;
  else if (prec > MAX_SECOND_FRAC_DIGITS)
    prec = MAX_SECOND_FRAC_DIGITS;

  QDateTime tmpDT = m_dateTime;
  double secFrac = double(m_microsecond) / TimeSpan::MICROSECONDS_IN_SECOND;
  if(roundTo(secFrac,prec) >= 1.0)
    tmpDT = tmpDT.addSecs(1);

  QString ret = tmpDT.toString(dtParseFormat);
  QString msStr = QString::number(secFrac, 'f', prec);

  return ret +msStr.remove(0,1);
}

void UtcDateTime::fromLocalDateTime(const QDateTime localDt, int secPartMicroSecs)
{
  this->setDateTime(localDt.toUTC());
  this->setSecPartMicroseconds(secPartMicroSecs);
}

UtcDateTime UtcDateTime::fromParsingString(QString str)
{
  UtcDateTime dtm;
  int count = str.count('.');

  if (count == 0)
    return UtcDateTime(QDateTime::fromString(str, dtParseFormat));
  else
  {
    int idx = str.lastIndexOf('.');
    QString tmpStr = str.left(idx);
    dtm.setDateTime(QDateTime::fromString(tmpStr, dtParseFormat));
    tmpStr = str.right(str.size() - idx).prepend('0');
    double micro = tmpStr.toDouble() * TimeSpan::MICROSECONDS_IN_SECOND;
    dtm.setSecPartMicroseconds(int(micro));

    return dtm;
  }
}

bool UtcDateTime::isValid() const
{
  return m_dateTime.isValid();
}

bool UtcDateTime::isNull() const
{
  return m_dateTime.isNull();
}

int UtcDateTime::daysTo(const UtcDateTime &udt) const
{
  return m_dateTime.daysTo(udt.dateTime());
}

int UtcDateTime::secsTo(const UtcDateTime &udt) const
{
  return m_dateTime.secsTo(udt.dateTime());
}

double UtcDateTime::secsPartMicrosecondsTo(const UtcDateTime &udt) const
{
  return microsecondsTo(udt)/(double)(TimeSpan::MICROSECONDS_IN_SECOND);
}


long long UtcDateTime::microsecondsTo(const UtcDateTime &udt) const
{
  int secTo = m_dateTime.secsTo(udt.m_dateTime);
  int msTo = udt.m_microsecond - m_microsecond;
  if (msTo < 0)
  {
    secTo--;
    msTo += TimeSpan::MICROSECONDS_IN_SECOND;
  }

  return secTo * TimeSpan::MICROSECONDS_IN_SECOND + msTo;
}

UtcDateTime UtcDateTime::addMicroseconds(long long microseconds) const
{
  int addSec = microseconds / TimeSpan::MICROSECONDS_IN_SECOND;
  int addMicroSec = microseconds % TimeSpan::MICROSECONDS_IN_SECOND;
  int finalMicroSec = m_microsecond + addMicroSec;


  if (finalMicroSec >= TimeSpan::MICROSECONDS_IN_SECOND)
  {
    addSec++;
    finalMicroSec -= TimeSpan::MICROSECONDS_IN_SECOND;
  }
  else if (finalMicroSec < 0)
  {
    addSec--;
    finalMicroSec += TimeSpan::MICROSECONDS_IN_SECOND;
  }

  return UtcDateTime(m_dateTime.addSecs(addSec), finalMicroSec);
}

UtcDateTime UtcDateTime::addSecs(int secs) const
{
  //добавить/вычесть указанное количество секунд
  UtcDateTime dt(*this);
  dt.m_dateTime = m_dateTime.addSecs(secs);

  return dt;
}

UtcDateTime UtcDateTime::addDays(int days) const
{
  //добавить/вычесть указанное количество дней
  UtcDateTime dt(*this);
  dt.m_dateTime = m_dateTime.addDays(days);

  return dt;
}

UtcDateTime UtcDateTime::addYears(int years) const
{
  //добавить/вычесть указанное количество лет
  UtcDateTime dt(*this);
  dt.m_dateTime = m_dateTime.addYears(years);

  return dt;
}

UtcDateTime UtcDateTime::addMonths(int months) const
{
  //добавить/вычесть указанное количество месяцев
  UtcDateTime dt(*this);
  dt.m_dateTime = m_dateTime.addMonths(months);

  return dt;
}

UtcDateTime UtcDateTime::fromStdString(QString str)
{
  UtcDateTime dtm;
  int count = str.count('.');
  if (count < 2)
    return dtm;

  if (count == 2)
    return UtcDateTime(QDateTime::fromString(str, dtFormat));
  else
  {
    int idx = str.lastIndexOf('.');
     QString tmpStr = str.left(idx);
    dtm.setDateTime(QDateTime::fromString(tmpStr, dtFormat));
    tmpStr = str.right(str.size() - idx).prepend('0');
    double micro = tmpStr.toDouble() * TimeSpan::MICROSECONDS_IN_SECOND;
    dtm.setSecPartMicroseconds(int(micro));

    return dtm;
  }
}

UtcDateTime UtcDateTime::fromString(QString str, QString format)
{
//добавить возможность парсить дату время в формате, поддерживаемом QDateTime
// с произвольным количеством цифр долей секунды
// "dd.MM.yyyy HH:mm:ss.zzz zzz", 12.12.2014 12:12:12.123 456
  // m_sec = 0.123456
  str = str.trimmed();
  format = format.trimmed();

  UtcDateTime dt;  
  QString tmpFormat = format;
  tmpFormat.remove("z");

  QString tmpStr = str;
  double m = 0.1;
  double tmpFrac = 0;
  int j = 0;
  for(int i = 0; i < format.length(); i++)
  {
    if(format[i] == QChar('z'))
    {
      tmpFrac += m * QString(tmpStr[i - j]).toInt();
      m = m * 0.1;
      tmpStr.remove(i - j, 1);
      j++;
    }
  }
  dt.setDateTime(QDateTime::fromString(tmpStr,tmpFormat));
  dt.m_microsecond = tmpFrac * TimeSpan::MICROSECONDS_IN_SECOND;

  return dt;
}

UtcDateTime UtcDateTime::fromMicrosecondsSinceEpoch(long long microseconds)
{
  UtcDateTime unixDt;
  if (microseconds >= 0)
  {
    unixDt.setBshvTime(microseconds / TimeSpan::MICROSECONDS_IN_SECOND);
    unixDt.m_microsecond = microseconds % TimeSpan::MICROSECONDS_IN_SECOND;
  }
  return unixDt;
}

bool UtcDateTime::operator==(const UtcDateTime &other) const
{
  return (m_dateTime == other.m_dateTime) && (m_microsecond == other.m_microsecond);
}

bool UtcDateTime::operator<(const UtcDateTime &other) const
{
  if (m_dateTime == other.dateTime())
  {
    if (m_microsecond < other.m_microsecond)
      return true;
    else
      return false;
  }
  else
    return m_dateTime < other.dateTime();
}

UtcDateTime &UtcDateTime::operator +=(const TimeSpan &t)
{
  UtcDateTime newDt(this->addMicroseconds(t.totalMicroseconds()));
  *this = newDt;
  return *this;
}

UtcDateTime &UtcDateTime::operator -=(const TimeSpan &t)
{
  UtcDateTime newDt(this->addMicroseconds(- t.totalMicroseconds()));
  *this = newDt;
  return *this;
}

void UtcDateTime::toJulianDaysWithDaySecThousands(double &jdays, double &jdaySecThs) const
{
  jdays = getJulianDay();

  QTime time = m_dateTime.time();
  int seconds = time.second() + time.minute() * 60 + time.hour() * 3600;
  jdaySecThs = (seconds + double(m_microsecond) / TimeSpan::MICROSECONDS_IN_SECOND) * 1E-3;
}

void UtcDateTime::toJulianDaysWithDayPart(double &jdays, double &jdayPart) const
{
  toJulianDaysWithDaySecThousands(jdays, jdayPart);
  jdayPart /= 86.4; // тыс. сек от начала дня в часть дня
}


double UtcDateTime::getJulianDay() const
{
  QDate date = m_dateTime.date();
  double JD = date.day() - 32075 + 1461 * (date.year() + 4800 + (date.month() - 14) / 12) / 4
           + 367 * (date.month() - 2 - ((date.month() - 14) / 12) * 12) / 12
           - 3 * ((date.year() + 4900 + (date.month() - 14) / 12) / 100) / 4;
  return (JD - 0.5); // от 00 часов вместо полудня
}

QDate UtcDateTime::getCalendarJday(double jdays)
{
  int l,n,i,j;
  int day, mon, year;

  l = (int)floor(jdays + 0.5) + 68569;
  n = (4 * l) / 146097;
  l = l - (146097 * n + 3) / 4;
  i = (4000 * (l + 1)) / 1461001;
  l = l - (1461 * i) / 4 + 31;
  j = (80 * l) / 2447;
  day = l - (2447 * j) / 80;
  l = j / 11;
  mon = j + 2 - (12 * l);
  year = 100 * (n - 49) + i + l;

  return QDate(year, mon, day);
}

void UtcDateTime::normalizeDtVals(int &year, int &month, int &day, int &hour, int &min, int &second, int &microsecond)
{
  int m[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};	// месяцы начинаются с 1

  // предварительная проверка секунд, минут, часов, дней, месяцев и лет
  // на принадлежность допустимому диапазону
  if (microsecond >= TimeSpan::MICROSECONDS_IN_SECOND)
  {
    second += microsecond / TimeSpan::MICROSECONDS_IN_SECOND;
    microsecond %= TimeSpan::MICROSECONDS_IN_SECOND;
  }
  if (second >= 60)
  {
    min += second / 60;
    second %= 60;
  }
  if (min >= 60)
  {
    hour += min/60;
    min %= 60;
  }
  if (hour >= 24)
  {
    day  += hour/24;
    hour %= 24;
  }

  // високосный год и февраль
  if (!(year%4) && (year%100 || !(year%400)) && month == 2)
    m[2] = 29;

  while (day > m[month])
  {
    day -= m[month];
    month++;
    if (month > 12)
    {
      year++; month = 1;
      if (!(year%4) && (year%100 || !(year%400)))
        m[2] = 29;
      else
        m[2] = 28;
    }
  }
}

UtcDateTime UtcDateTime::fromJulianDaysWithDaySecThousands(double jdays, double jdaySecThs)
{
  int hr, minute;
  double temp;

  QDate jDate(getCalendarJday(jdays));

  temp = jdaySecThs / 3.6;
  hr = (int)floor(temp);
  //temp=Td/0.06 -*hr*60.0;
  temp = (jdaySecThs - hr * 3.6) * 100 / 6;
  minute = (int)floor(temp);
  double sec = fabs(1000.0 * (jdaySecThs - hr * 3.6 - minute * 0.06));
  UtcDateTime jDt(jDate, QTime(hr, minute, 0), 0);

  return jDt + TimeSpan::fromSeconds(sec);
}

UtcDateTime UtcDateTime::fromJulianDaysWithDayPart(double jdays, double jdayPart)
{
  double jdaySecThs = jdayPart * 86.4;
  return fromJulianDaysWithDaySecThousands(jdays, jdaySecThs);
}


TimeSpan operator -(const UtcDateTime &left, const UtcDateTime &right)
{
  return TimeSpan(right.microsecondsTo(left));
}


UtcDateTime operator -(const UtcDateTime &dt, const TimeSpan &t)
{
  return dt.addMicroseconds( - t.totalMicroseconds());
}


UtcDateTime operator +(const UtcDateTime &dt, const TimeSpan &t)
{
  return dt.addMicroseconds(t.totalMicroseconds());
}


UtcDateTime operator +(const TimeSpan &t, const UtcDateTime &dt)
{
  return dt.addMicroseconds(t.totalMicroseconds());
}

QDebug operator<<(QDebug dbg, const UtcDateTime &dt)
{
  dbg.nospace() << dt.toStdString();

  return dbg.space();
}
