/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление QRectFUnion class.
 * \~englist
 * \brief File contains definition of QRectFUnion class.
 */
#ifndef QRECTUNION_H
#define QRECTUNION_H

#include "ganttproject_global.h"
#include <QPainter>
#include <QList>
#include <QRect>
#include <QDebug>

/*!
 * \~russian
 * \brief Класс GanttCurrentDtSlider class представляет объединение QRectF class
 * \~english
 * \brief The IntervalUnion class class represents union of QRectF class
 */
class GANTTMODELSHARED_EXPORT QRectFUnion
{
    friend QRectFUnion operator*(const QRectFUnion &f, qreal coef);
public:
    QRectFUnion();
    QRectFUnion(const QList<QRectF> &rects);

    void fillRects(QPainter *painter, const QBrush &brush);
    void join(const QRectF &newRect);
    inline bool isEmpty() const;
    inline int size() const;
    inline void clear();
    inline const QList<QRectF> &rects() const;
    QRectFUnion &operator|=(const QRectF& rect);
    QRectFUnion &operator*=(qreal coef);
private:
    void join(int index, int i = 0);

    QList<QRectF> _rects;
};

// INLINE FUNCTIONS
bool QRectFUnion::isEmpty() const { return _rects.isEmpty();}

int QRectFUnion::size() const { return _rects.size();}

void QRectFUnion::clear() { _rects.clear();}

const QList<QRectF> &QRectFUnion::rects() const { return _rects;}


GANTTMODELSHARED_EXPORT QDebug operator<<(QDebug, const QRectFUnion&);
GANTTMODELSHARED_EXPORT QRectFUnion operator*(const QRectFUnion &f, qreal coef);
GANTTMODELSHARED_EXPORT QRectFUnion operator*(qreal coef, const QRectFUnion &f);

#endif // QRECTUNION_H
