#ifndef PLOTMAGNIFIERX_H
#define PLOTMAGNIFIERX_H

#include <qwt_plot_magnifier.h>
#include <qwt_panner.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_zoomer.h>

#include <QWheelEvent>

class PlotMagnifierX : public QwtPlotMagnifier
{

private:
 enum AxisSet
 {
   setFirst,
   setSecond
 };

 QwtPlotZoomer * m_zoomerFirst;
 QwtPlotZoomer * m_zoomerSecond;
 bool m_zoomerSecondEnabled;

protected:
  virtual void widgetWheelEvent(QWheelEvent *wheelEvent);
  void zoom(AxisSet axisSet, qreal scaleFactor, QPoint mousePos);

public:
  explicit PlotMagnifierX(QwtPlotCanvas *canvas);
  ~PlotMagnifierX();

  void setFirstAxisSet(int xAxis, int yAxis);
  void setSecondAxisSet(int xAxis, int yAxis);
  void setSecondAxisEnabled(bool enabled);
  void SetZoomBase(bool ok);
};

#endif // PLOTMAGNIFIERX_H
