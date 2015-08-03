#include "plotinterval.h"
#include "qwt/qwt_painter.h"
#include "QRect"


qreal PlotInterval::beginX() const
{
    return _beginX;
}

void PlotInterval::setBeginX(qreal beginX)
{
    _beginX = beginX;
}

qreal PlotInterval::endX() const
{
    return _endX;
}

void PlotInterval::setEndX(qreal endX)
{
    _endX = endX;
}

QColor PlotInterval::background() const
{
    return _background1;
}

void PlotInterval::setBackground(const QColor &backgroubd)
{
    _background1 = backgroubd;
}

void PlotInterval::setGradient(const QColor &c1, const QColor &c2)
{
    _background1=c1;
    _background2=c2;
}
PlotInterval::PlotInterval()
{
    _beginX=0;
    _endX=0;
    _background1=Qt::white;
    _background2=QColor();
}

void PlotInterval::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect) const
{
    if(!_background2.isValid())
    {
        painter->setBrush(_background1);
    }
    else
    {
        QLinearGradient gradient( 0, 0, 1, 0 );
        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);

        gradient.setColorAt( 0.0, _background1 );
        gradient.setColorAt( 0.5, _background2 );
        gradient.setColorAt( 1.0, _background1 );

        painter->setBrush(gradient);
    }

    double valueX = xMap.transform( _beginX );
    double valueW = xMap.transform( _endX ) - valueX;
    QRectF rec(valueX,canvasRect.y(),valueW,canvasRect.height());
    QwtPainter::drawRect(painter,rec);

}
