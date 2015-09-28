#include "plotmagnifierx.h"


PlotMagnifierX::PlotMagnifierX(QwtPlotCanvas *canvas) :
  QwtPlotMagnifier(canvas)
{
  // объект для собственно масштабирования графика
  m_zoomerFirst = new QwtPlotZoomer(canvas);
  // выключить перехват кнопок этим объектом
  // т.к. будем использовать его вручную
  m_zoomerFirst->setEnabled(false);
  setFirstAxisSet(QwtPlot::xBottom, QwtPlot::yLeft);
  // для масштабирования по второй оси Y
  m_zoomerSecond = new QwtPlotZoomer(canvas);
  m_zoomerSecond->setEnabled(false);
  setSecondAxisSet(QwtPlot::xBottom, QwtPlot::yRight);

  m_zoomerSecondEnabled = false;
}

void PlotMagnifierX::SetZoomBase(bool ok)
{
    m_zoomerFirst->setZoomBase(ok);
    if (m_zoomerSecondEnabled)
      m_zoomerSecond->setZoomBase(ok);
}

void PlotMagnifierX::setFirstAxisSet(int xAxis, int yAxis)
{
  m_zoomerFirst->setAxis(xAxis, yAxis);
}

void PlotMagnifierX::setSecondAxisSet(int xAxis, int yAxis)
{
  m_zoomerSecond->setAxis(xAxis, yAxis);
}

void PlotMagnifierX::widgetWheelEvent(QWheelEvent *wheelEvent)
{
  QPoint mousePos;
  qreal scaleFactor;

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

  zoom(setFirst, scaleFactor, mousePos);
  if (m_zoomerSecondEnabled)
    zoom(setSecond, scaleFactor, mousePos);

  wheelEvent->accept();
}

void PlotMagnifierX::zoom(AxisSet axisSet, qreal scaleFactor, QPoint mousePos)
{
  QwtPlot::Axis scaleAxisX;
  QwtPlot::Axis scaleAxisY;
  // Прямоугольник на текущем увеличении
  QRectF zoomRect;
  if (axisSet == setFirst)
  {
    zoomRect = m_zoomerFirst->zoomRect();
    scaleAxisX = (QwtPlot::Axis)m_zoomerFirst->xAxis();
    scaleAxisY = (QwtPlot::Axis)m_zoomerFirst->yAxis();
  }
  else
  {
    zoomRect = m_zoomerSecond->zoomRect();
    scaleAxisX = (QwtPlot::Axis)m_zoomerSecond->xAxis();
    scaleAxisY = (QwtPlot::Axis)m_zoomerSecond->yAxis();
  }

  // текущие ширина и высота
  qreal xw = zoomRect.width();
  qreal yw = zoomRect.height();

  // координаты центральной точки
  qreal xhw = xw/2.0 + zoomRect.left();
  qreal yhw = yw/2.0 + zoomRect.top(); // QRect top указывает на нижнюю область графика.

  // ширина и высота после масштабирования
  qreal xz = xw / scaleFactor;
  qreal yz = yw / scaleFactor;

  // вычисляем новый масштаб в центральной точке
  zoomRect.setLeft (zoomRect.left() +(xw - xz)/2.0);
  zoomRect.setBottom(zoomRect.bottom()-(yw - yz)/2.0);
  zoomRect.setRight (zoomRect.right() -(xw - xz)/2.0);
  zoomRect.setTop (zoomRect.top() +(yw - yz)/2.0);

  // на сколько сдвинуть график в сторону мыши
  qreal slideFactor = 1.0 - (1.0 / scaleFactor);
  // перемещаем в сторону мыши
  // получаем расстояние до указателя мыши от центра
  qreal mx = (this->plot()->invTransform(scaleAxisX, mousePos.x()) - xhw) * slideFactor;
  qreal my = (this->plot()->invTransform(scaleAxisY, mousePos.y()) - yhw) * slideFactor;

  // скорректировать положение на полученые смещения
  zoomRect.setLeft (zoomRect.left() + mx);
  zoomRect.setRight (zoomRect.right() + mx);
  zoomRect.setBottom(zoomRect.bottom()+ my);
  zoomRect.setTop (zoomRect.top() + my);

  if (axisSet == setFirst)
    m_zoomerFirst->zoom(zoomRect);
  else
    m_zoomerSecond->zoom(zoomRect);
}

void PlotMagnifierX::setSecondAxisEnabled(bool enabled)
{
  m_zoomerSecondEnabled = enabled;
}

PlotMagnifierX::~PlotMagnifierX()
{
  delete m_zoomerFirst;
  delete m_zoomerSecond;
}

