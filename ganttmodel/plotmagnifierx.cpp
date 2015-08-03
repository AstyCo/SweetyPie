#include "plotmagnifierx.h"

void PlotMagnifierX::widgetWheelEvent(QWheelEvent *wheelEvent)
{
  QPoint mousePos;
  QRectF zoomRect;
  float xw, yw, xhw, yhw, xz, yz;
  float slideFactor;
  float scaleFactor;

  // Zoom IN
  if (wheelEvent->delta() > 0)
  {
    scaleFactor = 1.2;
  }
  else
  {
    scaleFactor = 0.8;
  }

  // позиция мыши
  mousePos.setX(wheelEvent->x());
  mousePos.setY(wheelEvent->y());

  // на сколько сдвинуть график в сторону мыши
  slideFactor = 1.0 - (1.0 / scaleFactor);

  // Прямоугольник на текущем увеличении
  zoomRect = m_zoomer->zoomRect();

  // текущие ширина и высота
  xw = zoomRect.width();
  yw = zoomRect.height();

  // координаты центральной точки
  xhw = xw/2.0 + zoomRect.left();
  yhw = yw/2.0 + zoomRect.top(); // QRect top указывает на нижнюю область графика.

  // ширина и высота после масштабирования
  xz = xw / scaleFactor;
  yz = yw / scaleFactor;

  // вычисляем новый масштаб в центральной точке
  zoomRect.setLeft (zoomRect.left() +(xw - xz)/2.0);
  zoomRect.setBottom(zoomRect.bottom()-(yw - yz)/2.0);
  zoomRect.setRight (zoomRect.right() -(xw - xz)/2.0);
  zoomRect.setTop (zoomRect.top() +(yw - yz)/2.0);

  // перемещаем в сторону мыши
  // получаем расстояние до указателя мыши от центра
  float mx = (this->plot()->invTransform(QwtPlot::xBottom, mousePos.x()) - xhw) * slideFactor;
  float my = (this->plot()->invTransform(QwtPlot::yLeft, mousePos.y()) - yhw) * slideFactor;

  // скорректировать положение на полученые смещения
  zoomRect.setLeft (zoomRect.left() + mx);
  zoomRect.setRight (zoomRect.right() + mx);
  zoomRect.setBottom(zoomRect.bottom()+ my);
  zoomRect.setTop (zoomRect.top() + my);

  m_zoomer->zoom(zoomRect);
  wheelEvent->accept();
}

PlotMagnifierX::PlotMagnifierX(QwtPlotCanvas *canvas) :
  QwtPlotMagnifier(canvas)
{
  // объект для собственно масштабирования графика
  m_zoomer = new QwtPlotZoomer(canvas);
  // выключить перехват кнопок этим объектом
  // т.к. будем использовать его вручную
  m_zoomer->setEnabled(false);
}

void PlotMagnifierX::SetZoomBase(bool ok)
{
    m_zoomer->setZoomBase(ok);
}

void PlotMagnifierX::setYAxis(QwtPlot::Axis axis)
{
  m_zoomer->setAxis(QwtPlot::xBottom,axis);
}



PlotMagnifierX::~PlotMagnifierX()
{
  delete m_zoomer;
}

