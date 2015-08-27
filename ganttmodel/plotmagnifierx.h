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
 QwtPlotZoomer * m_zoomer;
protected:
  virtual void widgetWheelEvent(QWheelEvent *wheelEvent);

public:
  explicit PlotMagnifierX(QwtPlotCanvas *canvas);
  void SetZoomBase(bool ok);

  void setYAxis(QwtPlot::Axis axis);

 ~PlotMagnifierX();
};

#endif // PLOTMAGNIFIERX_H
