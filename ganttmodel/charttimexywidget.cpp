#include "charttimexywidget.h"

#include <QAction>

#include <qwt/qwt_plot_marker.h>

ChartTimeXYWidget::ChartTimeXYWidget(QWidget *parent)
  : ChartXYWidget(parent)
{
  getPlot()->setAxisTitle(QwtPlot::xBottom, tr("Дата и время"));
  getPlot()->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
}


void ChartTimeXYWidget::addZone(const QString &name, const UtcDateTime &begin, const UtcDateTime &end, const QColor &c1, const QColor &c2)
{
  ChartXYWidget::addZone(name, dtToPoint(begin).x(), dtToPoint(end).x(), c1, c2);
}

QPointF ChartTimeXYWidget::dtToPoint(const UtcDateTime &dt, double y)
{
  return QPointF(double(dt.toMicrosecondsSinceEpoch() / 1000), y);
}

UtcDateTime ChartTimeXYWidget::pointToDt(const QPointF &p)
{
  return UtcDateTime::fromMicrosecondsSinceEpoch((long long)p.x() * 1000);
}

void ChartTimeXYWidget::getCurvePointLabel(const CurveIndex &idx, QString &xLbl, QString &yLbl) const
{
  QwtPlotCurve *curve = m_curves[idx.indexCurve];
  QPointF p = curve->sample(idx.indexPoint);

  QString valY = QString::number(p.y(), 'f', 6);

  xLbl = tr("[%1]").arg(pointToDt(p).toStdString(3));

  yLbl = QString("[%1]").arg(valY);
}

