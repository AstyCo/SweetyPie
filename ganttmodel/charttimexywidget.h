#ifndef CHARTTIMEXYWIDGET_H
#define CHARTTIMEXYWIDGET_H

#include <qwt/qwt_scale_draw.h>

#include <timespan.h>
#include <utcdatetime.h>

#include "chartxywidget.h"

/// Класс для нижней временной оси
class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw():
        QwtScaleDraw()
    {

    }

    virtual QwtText label (double v) const
    {
        UtcDateTime dt;
        dt = UtcDateTime::fromMicrosecondsSinceEpoch((long long)v * 1000);
        return dt.toString("hh:mm:ss\ndd.MM.yy");
    }
};

class GANTTMODELSHARED_EXPORT ChartTimeXYWidget : public ChartXYWidget
{
  Q_OBJECT

public:
  ChartTimeXYWidget(QWidget *parent = 0);

  /// Добавляет специально выделенную зону
  void addZone(const QString &name, const UtcDateTime &begin, const UtcDateTime &endX, const QColor &c1, const QColor &c2);
  using ChartXYWidget::addZone;

  static QPointF dtToPoint(const UtcDateTime &dt, double y = 0);
  static UtcDateTime pointToDt(const QPointF &p);

protected:
  void getCurvePointLabel(const CurveIndex &idx, QString &xLbl, QString &yLbl) const;

private:

};

#endif // CHARTTIMEXYWIDGET_H
