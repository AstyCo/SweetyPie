#ifndef PLOTNAVIGATOR_H
#define PLOTNAVIGATOR_H

#include <qwt/qwt_plot_magnifier.h>
#include <qwt/qwt_panner.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_zoomer.h>
#include <qwt/qwt_plot_panner.h>

#include <QObject>
#include <QWheelEvent>

class PlotNavigator : public QwtPlotMagnifier
{
Q_OBJECT

public:
  explicit PlotNavigator(QwtPlotCanvas *canvas);
  ~PlotNavigator();

  void setZoomRectEnabled(bool enabled);

  void setZoomBase(bool ok);
  void rescale(qreal scaleFactor, QPoint anchorPoint);
  void zoom(const QRectF &rect);

signals:
  void scaleChanged(qreal scaleFactor, QPoint mousePos);
  void panned(int, int);
  void zoomed(const QRectF &rect);

protected:
  virtual void widgetWheelEvent(QWheelEvent *wheelEvent);
  void zoom(QwtPlotZoomer *zoomer, qreal scaleFactor, QPoint mousePos);

private slots:
  void onPlotPanned(int dx, int dy);  

private:
 QwtPlotZoomer * m_zoomerPrimary;
 QwtPlotZoomer * m_zoomerSecondary;
 QwtPlotPanner *m_panner;

 int m_dx;
 int m_dy;
};

#endif // PLOTNAVIGATOR_H
