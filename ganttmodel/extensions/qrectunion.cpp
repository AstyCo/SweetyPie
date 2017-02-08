#include "qrectunion.h"

#include <QTransform>

QRectFUnion::QRectFUnion()
{
}

QRectFUnion::QRectFUnion(const QList<QRectF> &rects)
    : _rects(rects)
{

}

void QRectFUnion::fillRects(QPainter *painter, const QBrush &brush)
{
    foreach(const QRectF &rect, _rects)
        painter->fillRect(rect, brush);
}

void QRectFUnion::join(const QRectF &newRect)
{
    if(newRect.isEmpty())
        return;
    for(int i = 0; i < _rects.size(); ++i){
        if(_rects[i].intersects(newRect)){
            _rects[i] |= newRect;
            join(i,i+1);
            return;
        }
    }
    // NO INTERSECTION, ADD NEW
    _rects.append(newRect);
}





QRectFUnion &QRectFUnion::operator|=(const QRectF &rect)
{
    join(rect);
    return *this;
}

QRectFUnion &QRectFUnion::operator*=(qreal coef)
{
    QRectFUnion res = *this * coef;
    return *this = res;
}

void QRectFUnion::join(int index, int i)
{
    if(index >= i){
        qCritical("critical:: QRectFUnion::join index >= i");
        return;
    }
    for(; i < _rects.size(); ++i){
        if(_rects[i].intersects(_rects[index])){
            _rects[index] |= _rects[i];
            _rects.removeAt(i);
            --i;
        }
    }
}

QDebug operator<<(QDebug d, const QRectFUnion &u)
{
    d << QString("QRectFUnion( sz=%1,").arg(QString::number(u.size()));
    foreach(const QRectF &rect, u.rects())
       d << rect;
    d << ')';
    return d;
}

QRectFUnion operator*(const QRectFUnion &f, qreal coef)
{
    QRectFUnion res;

    QTransform matrix;
    matrix.scale(coef, 1);

    foreach(const QRectF &rect, f.rects())
        res._rects.append(matrix.mapRect(rect));
    return res;
}

QRectFUnion operator*(qreal coef, const QRectFUnion &f)
{
    return f * coef;
}
