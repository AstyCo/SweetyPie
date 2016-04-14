#include "plotnavigator.h"


PlotNavigator::PlotNavigator(QwtPlotCanvas *canvas) :
  QwtPlotMagnifier(canvas)
{
  m_dx = 0;
  m_dy = 0;
  // отключить масштабирование при зажатой кнопке и перемещении мыши
  setMouseButton(Qt::NoButton);
  // объект для собственно масштабирования графика
  m_zoomerPrimary = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas);
  // отключить отображение координат точки при выделении прямоугольником
  m_zoomerPrimary->setTrackerMode(QwtPicker::ActiveOnly);
  m_zoomerPrimary->setRubberBandPen(QColor(Qt::darkGreen));
  // Изменить кнопку для сброса масштаба
  m_zoomerPrimary->setMousePattern(QwtEventPattern::MouseSelect2, Qt::MiddleButton);
  connect(m_zoomerPrimary, SIGNAL(zoomed(QRectF)), SIGNAL(zoomed(QRectF)));
  // для масштабирования по второй оси Y
  m_zoomerSecondary = new QwtPlotZoomer(QwtPlot::xTop, QwtPlot::yRight, canvas);
  m_zoomerSecondary->setTrackerMode(QwtPicker::AlwaysOff);
  m_zoomerSecondary->setRubberBand(QwtPicker::NoRubberBand);
  // Изменить кнопку для сброса масштаба
  m_zoomerSecondary->setMousePattern(QwtEventPattern::MouseSelect2, Qt::MiddleButton);

  // возможность перемещения по графику зажимая кнопку мыши
  m_panner = new QwtPlotPanner(canvas);
  m_panner->setCursor(QCursor(Qt::ClosedHandCursor));
  m_panner->setMouseButton(Qt::RightButton); // используя правую кнопку мыши
  connect(m_panner, SIGNAL(panned(int, int)), SLOT(onPlotPanned(int,int)));
}

void PlotNavigator::setZoomBase(bool ok)
{
    m_zoomerPrimary->setZoomBase(ok);
    m_zoomerSecondary->setZoomBase(ok);
}

void PlotNavigator::rescale(qreal scaleFactor, QPoint anchorPoint)
{
  zoom(m_zoomerPrimary, scaleFactor, anchorPoint);
  zoom(m_zoomerSecondary, scaleFactor, anchorPoint);
  m_dx = 0;
  m_dy = 0;

}

void PlotNavigator::zoom(const QRectF &rect)
{
  QwtPlotZoomer *zoomer;
  QwtPlot::Axis scaleAxisX;
  QwtPlot::Axis scaleAxisY;
  for(int i = 0; i < 2; i++)
  {
    if (i == 0)
      zoomer = m_zoomerPrimary;
    else
      zoomer = m_zoomerSecondary;

    scaleAxisX = (QwtPlot::Axis)zoomer->xAxis();
    scaleAxisY = (QwtPlot::Axis)zoomer->yAxis();

    QRectF newRect = zoomer->zoomRect();
    newRect.setLeft(rect.left());
    newRect.setRight(rect.right());
    bool b = zoomer->blockSignals(true);
    zoomer->zoom(newRect);
    zoomer->blockSignals(b);
  }
}

void PlotNavigator::widgetWheelEvent(QWheelEvent *wheelEvent)
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

  zoom(m_zoomerPrimary, scaleFactor, mousePos);
  zoom(m_zoomerSecondary, scaleFactor, mousePos);
  m_dx = 0;
  m_dy = 0;

  wheelEvent->accept();

  emit scaleChanged(scaleFactor, mousePos);
}

void PlotNavigator::zoom(QwtPlotZoomer *zoomer, qreal scaleFactor, QPoint mousePos)
{
  // Для уменьшения масштаба просто возвращаем
  // значения из стека zoomStack
  if (scaleFactor < 1.0)
  {
    m_dx = 0;
    m_dy = 0;

    bool b = zoomer->blockSignals(true);
    zoomer->zoom(-1);
    zoomer->blockSignals(b);
  }
  else
  {
    // Прямоугольник на текущем увеличении
    QRectF  zoomRect = zoomer->zoomRect();
    QwtPlot::Axis scaleAxisX = (QwtPlot::Axis)zoomer->xAxis();
    QwtPlot::Axis scaleAxisY = (QwtPlot::Axis)zoomer->yAxis();

    if (m_dx != 0)
    {
      qreal centerPointX = this->plot()->transform(scaleAxisX, zoomRect.x());
      double newX = this->plot()->invTransform(scaleAxisX, centerPointX + m_dx);
      zoomRect.moveLeft(newX);
    }
    if (m_dy != 0)
    {
      qreal centerPointY = this->plot()->transform(scaleAxisY, zoomRect.y());
      double newY = this->plot()->invTransform(scaleAxisY, centerPointY + m_dy);
      zoomRect.moveTop(newY);
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

    bool b = zoomer->blockSignals(true);
    zoomer->zoom(zoomRect);
    zoomer->blockSignals(b);
  }
}

void PlotNavigator::onPlotPanned(int dx, int dy)
{
  m_dx -= dx;
  m_dy -= dy;

  emit panned(dx, dy);
}

PlotNavigator::~PlotNavigator()
{
  delete m_zoomerPrimary;
  delete m_zoomerSecondary;
}

void PlotNavigator::setZoomRectEnabled(bool enabled)
{
  m_zoomerPrimary->setEnabled(enabled);
  m_zoomerSecondary->setEnabled(enabled);
}

