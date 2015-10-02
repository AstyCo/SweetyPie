#ifndef PLOTMAGNIFIERX_H
#define PLOTMAGNIFIERX_H

#include <qwt_plot_magnifier.h>
#include <qwt_panner.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_zoomer.h>

#include <QObject>
#include <QWheelEvent>

class PlotMagnifierX : public QwtPlotMagnifier
{
Q_OBJECT

public:
  explicit PlotMagnifierX(QwtPlotCanvas *canvas);
  ~PlotMagnifierX();

  void setPrimaryAxisGroup(int xAxis, int yAxis);
  void setSecondaryAxisGroup(int xAxis, int yAxis);
  void setSecondaryAxisGroupEnabled(bool enabled);
  void SetZoomBase(bool ok);
  void rescale(qreal scaleFactor, QPoint anchorPoint);

signals:
  void scaleChanged(qreal scaleFactor, QPoint mousePos);

protected:
  enum AxisGroup
  {
    setPrimary,
    setSecondary
  };

  virtual void widgetWheelEvent(QWheelEvent *wheelEvent);
  void zoom(AxisGroup axisSet, qreal scaleFactor, QPoint mousePos);

private:
 QwtPlotZoomer * m_zoomerPrimary;
 QwtPlotZoomer * m_zoomerSecondary;
 bool m_zoomerSecondaryEnabled;
};

#endif // PLOTMAGNIFIERX_H
