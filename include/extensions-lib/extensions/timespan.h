/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление TimeSpan class.
 * \~englist
 * \brief File contains definition of TimeSpan class.
 */
#ifndef TIMESPAN_H
#define TIMESPAN_H

#include <QString>
#include <QDebug>

#include "extensions_global.h"

/*!
 * \~russian
 * \brief Класс TimeSpan class представляет отрезок времени с точностью до микросекунд
 * \~english
 * \brief The TimeSpan class class represents microsecond-precision timespan
 */
class EXTENSIONS_LIBRARY_EXPORT TimeSpan
{
public:
  static const long long MICROSECONDS_IN_MILLISECONDS;
  /// Число микросекунд в одной секунде
  static const long long MICROSECONDS_IN_SECOND;
  static const long long MICROSECONDS_IN_MINUTE;
  static const long long MICROSECONDS_IN_HOUR;
  static const long long MICROSECONDS_IN_DAY;
  /// Максимальное число знаков после запятой при выводе долей секунды
  static const int MAX_SECOND_FRAC_DIGITS;

  /// Виды представления времени
  enum TimeIntervalTypes{
                     tiDays,    ///< Дни
                     tiHours,   ///< Часы
                     tiMinutes, ///< Минуты
                     tiSeconds  ///< Секунды
                   };
  TimeSpan();
  TimeSpan(const TimeSpan &other);
  explicit TimeSpan(long long microseconds);
  TimeSpan(int hours, int minutes, int seconds);
  TimeSpan(int days, int hours, int minutes, int seconds);
  TimeSpan(int days, int hours, int minutes, int seconds, int microseconds);
  TimeSpan(double val, TimeIntervalTypes intType);

  int days() const { return int(m_microseconds / MICROSECONDS_IN_DAY); }
  int hours() const { return int(m_microseconds % MICROSECONDS_IN_DAY / MICROSECONDS_IN_HOUR); }
  int minutes() const { return int(m_microseconds % MICROSECONDS_IN_HOUR / MICROSECONDS_IN_MINUTE); }
  int seconds() const { return int(m_microseconds % MICROSECONDS_IN_MINUTE / MICROSECONDS_IN_SECOND); }
  int milliseconds() const { return int(m_microseconds % MICROSECONDS_IN_SECOND / 1000); }
  int microseconds() const { return int(m_microseconds % MICROSECONDS_IN_SECOND); }

  double totalDays() const;
  double totalHours() const;
  double totalMinutes() const;
  double totalSeconds() const;
  double totalMilliseconds() const;
  long long totalMicroseconds() const { return m_microseconds; }

  TimeSpan duration() const { return TimeSpan(qAbs(m_microseconds)); }
  TimeSpan negate() const { return TimeSpan( - m_microseconds); }

  static TimeSpan fromDays(double val);
  static TimeSpan fromHours(double val);
  static TimeSpan fromMinutes(double val);
  static TimeSpan fromSeconds(double val);
  static TimeSpan fromMilliseconds(double val);
  static TimeSpan fromMicroseconds(long long val) { return TimeSpan(val); }

  /** Функция для конвертирования представления времени из одного типа в другой
  @return Время в новом представлении */
  double totalTime(TimeIntervalTypes to    ///< тип представления времени
                                    );
  /** Статическая функция преобразует указанное количество секунд в строку в виде %часов %минут %секунд
  * \return Строка времени в часах минутах и секундах */
  QString toString(int prec = MAX_SECOND_FRAC_DIGITS, ///< Кол-во выводимых символов долей секунды
                   bool shortcut = false ///< Скрывать нулевые элементы (0 сут. 1 ч. 0 мин. 15 с => 1 ч. 15 с)
                                );
  /** Статическая функция преобразует указанное количество секунд в строку в виде %часов %минут %секунд
  * \return Строка времени в часах минутах и секундах */
  QString toString(int prec = MAX_SECOND_FRAC_DIGITS, ///< Кол-во выводимых символов долей секунды
                   bool shortcut = false ///< Скрывать нулевые элементы (0 сут. 1 ч. 0 мин. 15 с => 1 ч. 15 с)
                                ) const;


  QString toStringLimitationParts(int maxWordCount = 4);

  /** Функция перевода указанного времени в секунды
  @return Число секунд */
  static long long toMicroseconds(double time,      ///< время для перевода
                       TimeIntervalTypes from ///< тип представления
                       );

  bool operator ==(const TimeSpan &other) const;
  inline bool operator !=(const TimeSpan &other) const { return !(*this == other); }
  bool operator <(const TimeSpan &other) const;
  inline bool operator >(const TimeSpan &other) const { return (other < *this); }
  inline bool operator <=(const TimeSpan &other) const { return !(other < *this); }
  inline bool operator >=(const TimeSpan &other) const { return !(*this < other); }
  TimeSpan & operator +=(const TimeSpan &other);
  TimeSpan & operator -=(const TimeSpan &other);

  void showTimeSpanProperties();

private:
  long long m_microseconds;
  QString toStringImpl(int prec, bool shortcut, int maxWordCount) const;
};

TimeSpan EXTENSIONS_LIBRARY_EXPORT operator -(const TimeSpan &left, const TimeSpan &right);
TimeSpan EXTENSIONS_LIBRARY_EXPORT operator +(const TimeSpan &left, const TimeSpan &right);

// на будущее, когда toString будет const
QDebug EXTENSIONS_LIBRARY_EXPORT operator<<(QDebug dbg, const TimeSpan &ts);

#endif // TIMESPAN_H
