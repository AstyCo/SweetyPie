#include "chartwidget.h"
#include "ui_chartwidget.h"

#include <cmath>

#include <QScrollBar>
#include <QSplitter>
#include <QColorDialog>
#include <QMenu>

#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_symbol.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_plot_panner.h>
#include <qwt/qwt_plot_picker.h>
#include <qwt/qwt_picker_machine.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_series_data.h>

#include <qwt/qwt_color_map.h>
#include <qwt/qwt_raster_data.h>

#include "plotnavigator.h"
#include "curvedetailsgroupbox.h"
#include "plotinterval.h"

ChartWidget::ChartWidget(QWidget * parent) :
  QWidget(parent),
  ui(new Ui::ChartWidget)
{
  Q_INIT_RESOURCE(images);
  ui->setupUi(this);

  setLeftAxisMargin(0);
  setRightAxisMargin(0);

  createActionsToolBar();

  ui->m_plot->setCanvasBackground( Qt::white );

  ui->m_plot->setAxisTitle(QwtPlot::xBottom, tr("Дата и время"));
  ui->m_plot->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
  //ui->m_plot->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
  ui->m_plot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignCenter | Qt::AlignBottom);

  // сетка
  m_pGrid = new QwtPlotGrid();

  // добавляем возможность масштабирования колесом мыши
  m_navigator = new PlotNavigator(ui->m_plot->canvas());
  connect(m_navigator, SIGNAL(scaleChanged(qreal, QPoint)), SIGNAL(scaleChanged(qreal, QPoint)));
  connect(m_navigator, SIGNAL(panned(int, int)), SIGNAL(panned(int, int)));
  connect(m_navigator, SIGNAL(zoomed(QRectF)), SIGNAL(zoomed(QRectF)));
  // создать маркеры, которыми будем помечать выбранную точку
  m_pMarker[0] = new QwtPlotMarker();
  m_pMarker[0]->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
  QwtSymbol * sym = new QwtSymbol(QwtSymbol::Diamond, QBrush(Qt::magenta, Qt::SolidPattern),
                                  QPen(Qt::black, 0.0, Qt::SolidLine), QSize(7, 7));
  m_pMarker[0]->setSymbol(sym);
  m_pMarker[0]->setLineStyle(QwtPlotMarker::VLine);
  m_pMarker[0]->setLinePen(QPen( Qt::black, 0, Qt::DashLine ));
  m_pMarker[0]->hide();
  m_pMarker[0]->attach(ui->m_plot);

  m_pMarker[1] = new QwtPlotMarker();
  m_pMarker[1]->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
  m_pMarker[1]->setLineStyle(QwtPlotMarker::HLine);
  m_pMarker[1]->setLinePen(QPen( Qt::black, 0, Qt::DashLine ));
  m_pMarker[1]->hide();
  m_pMarker[1]->attach(ui->m_plot);

  // создать маркеры, которыми будет обозначена область выделения
  for(int i = 0; i <= 1; i++)
  {
    m_pIntervalMarker[i] = new QwtPlotMarker();
    m_pIntervalMarker[i]->setLineStyle(QwtPlotMarker::VLine);
    m_pIntervalMarker[i]->setLinePen(QPen( Qt::red, 3, Qt::SolidLine ));
    m_pIntervalMarker[i]->hide();
    m_pIntervalMarker[i]->attach(ui->m_plot);
  }

  m_pTargetingMarker = new QwtPlotMarker();
  m_pTargetingMarker->setLineStyle(QwtPlotMarker::VLine);
  m_pTargetingMarker->setLinePen(QPen( Qt::magenta, 3, Qt::SolidLine ));
  m_pTargetingMarker->hide();
  //m_pTargetingMarker->setZ(100);
  m_pTargetingMarker->attach(ui->m_plot);

  m_pMinLeftMarker = 0;
  m_pMaxLeftMarker = 0;
  m_pMinRightMarker = 0;
  m_pMaxRightMarker = 0;

  // объект отслеживает перемещение и нажатие кнопки мыши
  m_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                               QwtPicker::CrossRubberBand, QwtPicker::AlwaysOff, ui->m_plot->canvas());
  QwtPickerMachine* pickerMachine = new QwtPickerClickPointMachine();
  m_picker->setStateMachine(pickerMachine);
  // отключить перемещение курсора клавишами влево вправо
  m_picker->setKeyPattern(QwtEventPattern::KeyLeft, Qt::Key_unknown);
  m_picker->setKeyPattern(QwtEventPattern::KeyRight, Qt::Key_unknown);

  m_keyEventHandler = new PlotKeyEventHandler(ui->m_plot);
  ui->m_plot->installEventFilter(m_keyEventHandler);

  connect(m_picker, SIGNAL(selected(const QPointF&)), this, SLOT(onCurvePointSelected(const QPointF&)));
  connect(m_keyEventHandler, SIGNAL(nextPointSelected(bool)), SLOT(onNextCurvePointSelected(bool)));
  //connect(m_panner, SIGNAL(panned(int, int)), SLOT(onPlotPanned()));

  m_selectedPointIndex = CurveIndex();
  m_selectionState = ssNone;
  m_hasSelection = false;

  setPanelCurveDetailsVisible(true);

  createMenuIntervals();
  createMenuMaxMin();

  m_countLastPoints = 0;
  m_timerOnline = new QTimer(this);
  m_timerOnline->setInterval(1000);
  connect(m_timerOnline, SIGNAL(timeout()), this, SLOT(fullReplot()));
  connect(m_timerOnline, SIGNAL(timeout()), this, SLOT(calcDetailsPanel()));
}

ChartWidget::~ChartWidget()
{
  clearChart();
}

void ChartWidget::fullReplot()
{
  // скрыть маркеры
  m_pMarker[0]->hide();
  m_pMarker[1]->hide();
  m_pIntervalMarker[0]->hide();
  m_pIntervalMarker[1]->hide();
  m_pTargetingMarker->hide();
  autoZoom();
}

void ChartWidget::autoZoom()
{
  // автоматически подобрать масштаб
  ui->m_plot->setAxisAutoScale(QwtPlot::xBottom);
  ui->m_plot->setAxisAutoScale(QwtPlot::yRight);
  ui->m_plot->setAxisAutoScale(QwtPlot::yLeft);
  ui->m_plot->replot();
  // установить новый масштаб в качестве базового
  m_navigator->setZoomBase(true);
}

void ChartWidget::setGrid(bool b)
{
  QAction * act = m_actionsToolBar->getChartAction(caGrid);
  bool block = act->blockSignals(true);
  act->setChecked(b);
  act->blockSignals(block);
  onAction_grid_toggled(b);
}

void ChartWidget::showSelectionPoint(QwtText xLbl, QwtText yLbl, QPointF point, QwtPlot::Axis axis)
{
  m_pMarker[0]->setLabel(xLbl);
  m_pMarker[0]->setValue(point);
  m_pMarker[0]->show();

  m_pMarker[1]->setLabel(yLbl);
  m_pMarker[1]->setValue(point);
  m_pMarker[1]->setYAxis(axis);
  if(axis == QwtPlot::yRight)
    m_pMarker[1]->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
  else
    m_pMarker[1]->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);

  m_pMarker[1]->show();
  ui->m_plot->replot();
}

void ChartWidget::showSelectionInterval(QPointF start, QPointF end)
{
  m_pIntervalMarker[0]->setValue(start);
  m_pIntervalMarker[0]->show();
  m_pIntervalMarker[1]->setValue(end);
  m_pIntervalMarker[1]->show();
  ui->m_plot->replot();
}
void ChartWidget::showSelectionIntervalStart(QPointF start)
{
  m_pIntervalMarker[0]->setValue(start);
  m_pIntervalMarker[0]->show();
  ui->m_plot->replot();
}

void ChartWidget::showSelectionIntervalEnd(QPointF end)
{
  m_pIntervalMarker[1]->setValue(end);
  m_pIntervalMarker[1]->show();
  ui->m_plot->replot();
}

long ChartWidget::findPointIndexByPos(const QPointF &pos, SearchDirection direction, int curveId)
{
  QwtSeriesData<QPointF> *samples = m_curves.at(curveId)->data();
  size_t size = samples->size();
  if (size == 0)
    return -1;

  long endIdx = size - 1;

  if (direction == sdLeft)
  {
    for(int i = endIdx; i >= 0; i--)
    {
      if (samples->sample(i).x() <= pos.x())
        return i;
    }
    return 0;
  }
  else if (direction == sdRight)
  {
    for(int i = 0; i <= endIdx; i++)
    {
      if (samples->sample(i).x() >= pos.x())
        return i;
    }
    return endIdx;
  }
  else
  {
    QPointF p, prev;
    for(long i = 0; i <= endIdx; i++)
    {
      p = samples->sample(i);
      if (pos.x() <= p.x())
      {
        if (i > 0)
        {
          if ((p.x() - pos.x()) > (pos.x() - prev.x()))
          {
            p = prev;
            return (i - 1);
          }
        }
        return i;
      }
      prev = p;
    }
    return endIdx;
  }
}

CurveIndex ChartWidget::findClosestPointAllCurves(const QPointF &pos, SearchDirection direction)
{

  if(m_curves.isEmpty())
    return CurveIndex();

  double minLenth = 2147483647;
  CurveIndex rez;
  if (direction == sdLeft)
  {
    for(int j = 0; j < m_curves.count(); j++)
    {
      if(m_curves[j]->isVisible())
      {
        bool isFind = false;
        for(long i = m_curves[j]->dataSize() - 1; i >= 0; i--)
        {
          QPointF transP = getTransformedPoint(j, i);
          if (transP.x() <= pos.x())
          {
            double lenth = calcDistance(transP, pos);
            if(lenth < minLenth)
            {
              isFind = true;
              minLenth = lenth;
              rez.indexCurve = j;
              rez.indexPoint = i;
              break;
            }
          }
        }
        if(!isFind)
        {
          QPointF transP = getTransformedPoint(j, 0);
          double lenth = calcDistance(transP, pos);
          if(lenth < minLenth)
          {
            minLenth = lenth;
            rez.indexCurve = j;
            rez.indexPoint = 0;
          }
        }
      }
    }
    return rez;
  }
  else if (direction == sdRight)
  {
    for(int j = 0; j < m_curves.count(); j++)
    {
      if(! m_curves[j]->isVisible())
        continue;

      bool isFind = false;
      for(long i = 0; i < m_curves[j]->dataSize(); i++)
      {
        QPointF transP = getTransformedPoint(j, i);
        if (transP.x() >= pos.x())
        {
          double lenth = calcDistance(transP, pos);
          if(lenth < minLenth)
          {
            isFind = true;
            minLenth = lenth;
            rez.indexCurve = j;
            rez.indexPoint = i;
            break;
          }
        }
      }
      if(!isFind)
      {
        QPointF transP = getTransformedPoint(j, m_curves[j]->dataSize() - 1);
        double lenth = calcDistance(transP, pos);
        if(lenth < minLenth)
        {
          minLenth = lenth;
          rez.indexCurve = j;
          rez.indexPoint =  m_curves[j]->dataSize() - 1;
        }
      }

    }
    return rez;
  }
  else
  {
    for(int j = 0; j < m_curves.count(); j++)
    {
      if(m_curves[j]->isVisible())
      {
        bool isFind = false;
        QPointF p, prev;
        for(long i = 0; i < m_curves[j]->dataSize(); i++)
        {
          p = getTransformedPoint(j, i);
          if (pos.x() <= p.x())
          {
            if (i > 0)
            {
              double lenth = calcDistance(prev, pos);
              if(lenth < minLenth)
              {
                isFind = true;
                minLenth = lenth;
                rez.indexCurve = j;
                rez.indexPoint = i - 1;
              }
            }
            double lenth = calcDistance(p, pos);
            if(lenth < minLenth)
            {
              isFind = true;
              minLenth = lenth;
              rez.indexCurve = j;
              rez.indexPoint = i;
            }
            break;
          }
          prev = p;
        }
        if(!isFind)
        {
          QPointF transP = getTransformedPoint(j, m_curves[j]->dataSize() - 1);
          double lenth = calcDistance(transP, pos);
          if(lenth < minLenth)
          {
            minLenth = lenth;
            rez.indexCurve = j;
            rez.indexPoint =  m_curves[j]->dataSize() - 1;
          }
        }
      }
    }
    return rez;
  }
}

void ChartWidget::onCurvePointSelected(const QPointF &pos)
{
  if (m_selectionState == ssNone)
  {

    // найти и выделить точку с учетом выделенного интервала
    CurveIndex selPointIdx = findClosestPointAllCurves(pos);

    if(!selPointIdx.isValid())
      return;

    m_selectedPointIndex = selPointIdx;
    if (m_curves.size() > 1)
    {
      for(int i = 0; i < m_curves.count(); i++)
      {
        m_curves[i]->setPen(QPen(m_curves[i]->pen().color(), 0.8));
      }
      m_curves[m_selectedPointIndex.indexCurve]->setPen(QPen(m_curves[m_selectedPointIndex.indexCurve]->pen().color(), 2.4));
    }
    if(  m_pMinLeftMarker != 0 &&
         m_pMaxLeftMarker != 0)
    {
      m_pMinLeftMarker->setLinePen(QPen(m_pMinLeftMarker->linePen().color(), 1));
      m_pMaxLeftMarker->setLinePen(QPen(m_pMaxLeftMarker->linePen().color(), 1));

    }
    if(m_pMinRightMarker != 0 &&
        m_pMaxRightMarker != 0)
    {
      m_pMinRightMarker->setLinePen(QPen(m_pMinRightMarker->linePen().color(), 1));
      m_pMaxRightMarker->setLinePen(QPen(m_pMaxRightMarker->linePen().color(), 1));
    }

    if(m_curves[m_selectedPointIndex.indexCurve]->yAxis() == QwtPlot::yLeft)
    {
      if(m_pMinLeftMarker != 0 && m_pMaxLeftMarker != 0)
      {
        m_pMinLeftMarker->setLinePen(QPen(m_pMinLeftMarker->linePen().color(), 3));
        m_pMaxLeftMarker->setLinePen(QPen(m_pMaxLeftMarker->linePen().color(), 3));
      }
    }
    else if(m_curves[m_selectedPointIndex.indexCurve]->yAxis() == QwtPlot::yRight)
    {
      if(m_pMinRightMarker != 0 && m_pMaxRightMarker != 0)
      {
        m_pMinRightMarker->setLinePen(QPen(m_pMinRightMarker->linePen().color(), 3));
        m_pMaxRightMarker->setLinePen(QPen(m_pMaxRightMarker->linePen().color(), 3));
      }
    }

    drawMarkerOnCurve((QwtPlot::Axis)m_curves[m_selectedPointIndex.indexCurve]->yAxis());
    updateDetailsPanelsSelPoint();

    emit pointSelected(m_selectedPointIndex);
  }
  else if (m_selectionState == ssIntervalBegin)
  {
    setIntervalSelectionStart(pos);

    emit intervalSelectionStarted(pos);
  }
  else if (m_selectionState == ssIntervalEnd)
  {
    setIntervalSelectionEnd(pos);

    emit intervalSelectionEnded(pos);
  }
  else if (m_selectionState == ssTargetingPoint)
  {
    UtcDateTime dt = pointToDt(pos);
    setTargetingPoint(dt);

    emit targetingDtSet(dt);
  }
}

QPointF ChartWidget::dtToPoint(const UtcDateTime &dt, double y)
{
  return QPointF(double(dt.toMicrosecondsSinceEpoch() / 1000), y);
}

UtcDateTime ChartWidget::pointToDt(const QPointF &p)
{
  return UtcDateTime::fromMicrosecondsSinceEpoch((long long)p.x() * 1000);
}

QVector<QPointF> ChartWidget::trimData(const QVector<QPointF> data) const
{
  if(m_countLastPoints == 0 || m_countLastPoints > data.size())
    return data;
  else
  {
    QVector<QPointF> tmp;
    tmp = data.mid(data.size() - m_countLastPoints);
    return tmp;

  }
}

QwtPlot *ChartWidget::getPlot()
{
  return ui->m_plot;
}

void ChartWidget::zoom(const QRectF &rect)
{
  m_navigator->zoom(rect);
}

void ChartWidget::rescale(qreal scaleFactor, QPoint anchorPoint)
{
  m_navigator->rescale(scaleFactor, anchorPoint);
}

void ChartWidget::moveCanvas( int dx, int dy )
{
  if ( dx == 0 && dy == 0 )
    return;

  QwtPlot *plot = ui->m_plot;
  if ( plot == NULL )
    return;

  const bool doAutoReplot = plot->autoReplot();
  plot->setAutoReplot( false );

  for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
  {
    const QwtScaleMap map = plot->canvasMap( axis );

    const double p1 = map.transform( plot->axisScaleDiv( axis )->lowerBound() );
    const double p2 = map.transform( plot->axisScaleDiv( axis )->upperBound() );

    double d1, d2;
    if ( axis == QwtPlot::xBottom || axis == QwtPlot::xTop )
    {
      d1 = map.invTransform( p1 - dx );
      d2 = map.invTransform( p2 - dx );
    }
    else
    {
      d1 = map.invTransform( p1 - dy );
      d2 = map.invTransform( p2 - dy );
    }

    plot->setAxisScale( axis, d1, d2 );
  }

  plot->setAutoReplot( doAutoReplot );
  plot->replot();  
}
QList<CurveStyle> ChartWidget::getCurveStyles() const
{
  return m_curveStyles;
}

ChartActionsToolBar *ChartWidget::getActionsToolBar() const
{
  return m_actionsToolBar;
}

QScrollArea *ChartWidget::getPanelCurveDetails() const
{
  return ui->m_detailsPanel;
}

void ChartWidget::onAction_panelCurveDetails_toggled(bool checked)
{
  m_settings.detailsPanelVisible = checked;
  ui->m_detailsPanel->setVisible(checked);
}

void ChartWidget::onAction_grid_toggled(bool checked)
{
  m_settings.grid = checked;

  if(checked)
    m_pGrid->attach(ui->m_plot);
  else
    m_pGrid->detach();

  ui->m_plot->replot();
}

QList<CurveDetailsGroupBox *> ChartWidget::detailedPanels() const
{
  return m_panelCurveDetailsList;
}

void ChartWidget::setDetailedPanels(const QList<CurveDetailsGroupBox *> &detailedPanels)
{
  m_panelCurveDetailsList = detailedPanels;
}


void ChartWidget::updateCurvesIntervalStats()
{
  QPointF posBegin = m_pIntervalMarker[0]->value();
  QPointF posEnd = m_pIntervalMarker[1]->value();

  for(int i = 0; i < m_curves.size(); i++)
  {
    QwtPlotCurve *curve = m_curves.at(i);
    ChartCurveStats &stats = m_curvesStats[i];
    if (m_hasSelection)
    {
      long pointBeginIdx = findPointIndexByPos(posBegin, sdRight, i);
      long pointEndIdx = findPointIndexByPos(posEnd, sdLeft, i);
      if (pointBeginIdx != -1)
        stats.intervalBeginValue = curve->sample(pointBeginIdx);
      else
        stats.intervalBeginValue = QPointF();

      if (pointEndIdx != -1)
        stats.intervalEndValue = curve->sample(pointEndIdx);
      else
        stats.intervalEndValue = QPointF();
    }
    else
    {
      stats.intervalBeginValue = QPointF();
      stats.intervalEndValue = QPointF();
    }
  }
}

void ChartWidget::setIntervalSelectionByState(QPointF pos)
{
  if(m_curves.isEmpty())
    return;

  QPointF p = pos;
  CurveIndex selPointIdx;

  UtcDateTime selDt = pointToDt(p);

  if (!selDt.isValid())
    return;

  if (p.x() < m_curves.at(m_beginLimit.indexCurve)->sample(m_beginLimit.indexPoint).x())
  {
    selDt = pointToDt(m_curves.at(m_beginLimit.indexCurve)->sample(m_beginLimit.indexPoint));
  }
  else if (p.x() > m_curves.at(m_endLimit.indexCurve)->sample(m_endLimit.indexPoint).x())
  {
    selDt = pointToDt(m_curves.at(m_endLimit.indexCurve)->sample(m_endLimit.indexPoint));
  }

  //this->SetSelectionIntervalDates(selDt, m_curEndDt);
  int markerIdx = 0;
  if (m_selectionState == ssIntervalBegin)
  {
    selPointIdx = findClosestPointAllCurves(p, sdRight);
    m_selectionBeginPointIdx = selPointIdx;
    m_intervalBeginDt = selDt;
  }
  else if (m_selectionState == ssIntervalEnd)
  {
    if (m_intervalBeginDt > selDt)
    {
      // если конечная дата на самом деле является началом
      // интервала, то меняем местами
      m_intervalEndDt = m_intervalBeginDt;
      m_intervalBeginDt = selDt;

      // текущие маркеры начала заменяем на маркеры окончания
      m_pIntervalMarker[1]->setValue(m_pIntervalMarker[0]->value());
      m_pIntervalMarker[1]->show();
      ui->m_plot->replot();

      markerIdx = 0;
      m_selectionBeginPointIdx = findClosestPointAllCurves(p, sdRight);
      QPointF tmpP(dtToPoint(m_intervalEndDt, p.y()));
      m_selectionEndPointIdx = findClosestPointAllCurves(tmpP, sdLeft);
    }
    else
    {
      m_intervalEndDt = selDt;
      selPointIdx = findClosestPointAllCurves(p, sdLeft);
      m_selectionEndPointIdx = selPointIdx;
      markerIdx = 1;
    }
    // обновить поле "длительность"
    //TimeSpan duration = selDt - m_curStartDt;
    //this->SetSelectionInterval(duration);

    // обновить значения мин. сред. макс. для выбранного интервала
    //SetMinMeanMaxLbls(true);
  }
  // показать маркер начала/окончания выделения интервала
  m_pIntervalMarker[markerIdx]->setValue(p);
  m_pIntervalMarker[markerIdx]->show();  

  ui->m_plot->replot();
}
int ChartWidget::countLastPoints() const
{
  return m_countLastPoints;
}

void ChartWidget::setCountLastPoints(int countLastPoints)
{
  m_countLastPoints = countLastPoints;
}


QList<QwtPlotCurve *> ChartWidget::curves() const
{
  return m_curves;
}

void ChartWidget::startOnlineReplot()
{
  //onAction_TimerOnline_toggled(true);
  getActionsToolBar()->getChartAction(caTimer)->setChecked(true);
}

void ChartWidget::stopOnlineReplot()
{
  //onAction_TimerOnline_toggled(false);
  //getActionsToolBar()->onToolButton_TimerOnline_toggled(false);
  getActionsToolBar()->getChartAction(caTimer)->setChecked(false);
}

QList<PlotInterval *> ChartWidget::intervals() const
{
  return m_intervals;
}


QPointF ChartWidget::getTransformedPoint(int indexCurve, int indexPoint) const
{
  if(m_curves[indexCurve]->yAxis() == QwtPlot::yRight)
  {

    return QPointF(m_curves[indexCurve]->sample(indexPoint).x(),
                   ui->m_plot->invTransform(QwtPlot::yLeft, ui->m_plot->transform(QwtPlot::yRight, m_curves[indexCurve]->sample(indexPoint).y())));
  }
  else
  {
    return m_curves[indexCurve]->sample(indexPoint);
  }
}

QPointF ChartWidget::getTransformedPoint(const CurveIndex &index) const
{
  return getTransformedPoint(index.indexCurve, index.indexPoint);
}

double ChartWidget::calcDistance(const QPointF &p1, const QPointF &p2)
{
  double x2 = (p2.x() - p1.x()) * (p2.x() - p1.x());
  double y2 = (p2.y() - p1.y()) * (p2.y() - p1.y());
  return sqrt(x2 + y2);
}

void ChartWidget::createActionsToolBar()
{
  m_actionsToolBar = new ChartActionsToolBar();
  m_actionsToolBar->setOrientation(Qt::Vertical);
  QHBoxLayout* lay = (QHBoxLayout*) this->layout();
  lay->addWidget(m_actionsToolBar);

  connect(m_actionsToolBar->getChartAction(caScale), SIGNAL(triggered(bool)), SLOT(autoZoom()));
  connect(m_actionsToolBar->getChartAction(caGrid), SIGNAL(toggled(bool)), SLOT(onAction_grid_toggled(bool)));
  //connect(m_actionsToolBar->getChartAction(caPaintIntervals), SIGNAL(triggered(bool)), SLOT(setIntervalVisible(bool)));
  connect(m_actionsToolBar->getChartAction(caSelectInterval), SIGNAL(toggled(bool)), SLOT(setIntervalSelection(bool)));
  connect(m_actionsToolBar->getChartAction(caSelectTarget), SIGNAL(toggled(bool)), SLOT(onAction_SelectTarget_toggled(bool)));
  //connect(m_actionsToolBar->getChartAction(caMaxMinLines), SIGNAL(triggered(bool)), SLOT(setMaxMinLeftVisible(bool)));
  connect(m_actionsToolBar->getChartAction(caClear), SIGNAL(triggered(bool)), SLOT(fullReplot()));
  connect(m_actionsToolBar->getChartAction(caDetailsPanel), SIGNAL(toggled(bool)), SLOT(onAction_panelCurveDetails_toggled(bool)));
  connect(m_actionsToolBar->getChartAction(caSettingsDlg), SIGNAL(triggered(bool)), SLOT(onAction_chartSettings_triggered()));
  connect(m_actionsToolBar->getChartAction(caTimer),SIGNAL(toggled(bool)),this,SLOT(onAction_TimerOnline_toggled(bool)));
  m_actionsToolBar->setChartActions(QSet<ChartActions>()
                    << caScale
                    << caGrid
                    << caZones
                    << caSelectInterval
                    << caMaxMinLines
                    << caSelectTarget);
}

void ChartWidget::createMenuIntervals()
{
  QAction * intervalsAct = m_actionsToolBar->getChartAction(caZones);
  QMenu * menu = intervalsAct->menu();

  if (intervalsAct->menu() == NULL)
  {
    menu = new QMenu(m_actionsToolBar);
    intervalsAct->setMenu(menu);
  }
  else
    menu->clear();

  QList<QAction *> rez;
  for(int i = 0; i < m_intervals.count(); i++)
  {
    UtcDateTime beg = pointToDt(QPointF(m_intervals[i]->beginX(), 0));
    UtcDateTime end = pointToDt(QPointF(m_intervals[i]->endX(), 0));
    QAction *a = new QAction(beg.toStdString(0) + "-" + end.toStdString(0), menu);
    a->setCheckable(true);
    a->setChecked(true);
    connect(a, SIGNAL(toggled(bool)), this, SLOT(setIntervalVisible(bool)));
    rez.append(a);
  }
  menu->addActions(rez);

  m_actionsToolBar->setChartActionVisible(caZones, (! rez.isEmpty()));
}

void ChartWidget::createMenuMaxMin()
{
  QAction * maxMinAct = m_actionsToolBar->getChartAction(caMaxMinLines);
  QMenu * menu = maxMinAct->menu();

  if (maxMinAct->menu() == NULL)
  {
    menu = new QMenu(m_actionsToolBar);
    maxMinAct->setMenu(menu);
  }
  else
    menu->clear();

  QList<QAction *> rez;
  if(m_pMaxLeftMarker != 0 && m_pMinLeftMarker != 0)
  {
    QAction *amin = new QAction(tr("Пределы левой оси"), menu);
    amin->setCheckable(true);
    amin->setChecked(true);
    connect(amin, SIGNAL(toggled(bool)), this, SLOT(setMaxMinLeftVisible(bool)));
    rez.append(amin);

    QAction *acmin = new QAction(tr("Цвет пределов левой оси"), menu);
    connect(acmin, SIGNAL(triggered()), this, SLOT(changeColorMaxMinLeft()));
    rez.append(acmin);
  }

  if(m_pMaxRightMarker != 0 && m_pMinRightMarker != 0)
  {
    QAction *amin = new QAction(tr("Пределы правой оси"), menu);
    amin->setCheckable(true);
    amin->setChecked(true);
    connect(amin, SIGNAL(toggled(bool)), this, SLOT(setMaxMinRightVisible(bool)));
    rez.append(amin);

    QAction *acmin = new QAction(tr("Цвет пределов правой оси"), menu);
    connect(acmin, SIGNAL(triggered()), this, SLOT(changeColorMaxMinRight()));
    rez.append(acmin);
  }
  menu->addActions(rez);

  m_actionsToolBar->setChartActionVisible(caMaxMinLines, (! rez.isEmpty()));
}

QwtPlotCurve *ChartWidget::curve(int index) const
{
  return m_curves[index];
}


void ChartWidget::setCurveVisible(bool b)
{
  if(!b)
  {
    CurveDetailsGroupBox *curveDlg = (CurveDetailsGroupBox *)QObject::sender();
    bool isFind = false;
    for(int i = 0; i < m_curves.count(); i++)
      if(m_curves[i] == curveDlg->curve())
        if(m_selectedPointIndex.indexCurve == i)
        {
          isFind = true;
          fullReplot();
        }

    if(!isFind)
      ui->m_plot->replot();
  }
  else
  {
    ui->m_plot->replot();
  }
}

void ChartWidget::setIntervalVisible(bool b)
{
  QAction * a = (QAction *)QObject::sender();
  QAction * menuHost = m_actionsToolBar->getChartAction(caZones);
  int index = menuHost->menu()->actions().indexOf(a);
  if(index >= 0 && index < m_intervals.count())
  {
    m_intervals[index]->setVisible(b);
    ui->m_plot->replot();
  }
}

void ChartWidget::setMaxMinLeftVisible(bool b)
{
  QAction * a = (QAction *)QObject::sender();
  if(a->text() == tr("Пределы левой оси"))
  {
    m_pMaxLeftMarker->setVisible(b);
    m_pMinLeftMarker->setVisible(b);
    ui->m_plot->replot();
  }
}

void ChartWidget::setMaxMinRightVisible(bool b)
{
  QAction * a = (QAction *)QObject::sender();
  if(a->text() == tr("Пределы правой оси"))
  {
    m_pMaxRightMarker->setVisible(b);
    m_pMinRightMarker->setVisible(b);
    ui->m_plot->replot();
  }
}

void ChartWidget::calcDetailsPanel()
{
  for(int i = 0; i < m_panelCurveDetailsList.count(); i++)
  {
    CurveDetailsGroupBox *details = m_panelCurveDetailsList.at(i);

    if(m_hasSelection)
    {
      ChartCurveStats stats = m_curvesStats.at(i);
      details->setInterval(stats.intervalBeginValue.x(), stats.intervalEndValue.x());
    }
    else
      details->calcStats(false);

    details->setIntervalLabelsVisible(m_hasSelection);
  }
}

void ChartWidget::setIntervalSelection(bool b)
{
  if(b)
  {
    clearIntervalSelection();
    m_selectionState = ssIntervalBegin;
    m_navigator->setZoomRectEnabled(false);
  }
  else
  {
    if(m_selectionState == ssIntervalEnd)
      clearIntervalSelection();

    m_navigator->setZoomRectEnabled(true);
  }
}

void ChartWidget::changeColorMaxMinLeft()
{
  QAction * a = (QAction *)QObject::sender();
  if(a->text() == tr("Цвет пределов левой оси"))
  {
    QColor color;
    if(m_pMaxLeftMarker->linePen().color() == Qt::black)
      color = QColorDialog::getColor(Qt::white, this);
    else
      color = QColorDialog::getColor(m_pMaxLeftMarker->linePen().color(), this);
    if(color.isValid())
    {
      m_pMaxLeftMarker->setLinePen(QPen(color, m_pMaxLeftMarker->linePen().width()));
      m_pMinLeftMarker->setLinePen(QPen(color, m_pMinLeftMarker->linePen().width()));

      ui->m_plot->replot();
    }
  }
}

void ChartWidget::changeColorMaxMinRight()
{
  QAction * a = (QAction *)QObject::sender();
  if(a->text() == tr("Цвет пределов правой оси"))
  {
    QColor color;
    if(m_pMaxRightMarker->linePen().color() == Qt::black)
      color = QColorDialog::getColor(Qt::white, this);
    else
      color = QColorDialog::getColor(m_pMaxRightMarker->linePen().color(), this);
    if(color.isValid())
    {
      m_pMaxRightMarker->setLinePen(QPen(color, m_pMaxRightMarker->linePen().width()));
      m_pMinRightMarker->setLinePen(QPen(color, m_pMinRightMarker->linePen().width()));

      ui->m_plot->replot();
    }
  }
}

void ChartWidget::setData(const QString &title, const QVector<QPointF> &data, QwtPlot::Axis axis)
{
  CurveStyle style = nextCurveStyle();
  m_curveStyles[m_curves.size()].id = title;

  QwtPlotCurve * curve = new QwtPlotCurve();
  curve->setTitle(title);
  curve->setYAxis(axis);  
  curve->setPen(style.pen);
  curve->setSamples(trimData(data));
  curve->attach(ui->m_plot);

  if(m_curves.isEmpty())
  {
    m_beginLimit.indexCurve = m_curves.size();
    m_beginLimit.indexPoint = 0;

    m_endLimit.indexCurve = m_curves.size();
    m_endLimit.indexPoint = curve->dataSize() - 1;
  }
  else
  {
    if(curve->sample(0).x() < m_curves[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint).x())
    {
      m_beginLimit.indexCurve = m_curves.size();
      m_beginLimit.indexPoint = 0;
    }

    if(curve->sample(curve->dataSize() - 1).x() > m_curves[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint).x())
    {
      m_endLimit.indexCurve = m_curves.size();
      m_endLimit.indexPoint = curve->dataSize() - 1;
    }
  }

  ChartCurveStats stats;
  stats.maxValue = QPointF(curve->maxXValue(), curve->maxYValue());
  stats.minValue = QPointF(curve->minXValue(), curve->minYValue());

  m_curves.append(curve);
  m_curvesStats.append(stats);

  CurveDetailsGroupBox * details = new CurveDetailsGroupBox(curve, this);
  connect(details, SIGNAL(colorChanged(QColor)), ui->m_plot, SLOT(replot()));
  connect(details, SIGNAL(visibledChanged(bool)), this, SLOT(setCurveVisible(bool)));
  QVBoxLayout* lay = (QVBoxLayout*) ui->widgetDetail->layout();
  lay->insertWidget(lay->count() - 1, details);


  m_panelCurveDetailsList.append(details);

  calcDetailsPanel();
  fullReplot();
}

void ChartWidget::setLeftAxisMargin(int value)
{
  ui->widget_leftPadding->setMinimumWidth(value);
}

void ChartWidget::setRightAxisMargin(int value)
{
  ui->widget_rightPadding->setMinimumWidth(value);
}

void ChartWidget::updateData(int indexCurve, const QVector<QPointF> &data)
{

  m_curves[indexCurve]->setSamples(trimData(data));

  if(!getActionsToolBar()->getChartAction(caTimer)->isVisible())
  {
    calcDetailsPanel();
    fullReplot();
  }
}

void ChartWidget::setPanelCurveDetailsVisible(bool vis)
{  
  m_actionsToolBar->getChartAction(caDetailsPanel)->setChecked(vis);
}

bool ChartWidget::panelCurveDetailsVisible()
{
  return ui->m_detailsPanel->isVisible();
}

void ChartWidget::setChartToolBarVisible(bool vis)
{
  m_actionsToolBar->setVisible(vis);
}

void ChartWidget::clearChart()
{
  for(int i = 0; i < m_curves.count(); i++)
    m_curves[i]->detach();

  m_curves.clear();
  m_curvesStats.clear();

  m_beginLimit = CurveIndex();
  m_endLimit = CurveIndex();

  for(int i = 0; i < m_intervals.count(); i++)
    m_intervals[i]->detach();
  m_intervals.clear();

  // чистим легенду интервалов
  for(int i=0; i<m_intervalsWidgetsLegends.size(); i++)
      delete m_intervalsWidgetsLegends[i];
  m_intervalsWidgetsLegends.clear();



  for(int i = 0; i < m_panelCurveDetailsList.count(); i++)
    delete m_panelCurveDetailsList[i];
  m_panelCurveDetailsList.clear();

  m_selectedPointIndex = CurveIndex();
  m_selectionState = ssNone;
  m_hasSelection = false;
  m_selectionBeginPointIdx = CurveIndex();
  m_selectionEndPointIdx = CurveIndex();

  if(m_pMinLeftMarker != 0)
  {
    m_pMinLeftMarker->detach();
    delete m_pMinLeftMarker;
    m_pMinLeftMarker = 0;
  }
  if(m_pMaxLeftMarker != 0)
  {
    m_pMaxLeftMarker->detach();
    delete m_pMaxLeftMarker;
    m_pMaxLeftMarker = 0;
  }
  if(m_pMinRightMarker != 0)
  {
    m_pMinRightMarker->detach();
    delete m_pMinRightMarker;
    m_pMinRightMarker = 0;
  }
  if(m_pMaxRightMarker != 0)
  {
    m_pMaxRightMarker->detach();
    delete m_pMaxRightMarker;
    m_pMaxRightMarker = 0;
  }
  ui->m_plot->replot();
}

void ChartWidget::drawMarkerOnCurve(QwtPlot::Axis axis)
{
  if (!m_selectedPointIndex.isValid())
    return;

  QwtText txtX, txtY;

  QPointF p = m_curves[m_selectedPointIndex.indexCurve]->sample(m_selectedPointIndex.indexPoint);
  QString valY = QString::number(p.y(), 'f', 6);

  UtcDateTime refDt = pointToDt(p);
  QString refDtStr = refDt.toStdString(3);

  txtX = tr("[%1]").arg(refDtStr);

  txtY = QString("[%1]").arg(valY);

  showSelectionPoint(txtX, txtY, p, axis);

}

UtcDateTime ChartWidget::minimumDt()
{
  UtcDateTime rez;
  if (m_beginLimit.isValid())
    rez = pointToDt(m_curves[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint));

  return rez;
}

UtcDateTime ChartWidget::maximumDt()
{
  UtcDateTime rez;
  if (m_endLimit.isValid())
    rez = pointToDt(m_curves[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint));

  return rez;
}

void ChartWidget::setLeftAxis(const QString &title, double minLine, double maxLine, const QColor &defaultColor)
{
  ui->m_plot->setAxisTitle(QwtPlot::yLeft, title);
  //ui->m_plot->setAxisLabelRotation(QwtPlot::yLeft, -50.0);
  ui->m_plot->setAxisLabelAlignment(QwtPlot::yLeft, Qt::AlignLeft | Qt::AlignBottom);

  setLeftMinMaxValues(minLine, maxLine, defaultColor);


}

void ChartWidget::setRightAxis(const QString &title, double minLine, double maxLine, const QColor &defaultColor)
{
  ui->m_plot->enableAxis( QwtPlot::yRight );
  ui->m_plot->setAxisTitle(QwtPlot::yRight, title);
  //ui->m_plot->setAxisLabelRotation(QwtPlot::yRight, -50.0);

  setRightMinMaxValues(minLine, maxLine, defaultColor);
}

void ChartWidget::setLeftMinMaxValues(double minLine, double maxLine, const QColor &defaultColor)
{
  if(minLine != 0 || maxLine != 0)
  {
    m_pMinLeftMarker = new QwtPlotMarker();
    m_pMinLeftMarker->setLineStyle(QwtPlotMarker::HLine);
    m_pMinLeftMarker->setLinePen(QPen(defaultColor, 1, Qt::SolidLine ));
    m_pMinLeftMarker->setYAxis(QwtPlot::yLeft);
    m_pMinLeftMarker->setYValue(minLine);
    m_pMinLeftMarker->setLabel("{" + QString::number(minLine) + "}");
    m_pMinLeftMarker->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
    m_pMinLeftMarker->attach(ui->m_plot);

    m_pMaxLeftMarker = new QwtPlotMarker();
    m_pMaxLeftMarker->setLineStyle(QwtPlotMarker::HLine);
    m_pMaxLeftMarker->setLinePen(QPen(defaultColor, 1, Qt::SolidLine ));
    m_pMaxLeftMarker->setYAxis(QwtPlot::yLeft);
    m_pMaxLeftMarker->setYValue(maxLine);
    m_pMaxLeftMarker->setLabel("{" + QString::number(maxLine) + "}");
    m_pMaxLeftMarker->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_pMaxLeftMarker->attach(ui->m_plot);

    createMenuMaxMin();
    calcDetailsPanel();
  }
}

void ChartWidget::setRightMinMaxValues(double minLine, double maxLine, const QColor &defaultColor)
{
  if((minLine != 0 || maxLine != 0) && ui->m_plot->axisEnabled(QwtPlot::yRight ))
  {
    m_pMinRightMarker = new QwtPlotMarker();
    m_pMinRightMarker->setLineStyle(QwtPlotMarker::HLine);
    m_pMinRightMarker->setLinePen(QPen(defaultColor, 1, Qt::SolidLine ));
    m_pMinRightMarker->setYAxis(QwtPlot::yRight);
    m_pMinRightMarker->setYValue(minLine);
    m_pMinRightMarker->setLabel("{" + QString::number(minLine) + "}");
    m_pMinRightMarker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    m_pMinRightMarker->attach(ui->m_plot);

    m_pMaxRightMarker = new QwtPlotMarker();
    m_pMaxRightMarker->setLineStyle(QwtPlotMarker::HLine);
    m_pMaxRightMarker->setLinePen(QPen(defaultColor, 1, Qt::SolidLine ));
    m_pMaxRightMarker->setYAxis(QwtPlot::yRight);
    m_pMaxRightMarker->setYValue(maxLine);
    m_pMaxRightMarker->setLabel("{" + QString::number(maxLine) + "}");
    m_pMaxRightMarker->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
    m_pMaxRightMarker->attach(ui->m_plot);
    m_pMaxRightMarker->show();

    createMenuMaxMin();
    calcDetailsPanel();
  }
}

void ChartWidget::setIntervalSelectionByDates(UtcDateTime beginDt, UtcDateTime endDt)
{
  if (beginDt > endDt)
  {
    UtcDateTime tmpDt(beginDt);
    beginDt = endDt;
    endDt = tmpDt;
  }
  if(!m_beginLimit.isValid() || !m_endLimit.isValid())
    return;

  QPointF startLimitP = m_curves[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint);
  QPointF endLimitP = m_curves[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint);
  QPointF startP = dtToPoint(beginDt);
  QPointF endP = dtToPoint(endDt);

  if (startP.x() < startLimitP.x())
    startP = startLimitP;

  if (endP.x() > endLimitP.x())
    endP = endLimitP;

  m_selectionBeginPointIdx = findClosestPointAllCurves(startP, sdRight);
  m_selectionEndPointIdx = findClosestPointAllCurves(endP, sdLeft);

  m_hasSelection = true;

  showSelectionInterval(startP, endP);
  updateCurvesIntervalStats();
  calcDetailsPanel();
}

void ChartWidget::clearIntervalSelection()
{
  for(int i = 0; i < m_curves.size(); i++)
  {
    ChartCurveStats &stats = m_curvesStats[i];
    stats.intervalBeginValue = QPointF();
    stats.intervalEndValue = QPointF();
  }

  m_selectionState = ssNone;
  m_hasSelection = false;
  foreach(CurveDetailsGroupBox *panel, m_panelCurveDetailsList)
    panel->setIntervalLabelsVisible(false);

  m_intervalBeginDt = UtcDateTime();
  m_intervalEndDt = UtcDateTime();

  m_pIntervalMarker[0]->hide();
  m_pIntervalMarker[1]->hide();
  ui->m_plot->replot();
}

void ChartWidget::setIntervalSelectionStart(QPointF pos)
{
  m_selectionState = ssIntervalBegin;
  setIntervalSelectionByState(pos);
  m_selectionState = ssIntervalEnd;
  m_navigator->setZoomRectEnabled(false);
}

void ChartWidget::setIntervalSelectionEnd(QPointF pos)
{
  m_selectionState = ssIntervalEnd;
  m_hasSelection = true;
  setIntervalSelectionByState(pos);
  m_selectionState = ssNone;
  m_navigator->setZoomRectEnabled(true);

  QAction *selIntAct = m_actionsToolBar->getChartAction(caSelectInterval);
  selIntAct->setChecked(false);
  updateCurvesIntervalStats();
  calcDetailsPanel();
}

void ChartWidget::setSelectionModeTargetingPoint(bool b)
{
  clearTargetingPoint();

  if (b)
  {
    if (m_selectionState != ssNone)
      clearIntervalSelection();

    m_selectionState = ssTargetingPoint;
  }
}

void ChartWidget::setTargetingPoint(UtcDateTime dt)
{
  if (m_selectionState == ssTargetingPoint)
    m_selectionState = ssNone;

  QAction *selTargetAct = m_actionsToolBar->getChartAction(caSelectTarget);
  bool b = selTargetAct->blockSignals(true);
  selTargetAct->setChecked(false);
  selTargetAct->blockSignals(b);

  m_targetingDt = dt;
  if (!dt.isValid())
  {
    clearTargetingPoint();
    return;
  }
  QPointF point = dtToPoint(dt);
  m_pTargetingMarker->setValue(point);
  m_pTargetingMarker->show();
  for(int i = 0; i < m_curves.size(); i++)
  {
    QwtPlotCurve *curve = m_curves.at(i);
    ChartCurveStats &stats = m_curvesStats[i];
    long pointTargetingIdx = findPointIndexByPos(point, sdRight, i);

    if (pointTargetingIdx != -1)
      stats.intervalPointingValue = curve->sample(pointTargetingIdx);
    else
      stats.intervalPointingValue = QPointF();
  }
  ui->m_plot->replot();
}

void ChartWidget::clearTargetingPoint()
{
  if (m_selectionState == ssTargetingPoint)
    m_selectionState = ssNone;

  QAction *selTargetAct = m_actionsToolBar->getChartAction(caSelectTarget);
  bool b = selTargetAct->blockSignals(true);
  selTargetAct->setChecked(false);
  selTargetAct->blockSignals(b);

  m_pTargetingMarker->hide();
  for(int i = 0; i < m_curves.size(); i++)
    m_curvesStats[i].intervalPointingValue = QPointF();

  ui->m_plot->replot();
}

void ChartWidget::selectPointByIndex(CurveIndex idx)
{
  if ((idx.indexCurve < 0) || (idx.indexCurve > m_curves.size() - 1))
    return;

  if ((idx.indexPoint >= 0) && (idx.indexPoint < m_curves.at(idx.indexCurve)->dataSize()))
  {
    m_selectedPointIndex = idx;
    updateDetailsPanelsSelPoint();
    drawMarkerOnCurve((QwtPlot::Axis)m_curves[m_selectedPointIndex.indexCurve]->yAxis());
  }
}

void ChartWidget::addZone(const QString &name, double beginX, double endX, const QColor &c1, const QColor &c2)
{
  PlotInterval * interval = new PlotInterval();

  if(!c2.isValid())
    interval->setBackground(c1);
  else
    interval->setGradient(c1, c2);

  interval->setBeginX(beginX);
  interval->setEndX(endX);
  interval->setName(name);
  interval->attach(ui->m_plot);


  bool isFind = false;
  for(int i = 0; i < m_intervals.count(); i++)
  {
    if(m_intervals[i]->name() == name)
    {
      isFind = true;
      break;
    }
  }

  if(!isFind)
  {
    QVBoxLayout* lay = (QVBoxLayout*) ui->widgetDetail->layout();
    QHBoxLayout *hlay = new QHBoxLayout(this);

    QLabel * intervalColor = new QLabel(this);
    intervalColor->setPalette(QPalette(c1));
    intervalColor->setAutoFillBackground(true);
    intervalColor->setFrameShape(QFrame::Box);
    hlay->addWidget(intervalColor);

    QLabel * intervalName = new QLabel(this);
    intervalName->setText(name);
    hlay->addWidget(intervalName);

    lay->insertLayout(lay->count() - 1, hlay);
    m_intervalsWidgetsLegends.append(intervalColor);
    m_intervalsWidgetsLegends.append(intervalName);
  }

  m_intervals.append(interval);

  createMenuIntervals();
}

void ChartWidget::addZone(const QString &name, const UtcDateTime &begin, const UtcDateTime &end, const QColor &c1, const QColor &c2)
{
  addZone(name, dtToPoint(begin).x(), dtToPoint(end).x(), c1, c2);
}

void ChartWidget::onAction_TimerOnline_toggled(bool checked)
{
  if(checked)
    m_timerOnline->start();
  else
    m_timerOnline->stop();
}

void ChartWidget::onAction_SelectTarget_toggled(bool checked)
{
  if (checked)
  {
    if (m_selectionState != ssNone)
      clearIntervalSelection();

    m_selectionState = ssTargetingPoint;
  }
  else
    m_selectionState = ssNone;
}


void ChartWidget::onNextCurvePointSelected(bool prev)
{
  if (! m_selectedPointIndex.isValid())
    return;

  QwtPlotCurve *curve = m_curves.at(m_selectedPointIndex.indexCurve);
  QwtSeriesData<QPointF> *samples = curve->data();

  if (samples->size() == 0)
    return;

  int step = m_settings.keyStep;

  long startIdx = 0;
  long endIdx = samples->size() - 1;
  // если выбран интервал, то ограничиваем зону поиска точками из этого интервала
  if (m_hasSelection)
  {
    startIdx = m_selectionBeginPointIdx.indexPoint;
    endIdx = m_selectionEndPointIdx.indexPoint;
  }
  if (startIdx > endIdx)
  {
    // ни одна точка не попала в интервал
    return;
  }
  if (m_selectedPointIndex.indexPoint > -1)
  {
    // предыдущая точка имеется
    if (!prev)
    {

      if ((m_selectedPointIndex.indexPoint + step) < endIdx)
        m_selectedPointIndex.indexPoint += step;
      else
        m_selectedPointIndex.indexPoint = endIdx;
    }
    else
    {
      if ((m_selectedPointIndex.indexPoint - step) >= startIdx)
        m_selectedPointIndex.indexPoint -= step;
      else
        m_selectedPointIndex.indexPoint = startIdx;
    }
  }
  else
  {
    // предыдущая точка отсутствует
    if (!prev)
      m_selectedPointIndex.indexPoint = step;
    else
      m_selectedPointIndex.indexPoint = endIdx - step;

    if (m_selectedPointIndex.indexPoint < startIdx)
      m_selectedPointIndex.indexPoint = startIdx;
    else if (m_selectedPointIndex.indexPoint > endIdx)
      m_selectedPointIndex.indexPoint = endIdx;
  }
  drawMarkerOnCurve((QwtPlot::Axis)curve->yAxis());
  m_panelCurveDetailsList[m_selectedPointIndex.indexCurve]
      ->setCurrentSelPointIndex(m_selectedPointIndex.indexPoint);

  emit pointSelected(m_selectedPointIndex);
}


void ChartWidget::onAction_chartSettings_triggered()
{  
  ChartSettingsDlg dlg(m_settings, m_curves, this);
  if (dlg.exec() == QDialog::Accepted)
  {
    updateChartSettings(dlg.getChartSettings());
  }
}

void ChartWidget::updateDetailsPanelsSelPoint()
{
  for(int i = 0; i < m_panelCurveDetailsList.count(); i++)
  {
    CurveDetailsGroupBox *details = m_panelCurveDetailsList.at(i);
    if(m_selectedPointIndex.isValid()
      && (m_curves[m_selectedPointIndex.indexCurve] == details->curve()))
    {
      details->setCurrentSelPointIndex(m_selectedPointIndex.indexPoint);
    }
    else
      details->setCurrentSelPointIndex(-1);
  }
}


void ChartWidget::updateChartSettings(const ChartSettings &newSettings)
{
  m_settings = newSettings;

  setGrid(newSettings.grid);
  ui->m_plot->setAxisFont(QwtPlot::xBottom, newSettings.m_markerLabelFont);
  ui->m_plot->setAxisFont(QwtPlot::yLeft, newSettings.m_markerLabelFont);
  ui->m_plot->setAxisFont(QwtPlot::yRight, newSettings.m_markerLabelFont);

  updateChartSettingsPrivate();
  autoZoom();
  setPanelCurveDetailsVisible(newSettings.detailsPanelVisible);
  foreach(CurveDetailsGroupBox *panel, m_panelCurveDetailsList)
    panel->updateCurveColor();
}

