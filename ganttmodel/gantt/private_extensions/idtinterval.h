/*!
 * \file
 * \~russian
 * \brief Файл содержит интерфейс IDtInterval class.
 * \~englist
 * \brief File contains interface IDtInterval class.
 */
#ifndef IDTINTERVAL_H
#define IDTINTERVAL_H

#include "utcdatetime.h"

/*!
 * \~russian
 * \brief Класс IDtInterval class задает набор функций для объекта, выступающего в роли UtcDataTime class интервала
 * \~english
 * \brief The IDtInterval class sets interface for UtcDataTime class interval
 */
class IDtInterval
{
public:
    virtual ~IDtInterval(){};

    virtual UtcDateTime left() const = 0;
    virtual UtcDateTime right() const = 0;
    virtual UtcDateTime min() const = 0;
    virtual UtcDateTime max() const = 0;

    virtual void setLeft(const UtcDateTime &dt) = 0;
    virtual void setRight(const UtcDateTime &dt) = 0;
    virtual void setMin(const UtcDateTime &dt) = 0;
    virtual void setTimeSpan(const TimeSpan &ts) = 0;
};

#endif // IDTINTERVAL_H
