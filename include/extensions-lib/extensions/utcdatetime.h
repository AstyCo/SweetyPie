/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление UtcDateTime class.
 * \~englist
 * \brief File contains definition of UtcDateTime class.
 */
#ifndef UTCDATETIME_H
#define UTCDATETIME_H

#include <QDateTime>
#include <QString>
#include <QDebug>

#include "extensions_global.h"
#include "timespan.h"

/*!
 * \~russian
 * \brief Класс DtLine class представляет время в формате UTC с точностью до микросекунд
 * \~english
 * \brief The DtLine class class represents time in UTC format with microsecond precision
 */
class EXTENSIONS_LIBRARY_EXPORT UtcDateTime
{
public:
  /// Точность вывода долей секунды, знаков
  static const int MAX_SECOND_FRAC_DIGITS;

  /** Создает пустой объект со свойствами isNull() = true && isValid() = false */
  UtcDateTime();

  /** Конструктор копирования */
  UtcDateTime(const UtcDateTime & dtm);

  /** Создает объект с переданной датой и временем 00:00:00 */
  UtcDateTime(const QDate & date);

  /** Создает объект с переданными датой и временем и частью секунды.
  * Миллисекунды из объекта QTime (если есть) игнорируются,
  * вместо них используется параметр secPartMicroseconds
  * он должен быть в пределах от 0 до MICROSECONDS_IN_SECOND - 1 */
  UtcDateTime(const QDate & date, const QTime & time, int secPartMicroseconds = 0);

  /** Создает объект с переданными датой и временем и частью секунды.
  * Миллисекунды из объекта QTime (если есть) игнорируются!
  * вместо них используется параметр secPartMicroseconds
  * он должен быть в пределах от 0 до MICROSECONDS_IN_SECOND - 1 */
  UtcDateTime(const QDateTime dt, int secPartMicroseconds = 0);

  /** Создает объект с переданной датой и временем.
   * Входной параметр secondsWithFraction содержит число секунд вместе с дробной частью секунды.
   * Дробная часть секунды округляется до микросекунд, если требуется.
   * Параметры нормализуются при необходимости (hour = 25 -> day++ и hour = 1 и т.д.)  */
  UtcDateTime(int year, int month, int day, int hour, int min, double secondsWithFraction);
  /** Создает объект с переданной датой и временем с точностью до микросекунды.
   * Параметры нормализуются при необходимости (hour = 25 -> day++ и hour = 1 и т.д.)  */
  UtcDateTime(int year, int month, int day, int hour, int min, int second, int microsecond);

  int year() const { return m_dateTime.date().year(); }
  int month() const { return m_dateTime.date().month(); }
  int day() const { return m_dateTime.date().day(); }
  int hour() const { return m_dateTime.time().hour(); }
  int minute() const { return m_dateTime.time().minute(); }
  int second() const { return m_dateTime.time().second(); }
  /** Возвращает часть секунды в микросекундах */
  int microsecond() const { return m_microsecond; }
  /** Возвращает часть секунды, 123456 микросекунд => 0.123456 */
  double secPartMicroseconds() const { return double(m_microsecond) / 1e6; }
  /** Возвращает часть секунды с микросекундой 12 сек 345678 мс => 12.345678 */
  double secondAndMicrosecond() const { return (double)second() + secPartMicroseconds(); }

  /** Возвращает дату */
  QDate date() const;

  /** Возвращает время без долей секунды */
  QTime time() const;
  /** Возвращает дату и время (без долей секунды) */
  QDateTime dateTime() const;

  /** Объект содержит правильную дату и время */
  bool isValid() const;

  /** Объект пуст */
  bool isNull() const;

  /** Устанавливает переданную дату и время,
   * считая их как UTC (значение QDateTime::TimeSpec игнорируется) */
  void setDateTime(const QDateTime dt);

  /** Создает объект с датой и временем, равными прошедшему числу микросекунд с 1.1.1970 00:00:00 */
  void setMicrosecondsSinceEpoch(long long microsecondsSinceEpoch);

  /** Устанавливает дату и время, равные прошедшему числу секунд с 1.1.1970 00:00:00
  * число микросекунд устанавливается в ноль */
  void setBshvTime(uint seconds);
  /** Устанавливает дату и время, равные прошедшему числу секунд с 1.1.2000 00:00:00
  * число микросекунд устанавливается в ноль */
  void setBshvTime2000(uint seconds);


  /** Устанавливает часть секунды в микросекундах, должно быть в пределах от 0 до MAX_SECOND_PRECISION - 1 */
  void setSecPartMicroseconds(int secPartMicroSec);

  /** Преобразует текущие дату, время и доли секунды в число прошедших микросекунд с 1.1.1970 00:00:00 */
  long long toMicrosecondsSinceEpoch() const;

  /** Возвращает число секунд от  1.1.1970 00:00:00 БЕЗ учета долей секунды */
  uint toBshvTime() const;

  /** Возвращает число секунд от  1.1.2000 00:00:00 БЕЗ учета долей секунды */
  uint toBshvTime2000() const;

  /** Возвращает строку в формате "dd.MM.yyyy HH:mm:ss.zzzzzz" */
  QString toStdString(int prec = MAX_SECOND_FRAC_DIGITS) const;

  /** Возвращает строку в формате, указанном в параметре format.
  * Формат соответствует QDateTime::fromString, только знаков долей секунды
  * может быть от 0 до MAX_SECOND_PRECISION (например: "dd.MM.yyyy HH:mm:ss.zzzzzz") */
  QString toString(QString format = "dd.MM.yyyy HH:mm:ss.zzzzzz") const;

  /** Возвращает строку в формате "yyyy MM dd HH mm ss.zzzzzz" */
  QString toParsingString(int prec = MAX_SECOND_FRAC_DIGITS) const;

  /** Переводит календарную дату и время с долями секунды
   *  в юлианскую дату и часть дня в тысячах секунд от начала дня*/
  void toJulianDaysWithDaySecThousands(double &jdays, double &jdaySecThs) const;

  /** Переводит календарную дату и время с долями секунды
   *  в юлианскую дату и часть дня от начала дня */
  void toJulianDaysWithDayPart(double &jdays, double &jdayPart) const;

  /** Устанавливает дату и время на переданные, предварительно преобразованные в UTC
  * Миллисекунды из объекта QTime (если есть) игнорируются,
  * вместо них используется параметр secPartMicroseconds
  * он должен быть в пределах от 0 до MICROSECONDS_IN_SECOND - 1 */
  void fromLocalDateTime(const QDateTime localDt, int secPartMicroSecs = 0);

  /** Создает объект из переданной строки в формате, полученной
  * вызовом UtcDateTime::toParsingString() */
  static UtcDateTime fromParsingString(QString str);

  /** Создает объект из переданной строки в формате, полученной
  * вызовом UtcDateTime::toStdString() */
  static UtcDateTime fromStdString(QString str);

  /** Создает объект из переданной строки в формате, указанном в параметре format.
  * Формат соответствует QDateTime::fromString, только знаков долей секунды
  * может быть от 0 до MAX_SECOND_PRECISION (например: "dd.MM.yyyy HH:mm:ss.zzzzzz") */
  static UtcDateTime fromString(QString str, QString format);

  /** Создает объект с датой и временем, равными прошедшему числу микросекунд с 1.1.1970 00:00:00 */
  static UtcDateTime fromMicrosecondsSinceEpoch(long long toMicrosecondsSinceEpoch);

  /** Переводит Юлианскую дату и часть дня в тысячах секунд от начала дня
   *  в календарную дату и время с долями секунды */
  static UtcDateTime fromJulianDaysWithDaySecThousands(double jdays, double jdaySecThs);

  /** Переводит Юлианскую дату и часть дня от начала дня
   *  в календарную дату и время с долями секунды */
  static UtcDateTime fromJulianDaysWithDayPart(double jdays, double jdayPart);

  /** Возращает число дней от текущей до переданной даты */
  int daysTo(const UtcDateTime &udt) const;

  /** Возращает число полных секунд от текущих даты и времени до переданных */
  int secsTo(const UtcDateTime &udt) const;

  /** Возращает число дробных секунд от текущих даты и времени до переданных */
  double secsPartMicrosecondsTo(const UtcDateTime &udt) const;

  /** Возращает число секунд с долями от текущих даты и времени до переданных */
  long long microsecondsTo(const UtcDateTime &udt) const;

  /** Добавляет к текущему времени число секунд с долями */
  UtcDateTime addMicroseconds(long long microseconds) const;

  /** Добавляет к текущему времени число секунд */
  UtcDateTime addSecs(int secs) const;

  /** Добавляет к текущей дате число дней */
  UtcDateTime addDays(int days) const;

  /** Добавляет к текущей дате число переданных лет */
  UtcDateTime addYears(int years) const;

  /** Добавляет к текущей дате число месяцев */
  UtcDateTime addMonths(int months) const;

  bool operator==(const UtcDateTime &other) const;
  inline bool operator!=(const UtcDateTime &other) const { return !(*this == other); }
  bool operator<(const UtcDateTime &other) const;
  inline bool operator<=(const UtcDateTime &other) const { return !(other < *this); }
  inline bool operator>(const UtcDateTime &other) const { return other < *this; }
  inline bool operator>=(const UtcDateTime &other) const { return !(*this < other); }

  UtcDateTime& operator +=(const TimeSpan &t);
  UtcDateTime& operator -=(const TimeSpan &t);

private:
  /// дата время в UTC с точностью до секунды
  QDateTime m_dateTime;

  /** часть секунды выраженная в микросекундах. Используется для
   * повышения точности хранения времени стандартного класса QTime.
   * заменяет собой число миллисекунд из компонента QTime,
   * диапазон от 0 до 999999 */
  int m_microsecond;

  /** Возвращает текущую дату как юлианскую дату */
  double getJulianDay() const;

  /** Возвращает календарную дату из юлианской даты */
  static QDate getCalendarJday(double jdays);

  /** Предварительная проверка микросекунд, секунд, минут, часов, дней, месяцев и лет
  * на принадлежность допустимому диапазону */
  static void normalizeDtVals(int &year, int &month, int &day,
                              int &hour, int &min, int &second, int &microsecond);
};

TimeSpan EXTENSIONS_LIBRARY_EXPORT operator -(const UtcDateTime &left, const UtcDateTime &right);
UtcDateTime EXTENSIONS_LIBRARY_EXPORT operator -(const UtcDateTime &dt, const TimeSpan &t);

UtcDateTime EXTENSIONS_LIBRARY_EXPORT operator +(const UtcDateTime &dt, const TimeSpan &t);
UtcDateTime EXTENSIONS_LIBRARY_EXPORT operator +(const TimeSpan &t, const UtcDateTime &dt);

QDebug EXTENSIONS_LIBRARY_EXPORT operator<<(QDebug dbg, const UtcDateTime &dt);

#endif // UTCDATETIME_H
