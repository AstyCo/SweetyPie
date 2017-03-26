/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление Interval class.
 * \~englist
 * \brief File contains definition of Interval class.
 */
#ifndef INTERVAL_H
#define INTERVAL_H

/*! \brief Класс Interval class  - Шаблонный класс для представления интервала
 *
 *  Point - тип для представления точки на интервале;
 *  Length - тип для представления длины интервала.
 *  Point - Point = Length.
 */
template<typename Point, typename Length>
class Interval
{
public:
  Interval() {}

  Interval(const Point &begin, const Point &end)
  {
    m_begin = begin;
    m_end = end;    
  }

  Interval(const Interval<Point, Length> &other)
  {
    m_begin = other.m_begin;
    m_end = other.m_end;
  }

  virtual ~Interval() { }

  bool isEmpty() const { return ! (m_begin <= m_end); }

  bool intersect(const Interval<Point, Length> &other) const
  {
    if (isEmpty() || other.isEmpty())
      return false;

    if (m_begin < other.m_begin)
      return ! (m_end < other.m_begin);
    else
      return ! (other.m_end < m_begin);
  }

  Interval<Point, Length> intersectionInterval(const Interval<Point, Length> &other) const
  {
    if (intersect(other))
    {
      Interval<Point, Length> result;
      if (m_begin < other.m_begin)
        result.setBegin(other.begin());
      else
        result.setBegin(m_begin);

      if (m_end < other.m_end)
        result.setEnd(m_end);
      else
        result.setEnd(other.m_end);

      return result;
    }
    else
      return Interval<Point, Length>();
  }

  bool contains(const Point &point) const
  {
    if (isEmpty())
      return false;

    return ((m_begin <= point) && (point <= m_end));
  }

  bool contains(const Interval<Point, Length> &other) const
  {
    if (isEmpty() || other.isEmpty())
      return false;

    return ((m_begin <= other.begin()) && (other.end() <= m_end));
  }

  Length distanceTo(const Point &point) const
  {
    if (isEmpty() || contains(point))
      return Length();
    else
    {
      if (point < m_begin)
        return (point - m_begin);
      else
        return (point - m_end);
    }
  }

  bool operator ==(const Interval<Point, Length> &other) const
  {
    if (isEmpty())
      return false;

    return ((this->m_begin == other.m_begin)
            && (this->m_end == other.m_end));
  }

  inline bool operator !=(const Interval<Point, Length> &other) const
  {
    return (! (*this == other));
  }

  virtual Length length() const { return (m_end - m_begin); }

  void move(const Length &by)
  {
    if (! isEmpty())
    {
      m_begin += by;
      m_end += by;
    }
  }

  Interval<Point, Length> &operator+=(const Interval<Point, Length> &other)
  {
    if (! isEmpty() && ! other.isEmpty())
    {
      if (other.m_begin < m_begin)
        m_begin = other.m_begin;

      if (m_end < other.m_end)
        m_end = other.m_end;
    }
    return *this;
  }

  Interval<Point, Length> &operator+=(const Point &other)
  {
    if (! isEmpty())
    {
      if (other < m_begin)
        m_begin = other;
      else if (m_end < other)
        m_end = other;
    }
    else
    {
      m_begin = other;
      m_end = other;
    }

    return *this;
  }

  Interval<Point, Length> operator+(const Interval<Point, Length> &other) const
  {
    Interval<Point, Length> temp(*this);
    temp += other;
    return temp;
  }

  Interval<Point, Length> operator+(const Point &other) const
  {
    Interval<Point, Length> temp(*this);
    temp += other;
    return temp;
  }

  Point begin() const
  {
    return m_begin;
  }

  void setBegin(const Point &begin)
  {
    m_begin = begin;
  }

  Point end() const
  {
    return m_end;
  }

  void setEnd(const Point &end)
  {
    m_end = end;
  }

  void assign(const Point &begin, const Point &end)
  {
    m_begin = begin;
    m_end = end;
  }

private:
  Point m_begin;   ///< Точка начала интервала
  Point m_end;     ///< Точка окончания интервала
};

#endif // INTERVAL_H
