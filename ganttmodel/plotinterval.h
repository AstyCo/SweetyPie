#ifndef PLOTINTERVAL_H
#define PLOTINTERVAL_H

#include "qwt/qwt_plot_item.h"

#include <QPainter>
#include "qwt/qwt_scale_map.h"

class PlotInterval : public QwtPlotItem
{
private:
    qreal _beginX;
    qreal _endX;
    QColor _background1;
    QColor _background2;
public:
    PlotInterval();

    virtual void draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect) const;
    qreal beginX() const;
    void setBeginX(qreal beginX);
    qreal endX() const;
    void setEndX(qreal endX);
    QColor background() const;
    void setBackground(const QColor &background);
    void setGradient(const QColor &c1, const QColor &c2);
};

#endif // PLOTINTERVAL_H
