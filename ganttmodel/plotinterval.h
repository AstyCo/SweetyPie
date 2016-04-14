#ifndef PLOTINTERVAL_H
#define PLOTINTERVAL_H

#include <QPainter>

#include <qwt/qwt_plot_item.h>
#include <qwt/qwt_scale_map.h>

#include "ganttmodel_global.h"

class GANTTMODELSHARED_EXPORT PlotInterval : public QwtPlotItem
{
private:
    qreal _beginX;
    qreal _endX;
    QString _name;
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
    QString name() const;
    void setName(const QString &name);
};

#endif // PLOTINTERVAL_H
