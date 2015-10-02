#include "plotmagnifierx.h"


PlotMagnifierX::PlotMagnifierX(QwtPlotCanvas *canvas) :
  QwtPlotMagnifier(canvas)
{
  // объект для собственно масштабирования графика
  m_zoomerPrimary = new QwtPlotZoomer(canvas);
  // выключить перехват кнопок этим объектом
  // т.к. будем использовать его вручную
  m_zoomerPrimary->setEnabled(false);
  setPrimaryAxisGroup(QwtPlot::xBottom, QwtPlot::yLeft);
  // для масштабирования по второй оси Y
  m_zoomerSecondary = new QwtPlotZoomer(canvas);
  m_zoomerSecondary->setEnabled(false);
  setSecondaryAxisGroup(QwtPlot::xBottom, QwtPlot::yRight);

  m_zoomerSecondaryEnabled = false;
}

void PlotMagnifierX::SetZoomBase(bool ok)
{
    m_zoomerPrimary->setZoomBase(ok);
    if (m_zoomerSecondaryEnabled)
      m_zoomerSecondary->setZoomBase(ok);
}

void PlotMagnifierX::rescale(qreal scaleFactor, QPoint anchorPoint)
{
  zoom(setPrimary, scaleFactor, anchorPoint);
  if (m_zoomerSecondaryEnabled)
    zoom(setSecondary, scaleFactor, anchorPoint);
}

void PlotMagnifierX::setPrimaryAxisGroup(int xAxis, int yAxis)
{
  m_zoomerPrimary->setAxis(xAxis, yAxis);
}

void PlotMagnifierX::setSecondaryAxisGroup(int xAxis, int yAxis)
{
  m_zoomerSecondary->setAxis(xAxis, yAxis);
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

  zoom(setPrimary, scaleFactor, mousePos);
  if (m_zoomerSecondaryEnabled)
    zoom(setSecondary, scaleFactor, mousePos);

  wheelEvent->accept();

  emit scaleChanged(scaleFactor, mousePos);
}

void PlotMagnifierX::zoom(AxisGroup axisSet, qreal scaleFactor, QPoint mousePos)
{
  QwtPlot::Axis scaleAxisX;
  QwtPlot::Axis scaleAxisY;
  // Прямоугольник на текущем увеличении
  QRectF zoomRect;
  if (axisSet == setPrimary)
  {
    zoomRect = m_zoomerPrimary->zoomRect();
    scaleAxisX = (QwtPlot::Axis)m_zoomerPrimary->xAxis();
    scaleAxisY = (QwtPlot::Axis)m_zoomerPrimary->yAxis();
  }
  else
  {
    zoomRect = m_zoomerSecondary->zoomRect();
    scaleAxisX = (QwtPlot::Axis)m_zoomerSecondary->xAxis();
    scaleAxisY = (QwtPlot::Axis)m_zoomerSecondary->yAxis();
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

  if (axisSet == setPrimary)
    m_zoomerPrimary->zoom(zoomRect);
  else
    m_zoomerSecondary->zoom(zoomRect);
}

void PlotMagnifierX::setSecondaryAxisGroupEnabled(bool enabled)
{
  m_zoomerSecondaryEnabled = enabled;
}

PlotMagnifierX::~PlotMagnifierX()
{
  delete m_zoomerPrimary;
  delete m_zoomerSecondary;
}

