#ifndef QRECTUNION_H
#define QRECTUNION_H

#include "gantt-static-lib-global.h"
#include <QPainter>
#include <QList>
#include <QRect>
#include <QDebug>

class GANTTSTATICLIB_EXPORT QRectFUnion
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


GANTTSTATICLIB_EXPORT QDebug operator<<(QDebug, const QRectFUnion&);
GANTTSTATICLIB_EXPORT QRectFUnion operator*(const QRectFUnion &f, qreal coef);
GANTTSTATICLIB_EXPORT QRectFUnion operator*(qreal coef, const QRectFUnion &f);

#endif // QRECTUNION_H
