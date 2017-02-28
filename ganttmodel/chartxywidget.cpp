#include "chartxywidget.h"
#include "ui_chartxywidget.h"

#include <cmath>

#include <QScrollBar>
#include <QSplitter>
#include <QColorDialog>
#include <QMenu>
#include <qwt_picker_machine.h>

#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_symbol.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_plot_panner.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_series_data.h>
#include <qwt/qwt_scale_draw.h>

#include <qwt/qwt_color_map.h>
#include <qwt/qwt_raster_data.h>

#include "plotnavigator.h"
#include "curvedetailsgroupbox.h"
#include "plotinterval.h"

ChartXYWidget::ChartXYWidget(QWidget * parent) :
  QWidget(parent),
  ui(new Ui::ChartXYWidget)
{
  Q_INIT_RESOURCE(images);
  ui->setupUi(this);

  setLeftAxisMargin(0);
  setRightAxisMargin(0);

  createActionsToolBar();

  ui->m_plot->setCanvasBackground( Qt::white );

  //ui->m_plot->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
  ui->m_plot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignCenter | Qt::AlignBottom);

  // сетка
  m_pGrid = new QwtPlotGrid();

  // добавляем возможность масштабирования колесом мыши
  m_navigator = new PlotNavigator(ui->m_plot->canvas());
  connect(m_navigator, SIGNAL(scaleChanged(qreal, QPoint)), SIGNAL(scaleChanged(qreal, QPoint)));
  connect(m_navigator, SIGNAL(panned(int, int)), SIGNAL(panned(int, int)));
  connect(m_navigator, SIGNAL(zoomed(QRectF)), SIGNAL(zoomed(QRectF)));

  // объект отслеживает перемещение и нажатие кнопки мыши
  m_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                               QwtPicker::CrossRubberBand, QwtPicker::AlwaysOff, ui->m_plot->canvas());
  QwtPickerMachine* pickerMachine = new QwtPickerClickPointMachine();
  m_picker->setStateMachine(pickerMachine);
  // отключить перемещение курсора клавишами влево вправо
  m_picker->setKeyPattern(QwtEventPattern::KeyLeft, Qt::Key_unknown);
  m_picker->setKeyPattern(QwtEventPattern::KeyRight, Qt::Key_unknown);


  m_pMinLeftMarker = 0;
  m_pMaxLeftMarker = 0;
  m_pMinRightMarker = 0;
  m_pMaxRightMarker = 0;

  m_intervalSelectorModel = new ChartIntervalSelector(this, m_navigator);
  m_showLegend = false;

  connect(m_actionsToolBar->getChartAction(caSelectInterval), SIGNAL(toggled(bool)),
          m_intervalSelectorModel, SLOT(onAction_SelectInterval_toggled(bool)));
  connect(m_actionsToolBar->getChartAction(caSelectTarget), SIGNAL(toggled(bool)),
          m_intervalSelectorModel, SLOT(onAction_SelectTarget_toggled(bool)));

  m_pointSelectorModel = new ChartPointSelector(this,m_navigator);

  connect(m_picker, SIGNAL(selected(const QPointF&)), m_intervalSelectorModel, SLOT(onCurvePointSelected(const QPointF&)));
  connect(m_picker, SIGNAL(selected(const QPointF&)), m_pointSelectorModel, SLOT(onCurvePointSelected(const QPointF&)));
  connect(m_picker,SIGNAL(selected(QPointF)),this,SIGNAL(selected(QPointF)));

  setPanelCurveDetailsVisible(true);  

  createMenuIntervals();
  createMenuMaxMin();

  m_countLastPoints = 0;
  m_timerOnline = new QTimer(this);
  m_timerOnline->setInterval(1000);
  connect(m_timerOnline, SIGNAL(timeout()), this, SLOT(fullReplot()));
  connect(m_timerOnline, SIGNAL(timeout()), this, SLOT(calcDetailsPanel()));
}

ChartXYWidget::~ChartXYWidget()
{
  clearChart();
}


void ChartXYWidget::fullReplot()
{
  m_intervalSelectorModel->clearAllSelections();
  autoZoom();
}

void ChartXYWidget::autoZoom()
{
  // автоматически подобрать масштаб
  ui->m_plot->setAxisAutoScale(QwtPlot::xBottom);
  ui->m_plot->setAxisAutoScale(QwtPlot::yRight);
  ui->m_plot->setAxisAutoScale(QwtPlot::yLeft);
  ui->m_plot->replot();
  ui->m_plot->setAxisAutoScale(QwtPlot::xBottom, false);
  ui->m_plot->setAxisAutoScale(QwtPlot::yRight, false);
  ui->m_plot->setAxisAutoScale(QwtPlot::yLeft, false);
  // установить новый масштаб в качестве базового
  m_navigator->setZoomBase(true);
}

void ChartXYWidget::setGrid(bool b)
{
  QAction * act = m_actionsToolBar->getChartAction(caGrid);
  bool block = act->blockSignals(true);
  act->setChecked(b);
  act->blockSignals(block);
  onAction_grid_toggled(b);
}

long ChartXYWidget::findPointIndexByPos(const QPointF &pos, SearchDirection direction, int curveId)
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

CurveIndex ChartXYWidget::findClosestPointAllCurves(const QPointF &pos, SearchDirection direction) const
{

  if(m_curves.isEmpty())
    return CurveIndex();

  double minLenth = 2147483647;
  CurveIndex rez;
  if (direction == sdLeft)
  {
    for(int j = 0; j < m_curves.count(); j++)
    {
      if (m_curves[j]->isVisible() && m_curves[j]->dataSize() > 0)
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
      if (! m_curves[j]->isVisible() || m_curves[j]->dataSize() == 0)
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
          if (m_curves[j]->isVisible() && m_curves[j]->dataSize() > 0)
          {
              bool isFind = false;
              QPointF p;
              for(long i = 0; i < m_curves[j]->dataSize(); i++)
              {
                  p = getTransformedPoint(j, i);
                  if (pos.x() < 0)  //Кликнули слева от 0 по Х
                  {
                      if (p.x() >= 0) continue;  //Если точка в списке по Х справа от 0, продолжаем со сл. точкой
                      else                   //Иначе, смотрим насколько она близко
                      {
                          double length = calcDistanceOnX(p, pos);
                          if (length < minLenth)
                          {
                              rez.indexCurve = j;
                              rez.indexPoint = i;
                              minLenth = length;
                              isFind = true;  //Запоминаем, что в левой половине плоскости была точка
                          }
                      }
                  }
                  else if (pos.x() >=0) //Кликнули справа от 0 по Х
                  {
                      if (p.x() < 0) continue; //Если точка в списке по Х слева от 0, продолжаем со сл. точкой
                      else  //Иначе, смотрим насколько она близко
                      {
                          double length = calcDistanceOnX(p, pos);
                          if (length < minLenth)
                          {
                              rez.indexCurve = j;
                              rez.indexPoint = i;
                              minLenth = length;
                              isFind = true;  //Запоминаем, что в левой половине плоскости была точка
                          }
                      }
                  }
              }
              if (!isFind) //Кликнули в той половине, где нет точек
              {
                  QPointF prev;
                  for(long i = 0; i < m_curves[j]->dataSize(); i++)
                  {
                      p = getTransformedPoint(j, i);
                      double length = calcDistanceOnX(p, prev); //Ближайшая к точке с координатами (0;0)
                      if (length < minLenth)
                      {
                          rez.indexCurve = j;
                          rez.indexPoint = i;
                          minLenth = length;
                      }
                  }
              }
          }
      }
    return rez;
  }
}

QVector<QPointF> ChartXYWidget::trimData(const QVector<QPointF> data) const
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

QwtPlot *ChartXYWidget::getPlot() const
{
  return ui->m_plot;
}

void ChartXYWidget::zoom(const QRectF &rect)
{
  m_navigator->zoom(rect);
}

void ChartXYWidget::rescale(qreal scaleFactor, QPoint anchorPoint)
{
  m_navigator->rescale(scaleFactor, anchorPoint);
}

void ChartXYWidget::moveCanvas( int dx, int dy )
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

QList<CurveDetailsGroupBox *> ChartXYWidget::getPanelCurveDetailsList() const
{
    return m_panelCurveDetailsList;
}

ChartSettings ChartXYWidget::getSettings() const
{
    return m_settings;
}

ChartIntervalSelector *ChartXYWidget::intervalSelector() const
{
    return m_intervalSelectorModel;
}

ChartPointSelector *ChartXYWidget::pointSelector() const
{
    return m_pointSelectorModel;
}

bool ChartXYWidget::showLegend() const
{
  return m_showLegend;
}

void ChartXYWidget::setShowLegend(bool show)
{
  m_showLegend = show;

  if ((! m_settings.detailsPanelVisible) && m_showLegend)
    ui->m_plot->insertLegend(new QwtLegend(), QwtPlot::RightLegend);
  else
    ui->m_plot->insertLegend(NULL);
}

QList<CurveStyle> ChartXYWidget::getCurveStyles() const
{
  return m_curveStyles;
}

ChartActionsToolBar *ChartXYWidget::getActionsToolBar() const
{
  return m_actionsToolBar;
}

QScrollArea *ChartXYWidget::getPanelCurveDetails() const
{
  return ui->m_detailsPanel;
}

void ChartXYWidget::onAction_panelCurveDetails_toggled(bool checked)
{
  m_settings.detailsPanelVisible = checked;
  ui->m_detailsPanel->setVisible(checked);
  setShowLegend(m_showLegend);
}

void ChartXYWidget::onAction_grid_toggled(bool checked)
{
  m_settings.grid = checked;

  if(checked)
    m_pGrid->attach(ui->m_plot);
  else
    m_pGrid->detach();

  ui->m_plot->replot();
}

QList<CurveDetailsGroupBox *> ChartXYWidget::detailedPanels() const
{
  return m_panelCurveDetailsList;
}

void ChartXYWidget::setDetailedPanels(const QList<CurveDetailsGroupBox *> &detailedPanels)
{
  m_panelCurveDetailsList = detailedPanels;
}


int ChartXYWidget::countLastPoints() const
{
  return m_countLastPoints;
}

void ChartXYWidget::setCountLastPoints(int countLastPoints)
{
  m_countLastPoints = countLastPoints;
}


QList<QwtPlotCurve *> ChartXYWidget::curves() const
{
  return m_curves;
}

void ChartXYWidget::startOnlineReplot()
{
  //onAction_TimerOnline_toggled(true);
  getActionsToolBar()->getChartAction(caTimer)->setChecked(true);
}

void ChartXYWidget::stopOnlineReplot()
{
  //onAction_TimerOnline_toggled(false);
  //getActionsToolBar()->onToolButton_TimerOnline_toggled(false);
  getActionsToolBar()->getChartAction(caTimer)->setChecked(false);
}

QList<PlotInterval *> ChartXYWidget::intervals() const
{
  return m_zones;
}


QPointF ChartXYWidget::getTransformedPoint(int indexCurve, int indexPoint) const
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

QPointF ChartXYWidget::getTransformedPoint(const CurveIndex &index) const
{
  return getTransformedPoint(index.indexCurve, index.indexPoint);
}

double ChartXYWidget::calcDistance(const QPointF &p1, const QPointF &p2) const
{
  double x2 = (p2.x() - p1.x()) * (p2.x() - p1.x());
  double y2 = (p2.y() - p1.y()) * (p2.y() - p1.y());
  return sqrt(x2 + y2);
}
double ChartXYWidget::calcDistanceOnX(const QPointF &p1, const QPointF &p2) const
{
    double x = fabs(p2.x()) - fabs(p1.x());
    return fabs(x);
}
void ChartXYWidget::createActionsToolBar()
{
  m_actionsToolBar = new ChartActionsToolBar();
  m_actionsToolBar->setOrientation(Qt::Vertical);
  QHBoxLayout* lay = (QHBoxLayout*) this->layout();
  lay->addWidget(m_actionsToolBar);

  connect(m_actionsToolBar->getChartAction(caScale), SIGNAL(triggered(bool)), SLOT(autoZoom()));
  connect(m_actionsToolBar->getChartAction(caGrid), SIGNAL(toggled(bool)), SLOT(onAction_grid_toggled(bool)));
  //connect(m_actionsToolBar->getChartAction(caPaintIntervals), SIGNAL(triggered(bool)), SLOT(setIntervalVisible(bool)));  
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
                    << caSelectTarget
                    << caSelectIntervalPanel);
}

void ChartXYWidget::createMenuIntervals()
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

  /*
  QList<QAction *> rez;
  for(int i = 0; i < m_zones.count(); i++)
  {
    UtcDateTime beg = pointToDt(QPointF(m_zones[i]->beginX(), 0));
    UtcDateTime end = pointToDt(QPointF(m_zones[i]->endX(), 0));
    QAction *a = new QAction(beg.toStdString(0) + "-" + end.toStdString(0), menu);
    a->setCheckable(true);
    a->setChecked(true);
    connect(a, SIGNAL(toggled(bool)), this, SLOT(setIntervalVisible(bool)));
    rez.append(a);
  }
  menu->addActions(rez);

  m_actionsToolBar->setChartActionVisible(caZones, (! rez.isEmpty()));
  */
}

void ChartXYWidget::createMenuMaxMin()
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

QwtPlotCurve *ChartXYWidget::curve(int index) const
{
  return m_curves[index];
}


void ChartXYWidget::setCurveVisible(bool b)
{
  CurveDetailsGroupBox *curveDlg = (CurveDetailsGroupBox *)QObject::sender();
  int curveIdx = m_curves.indexOf(curveDlg->curve());
  m_pointSelectorModel->onCurveVisibilityChanged(curveIdx, b);

  ui->m_plot->replot();
}

void ChartXYWidget::setIntervalVisible(bool b)
{
  QAction * a = (QAction *)QObject::sender();
  QAction * menuHost = m_actionsToolBar->getChartAction(caZones);
  int index = menuHost->menu()->actions().indexOf(a);
  if(index >= 0 && index < m_zones.count())
  {
    m_zones[index]->setVisible(b);
    ui->m_plot->replot();
  }
}

void ChartXYWidget::setMaxMinLeftVisible(bool b)
{
  QAction * a = (QAction *)QObject::sender();
  if(a->text() == tr("Пределы левой оси"))
  {
    m_pMaxLeftMarker->setVisible(b);
    m_pMinLeftMarker->setVisible(b);
    ui->m_plot->replot();
  }
}

void ChartXYWidget::setMaxMinRightVisible(bool b)
{
  QAction * a = (QAction *)QObject::sender();
  if(a->text() == tr("Пределы правой оси"))
  {
    m_pMaxRightMarker->setVisible(b);
    m_pMinRightMarker->setVisible(b);
    ui->m_plot->replot();
  }
}

void ChartXYWidget::calcDetailsPanel()
{
  for(int i = 0; i < m_panelCurveDetailsList.count(); i++)
  {
    CurveDetailsGroupBox *details = m_panelCurveDetailsList.at(i);    
    details->calcStats(false);
  }
}

void ChartXYWidget::changeColorMaxMinLeft()
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

void ChartXYWidget::changeColorMaxMinRight()
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

void ChartXYWidget::setData(const QString &title, const QVector<QPointF> &data, QwtPlot::Axis axis)
{
  CurveStyle style = nextCurveStyle();
  m_curveStyles[m_curves.size()].id = title;

  QwtPlotCurve * curve = new QwtPlotCurve();
  curve->setTitle(title);
  curve->setYAxis(axis);  
  curve->setPen(style.pen);
  curve->setSamples(trimData(data));
  curve->attach(ui->m_plot);

  ChartCurveStats stats;
  stats.maxValue = QPointF(curve->maxXValue(), curve->maxYValue());
  stats.minValue = QPointF(curve->minXValue(), curve->minYValue());


  m_curves.append(curve);
  m_curvesStats.append(stats);      

  m_intervalSelectorModel->onCurveAdded(curve);

  CurveDetailsGroupBox * details = new CurveDetailsGroupBox(curve, this);
  details->setCurveCheckable(m_curves.size() > 1);
  connect(details, SIGNAL(colorChanged(QColor)), ui->m_plot, SLOT(replot()));
  connect(details, SIGNAL(visibledChanged(bool)), this, SLOT(setCurveVisible(bool)));
  QVBoxLayout* lay = (QVBoxLayout*) ui->widgetDetail->layout();
  lay->insertWidget(lay->count() - 1, details);


  m_panelCurveDetailsList.append(details);

  if (m_curves.size() == 2)
    m_panelCurveDetailsList.first()->setCurveCheckable(true);  

  calcDetailsPanel();
  fullReplot();

  emit curveDataChanged();
}

void ChartXYWidget::setLeftAxisMargin(int value)
{
  ui->widget_leftPadding->setMinimumWidth(value);
}

void ChartXYWidget::setRightAxisMargin(int value)
{
  ui->widget_rightPadding->setMinimumWidth(value);
}

void ChartXYWidget::updateData(int indexCurve, const QVector<QPointF> &data)
{

  m_curves[indexCurve]->setSamples(trimData(data));

  if(!getActionsToolBar()->getChartAction(caTimer)->isVisible())
  {
    calcDetailsPanel();
    fullReplot();
  }
}

void ChartXYWidget::getCurvePointLabel(const CurveIndex &idx, QString &xLbl, QString &yLbl) const
{
  QwtPlotCurve *curve = m_curves[idx.indexCurve];
  QPointF p = curve->sample(idx.indexPoint);

  QString valY = QString::number(p.y(), 'f', 6);

  xLbl = tr("[%1]").arg(ui->m_plot->axisScaleDraw(curve->xAxis())->label(p.x()).text());

  yLbl = QString("[%1]").arg(valY);
}

void ChartXYWidget::setPanelCurveDetailsVisible(bool vis)
{  
  m_actionsToolBar->getChartAction(caDetailsPanel)->setChecked(vis);
}

bool ChartXYWidget::panelCurveDetailsVisible() const
{
  return ui->m_detailsPanel->isVisible();
}

void ChartXYWidget::setChartToolBarVisible(bool vis)
{
  m_actionsToolBar->setVisible(vis);
}

void ChartXYWidget::clearChart()
{
  for(int i = 0; i < m_curves.count(); i++)
    m_curves[i]->detach();

  m_curves.clear();
  m_curvesStats.clear();

  for(int i = 0; i < m_zones.count(); i++)
    m_zones[i]->detach();
  m_zones.clear();

  // чистим легенду интервалов
  for(int i=0; i<m_zoneLegendWidgets.size(); i++)
      delete m_zoneLegendWidgets[i];
  m_zoneLegendWidgets.clear();



  for(int i = 0; i < m_panelCurveDetailsList.count(); i++)
    delete m_panelCurveDetailsList[i];
  m_panelCurveDetailsList.clear();

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

  m_intervalSelectorModel->reset();

  ui->m_plot->replot();
}


void ChartXYWidget::setLeftAxis(const QString &title, double minLine, double maxLine, const QColor &defaultColor)
{
  ui->m_plot->setAxisTitle(QwtPlot::yLeft, title);
  //ui->m_plot->setAxisLabelRotation(QwtPlot::yLeft, -50.0);
  ui->m_plot->setAxisLabelAlignment(QwtPlot::yLeft, Qt::AlignLeft | Qt::AlignBottom);

  setLeftMinMaxValues(minLine, maxLine, defaultColor);


}

void ChartXYWidget::setRightAxis(const QString &title, double minLine, double maxLine, const QColor &defaultColor)
{
  ui->m_plot->enableAxis( QwtPlot::yRight );
  ui->m_plot->setAxisTitle(QwtPlot::yRight, title);
  //ui->m_plot->setAxisLabelRotation(QwtPlot::yRight, -50.0);

  setRightMinMaxValues(minLine, maxLine, defaultColor);
}

void ChartXYWidget::setLeftMinMaxValues(double minLine, double maxLine, const QColor &defaultColor)
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

void ChartXYWidget::setRightMinMaxValues(double minLine, double maxLine, const QColor &defaultColor)
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


void ChartXYWidget::addZone(const QString &name, double beginX, double endX, const QColor &c1, const QColor &c2)
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
  for(int i = 0; i < m_zones.count(); i++)
  {
    if(m_zones[i]->name() == name)
    {
      isFind = true;
      break;
    }
  }

  if(!isFind)
  {
    QVBoxLayout* lay = (QVBoxLayout*) ui->widgetDetail->layout();
    QHBoxLayout *hlay = new QHBoxLayout();

    QLabel * intervalColor = new QLabel(this);
    intervalColor->setPalette(QPalette(c1));
    intervalColor->setAutoFillBackground(true);
    intervalColor->setFrameShape(QFrame::Box);
    hlay->addWidget(intervalColor);

    QLabel * intervalName = new QLabel(this);
    intervalName->setText(name);
    hlay->addWidget(intervalName);

    lay->insertLayout(lay->count() - 1, hlay);
    m_zoneLegendWidgets.append(intervalColor);
    m_zoneLegendWidgets.append(intervalName);
  }

  m_zones.append(interval);

  createMenuIntervals();
}

void ChartXYWidget::onAction_TimerOnline_toggled(bool checked)
{
  if(checked)
    m_timerOnline->start();
  else
    m_timerOnline->stop();
}



void ChartXYWidget::onAction_chartSettings_triggered()
{  
  ChartSettingsDlg dlg(m_settings, m_curves, this);
  if (dlg.exec() == QDialog::Accepted)
  {
    updateChartSettings(dlg.getChartSettings());
  }
}

void ChartXYWidget::updateChartSettings(const ChartSettings &newSettings)
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
void ChartXYWidget::setAxisTitle(const QString &title, QwtPlot::Axis axis)
{
   ui->m_plot->setAxisTitle(axis, title);
}
