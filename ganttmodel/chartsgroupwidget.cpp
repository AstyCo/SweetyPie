#include "chartsgroupwidget.h"
#include "ui_chartsgroupwidget.h"

#include <QQueue>

#include <qwt/qwt_scale_widget.h>
#include <qwt/qwt_plot_layout.h>
#include <qwt/qwt_scale_div.h>

#include <verticalscrollarea.h>

typedef Interval<double, double> NumericInterval;

qreal dtToPoint(const UtcDateTime &dt)
{
  return qreal(dt.toMicrosecondsSinceEpoch() / 1000);
}

UtcDateTime pointToDt(qreal p)
{
  return UtcDateTime::fromMicrosecondsSinceEpoch((long long)p * 1000);
}

ChartsGroupWidget::ChartsGroupWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ChartsGroupWidget)
{
  ui->setupUi(this);  

  createActionsToolBar();

  m_zoomActive = 0;
  m_syncChartsByAxisX = true;

  m_panelStatsVisible = false;
  setPanelSelectIntervalVisible(false);
  onAction_panelSelectionInterval_clicked();
  ui->frame_stats->hide();

  connect(ui->splitter_charts, SIGNAL(splitterMoved(int,int)), SLOT(scaleDivChanged()));  
  connect(ui->widget_SelectionPanel, SIGNAL(intervalChanged()), SLOT(onIntervalSelected()));
}

ChartsGroupWidget::~ChartsGroupWidget()
{ 
  delete ui;
}

void ChartsGroupWidget::createActionsToolBar()
{
  m_actionsToolBar = new ChartActionsToolBar();

  m_actionsToolBar->setOrientation(Qt::Vertical);
  ui->gridLayout_chartsGroupWidget->addWidget(m_actionsToolBar, 0, 2, 3, 1);

  m_actionsToolBar->setChartActions(QSet<ChartActions>()
                  << caScale
                  << caGrid
                  << caSelectInterval
                  << caSelectTarget
                  << caClear
                  << caDetailsPanel
                  << caSettingsDlg
                  << caSelectIntervalPanel);

  connect(m_actionsToolBar->getChartAction(caScale), SIGNAL(triggered(bool)), SLOT(onAction_autoZoom_clicked()));
  connect(m_actionsToolBar->getChartAction(caGrid), SIGNAL(toggled(bool)), SLOT(onAction_grid_toggled(bool)));
  //connect(m_actionsToolBar->getChartAction(caPaintIntervals), SIGNAL(triggered(bool)), SLOT(setIntervalVisible(bool)));
  connect(m_actionsToolBar->getChartAction(caSelectInterval), SIGNAL(toggled(bool)), SLOT(onAction_selectIntervalByMouse_toggled(bool)));
  connect(m_actionsToolBar->getChartAction(caSelectTarget), SIGNAL(toggled(bool)), SLOT(onAction_selectTarget_toggled(bool)));
  //connect(m_actionsToolBar->getChartAction(caMaxMinLines), SIGNAL(triggered(bool)), SLOT(setMaxMinLeftVisible(bool)));
  connect(m_actionsToolBar->getChartAction(caClear), SIGNAL(triggered(bool)), SLOT(onAction_clearChart_clicked()));
  connect(m_actionsToolBar->getChartAction(caSettingsDlg), SIGNAL(triggered(bool)), SLOT(onAction_chartSettings_triggered()));
  m_actionsToolBar->insertSeparator(m_actionsToolBar->getChartAction(caClear));

  connect(m_actionsToolBar->getChartAction(caSelectIntervalPanel), SIGNAL(toggled(bool)), SLOT(onAction_panelSelectionInterval_clicked()));
  connect(m_actionsToolBar->getChartAction(caDetailsPanel), SIGNAL(toggled(bool)), SLOT(onAction_panelCurveDetails_toggled(bool)));

  m_actionsToolBar->addSeparator();
}
ChartSettings ChartsGroupWidget::getSettings() const
{
  return m_settings;
}

void ChartsGroupWidget::setSettings(const ChartSettings &settings)
{
  m_settings = settings;
  for(int i = 0; i < m_charts.size(); i++)
  {
    ChartXYWidget *chart = m_charts.at(i);
    chart->updateChartSettings(m_settings);
  }

  m_actionsToolBar->getChartAction(caGrid)->setChecked(m_settings.grid);
  m_actionsToolBar->getChartAction(caDetailsPanel)->setChecked(m_settings.detailsPanelVisible);
}

ChartActionsToolBar *ChartsGroupWidget::getActionsToolBar() const
{
  return m_actionsToolBar;
}


void ChartsGroupWidget::addChart(ChartXYWidget *chart)
{
  insertChart(m_charts.size(), chart);
}

void ChartsGroupWidget::insertChart(int index, ChartXYWidget *chart)
{
  int chartNum = m_charts.size() + 1;
  QAction * zoomAct = new QAction(QString::number(chartNum), m_actionsToolBar);
  zoomAct->setCheckable(true);
  zoomAct->setToolTip(tr("Увеличить график №%1").arg(chartNum));
  connect(zoomAct, SIGNAL(toggled(bool)), SLOT(onToolButtonZoom_clicked()));
  m_actionsToolBar->addAction(zoomAct);

  QSize toolButtonSize = m_actionsToolBar->widgetForAction(m_actionsToolBar->getChartAction(caScale))->size();
  m_actionsToolBar->widgetForAction(zoomAct)->setFixedSize(toolButtonSize);
  m_chartZoomActions << zoomAct;

  if ((chartNum > 1) && (m_syncChartsByAxisX))
  {
    if (index >= m_charts.size())
      m_charts.last()->getPlot()->enableAxis(QwtPlot::xBottom, false);
    else
      chart->getPlot()->enableAxis(QwtPlot::xBottom, false);
  }

  connectChart(chart);
  ui->splitter_charts->insertWidget(index, chart);
  m_charts.insert(index, chart);
  chart->updateChartSettings(m_settings);
  chart->setPanelCurveDetailsVisible(m_settings.detailsPanelVisible);
  chart->setChartToolBarVisible(false);  

  updateSelectionPanel();  
}

bool ChartsGroupWidget::frameStatsEnabled() const
{
  return m_panelStatsVisible;
}

void ChartsGroupWidget::setFrameStatsEnabled(bool frameStatsEnabled)
{
  m_panelStatsVisible = frameStatsEnabled;
  if (frameStatsEnabled)
    ui->frame_stats->setVisible(((m_syncChartsByAxisX) || (m_charts.size() == 1)));
  else
    ui->frame_stats->hide();
}


void ChartsGroupWidget::onPointSelected(CurveIndex idx)
{
  ChartXYWidget * senderChart = static_cast<ChartXYWidget *>(QObject::sender());

  foreach (ChartXYWidget *cc, m_charts)
    if (cc != senderChart)
      cc->selectionModel()->selectPointByIndex(idx);

}

void ChartsGroupWidget::onIntervalSelectionStart(QPointF pos)
{
  ChartXYWidget * senderChart = static_cast<ChartXYWidget *>(QObject::sender());
  foreach (ChartXYWidget *cc, m_charts)
    if (cc != senderChart)
      cc->selectionModel()->setIntervalSelectionStart(pos);
}

void ChartsGroupWidget::onIntervalSelectionEnd(QPointF pos)
{
  ChartIntervalSelector *senderModel =
      qobject_cast<ChartIntervalSelector *>(QObject::sender());

  qreal begin = senderModel->intervalSelectionBegin();
  qreal end = senderModel->intervalSelectionEnd();
  ui->widget_SelectionPanel->setSelectedInterval(UtcDateTimeInterval(
                                                   pointToDt(begin),
                                                   pointToDt(end)));

  QAction *selIntAct = m_actionsToolBar->getChartAction(caSelectInterval);

  bool b = selIntAct->blockSignals(true);
  selIntAct->setChecked(false);
  selIntAct->blockSignals(b);
  foreach (ChartXYWidget *cc, m_charts)
    if (cc->selectionModel() != senderModel)
      cc->selectionModel()->setIntervalSelectionEnd(pos);

  emit intervalSelectionEnded(pos);
}

void ChartsGroupWidget::onTargetingDtSet(qreal value)
{
  ChartXYWidget * senderChart = static_cast<ChartXYWidget *>(QObject::sender());
  foreach (ChartXYWidget *cc, m_charts)
    if (cc != senderChart)
      cc->selectionModel()->setTargetingPoint(value);

  QAction *selTargetAct = m_actionsToolBar->getChartAction(caSelectTarget);
  bool b = selTargetAct->blockSignals(true);
  selTargetAct->setChecked(false);
  selTargetAct->blockSignals(b);

  emit targetPointSelected(pointToDt(value));
}

void ChartsGroupWidget::onScaleChanged(qreal scaleFactor, QPoint anchorPoint)
{
  ChartXYWidget * senderChart = static_cast<ChartXYWidget *>(QObject::sender());
  foreach (ChartXYWidget *cc, m_charts)
    if (cc != senderChart)
      cc->rescale(scaleFactor, anchorPoint);

  scaleDivChanged();
}

void ChartsGroupWidget::onZoomed(const QRectF &rect)
{
  ChartXYWidget * senderChart = static_cast<ChartXYWidget *>(QObject::sender());
  foreach (ChartXYWidget *cc, m_charts)
    if (cc != senderChart)
    {
      cc->zoom(rect);
    }

  scaleDivChanged();
}


void ChartsGroupWidget::onChartPanned(int dx, int dy)
{
  // при сдвиге графика учитываем только сдвиг по общей оси X
  Q_UNUSED(dy)
  ChartXYWidget * senderChart = static_cast<ChartXYWidget *>(QObject::sender());
  foreach (ChartXYWidget *cc, m_charts)
    if (cc != senderChart)
      cc->moveCanvas(dx, 0);

  scaleDivChanged();
}

void ChartsGroupWidget::scaleDivChanged()
{
  alignAxes(QwtPlot::yLeft);
  alignAxes(QwtPlot::yRight);
  //alignScaleBorder(QwtPlot::xBottom);

}

void ChartsGroupWidget::alignPanelsDetails()
{
  if (! m_settings.detailsPanelVisible)
    return;

  int minWidth = 0;
  foreach(ChartXYWidget *cc, m_charts)
  {
    int panelWidth = cc->getPanelCurveDetails()->widget()->minimumSizeHint().width();
    if (panelWidth > minWidth)
      minWidth = panelWidth;
  }
  foreach(ChartXYWidget *cc, m_charts)
  {
    VerticalScrollArea *v = (VerticalScrollArea *)cc->getPanelCurveDetails();
    v->setUserMinWidth(minWidth);
  }
}

void ChartsGroupWidget::resizeEvent(QResizeEvent *ev)
{
  QWidget::resizeEvent(ev);
  scaleDivChanged();
  alignPanelsDetails();
}

ChartXYWidget * ChartsGroupWidget::removeChartAt(int index)
{
  if ((index < 0) || (index >= m_charts.size()))
      return NULL;

  bool chartWithXAxisLabel = (index == (m_charts.size() - 1));

  ChartXYWidget *chartCtrl = m_charts.takeAt(index);
  QAction * zoomAct = m_chartZoomActions.takeLast();

  if ((! m_charts.isEmpty()) && (m_syncChartsByAxisX))
  {
    if (chartWithXAxisLabel)
      m_charts.last()->getPlot()->enableAxis(QwtPlot::xBottom, true);
  }

  chartCtrl->setParent(NULL);
  m_actionsToolBar->removeAction(zoomAct);
  delete zoomAct;

  return chartCtrl;
}

ChartXYWidget * ChartsGroupWidget::at(int index)
{
  if ((index < 0) || (index >= m_charts.size()))
      return NULL;
  else
    return m_charts.at(index);
}

void ChartsGroupWidget::clearChartsData()
{
  foreach(ChartXYWidget *cc, m_charts)
    cc->clearChart();
}

void ChartsGroupWidget::onToolButtonZoom_clicked()
{
  QAction * act = static_cast<QAction *>(QObject::sender());
  int chartNum = act->text().toInt();
  zoomChart(chartNum);
}


void ChartsGroupWidget::onIntervalSelected()
{
  UtcDateTimeInterval newInt = ui->widget_SelectionPanel->selectedInterval();
  for(int i = 0; i < m_charts.size(); i++)
  {
    ChartXYWidget *cc = m_charts.at(i);
    cc->selectionModel()->setIntervalSelection(dtToPoint(newInt.begin()),
                                               dtToPoint(newInt.end()));
  }
}

void ChartsGroupWidget::zoomChart(int newZoom)
{
  if (newZoom > m_charts.size())
    return;

  int oldZoom = m_zoomActive;

  if (newZoom == oldZoom)
    newZoom = 0;

  QSize base = ui->splitter_charts->size();
  QList<int> sizes;

  int idx = 0;
  foreach(ChartXYWidget *cc, m_charts)
  {
    bool visible;
    if (newZoom == 0)
    {
      visible = true;

      bool b = m_chartZoomActions.at(idx)->blockSignals(true);
      m_chartZoomActions.at(idx)->setChecked(false);
      m_chartZoomActions.at(idx)->blockSignals(b);

      sizes << base.height() / m_charts.size();

      if (m_syncChartsByAxisX)
      {
        if (idx < (m_charts.size() - 1))
          cc->getPlot()->enableAxis(QwtPlot::xBottom, false);
      }
    }
    else
    {
      if ((idx + 1) == newZoom)
      {
        visible = true;
        sizes << base.height();

        if (m_syncChartsByAxisX)
          cc->getPlot()->enableAxis(QwtPlot::xBottom, true);
      }
      else
      {
        visible = false;
        bool b = m_chartZoomActions.at(idx)->blockSignals(true);
        m_chartZoomActions.at(idx)->setChecked(false);
        m_chartZoomActions.at(idx)->blockSignals(b);
        sizes << 0;
      }
    }
    cc->setVisible(visible);

    idx++;
  }
  m_zoomActive = newZoom;

  ui->splitter_charts->setSizes(sizes);
}

void ChartsGroupWidget::interconnectCharts()
{
  foreach(ChartXYWidget *chart, m_charts)
    connectChart(chart);
}

void ChartsGroupWidget::connectChart(ChartXYWidget *chart)
{
  disconnect(chart, 0, this, 0);
  disconnect(chart->selectionModel(), 0, this, 0);

  if (m_syncChartsByAxisX)
  {
    ChartIntervalSelector *selModel = chart->selectionModel();
    connect(selModel, SIGNAL(pointSelected(CurveIndex)), SLOT(onPointSelected(CurveIndex)));
    connect(selModel, SIGNAL(intervalSelectionStarted(QPointF)), SLOT(onIntervalSelectionStart(QPointF)));
    connect(selModel, SIGNAL(intervalSelectionEnded(QPointF)), SLOT(onIntervalSelectionEnd(QPointF)));
    connect(selModel, SIGNAL(targetingPointSet(qreal)), SLOT(onTargetingDtSet(qreal)));

    connect(chart, SIGNAL(scaleChanged(qreal,QPoint)), SLOT(onScaleChanged(qreal,QPoint)));
    connect(chart, SIGNAL(zoomed(QRectF)), SLOT(onZoomed(QRectF)));
    connect(chart, SIGNAL(panned(int,int)), SLOT(onChartPanned(int,int)));
  }
  connect(chart, SIGNAL(curveDataChanged()), SLOT(updateSelectionPanel()));
}

bool ChartsGroupWidget::panelSelectIntervalVisible() const
{
  return m_panelSelectIntervalVisible;
}

void ChartsGroupWidget::setPanelSelectIntervalVisible(bool vis)
{  
  m_actionsToolBar->getChartAction(caSelectIntervalPanel)->setChecked(vis);
}


bool ChartsGroupWidget::syncChartsByAxisX() const
{
  return m_syncChartsByAxisX;
}

void ChartsGroupWidget::setSyncChartsByAxisX(bool syncChartsByAxisX)
{
  if (m_syncChartsByAxisX == syncChartsByAxisX)
    return;

  m_syncChartsByAxisX = syncChartsByAxisX;

  for(int i = 0; i < m_charts.size() - 1; i++)
  {
    ChartXYWidget *chart = m_charts.at(i);
      chart->getPlot()->enableAxis(QwtPlot::xBottom, (! m_syncChartsByAxisX));
  }

  interconnectCharts();
}

void ChartsGroupWidget::selectIntervalByDates(UtcDateTime beginDt, UtcDateTime endDt)
{
  foreach(ChartXYWidget *cc, m_charts)
      cc->selectionModel()->setIntervalSelection(dtToPoint(beginDt), dtToPoint(endDt));


  ui->widget_SelectionPanel->setSelectedInterval(UtcDateTimeInterval(beginDt, endDt));
}

void ChartsGroupWidget::setTargetingPoint(UtcDateTime dt)
{
  foreach(ChartXYWidget *cc, m_charts)
      cc->selectionModel()->setTargetingPoint(dtToPoint(dt));
}

void ChartsGroupWidget::clearTargetingPoint()
{
  foreach(ChartXYWidget *cc, m_charts)
      cc->selectionModel()->clearTargetingPoint();
}

UtcDateTime ChartsGroupWidget::getSelIntervalBeginDt()
{
  return ui->widget_SelectionPanel->selectedInterval().begin();
}

UtcDateTime ChartsGroupWidget::getSelIntervalEndDt()
{
  return ui->widget_SelectionPanel->selectedInterval().end();
}


void ChartsGroupWidget::onAction_panelSelectionInterval_clicked()
{  
  m_panelSelectIntervalVisible = m_actionsToolBar->getChartAction(caSelectIntervalPanel)->isChecked();
  ui->widget_SelectionPanel->setVisible(m_panelSelectIntervalVisible);
}
/*
void ChartsGroupWidget::on_toolButton_EndSelectionByMouse_clicked()
{
  foreach(ChartWidget *cc, m_chartControls)
    cc->clearSelectedInterval();

  updateSelectionPanel();
}
*/
void ChartsGroupWidget::updateSelectionPanel()
{
  if (m_charts.isEmpty())
    return;

  qreal startLimit = m_charts.first()->selectionModel()->begin();
  qreal endLimit = m_charts.first()->selectionModel()->end();
  NumericInterval newAvailInt(startLimit, endLimit);
  if ((! m_syncChartsByAxisX) && (m_charts.size() != 1))
  {
    foreach(ChartXYWidget *cc, m_charts)
    {
      qreal chartBegin = cc->selectionModel()->begin();
      qreal chartEnd = cc->selectionModel()->end();

      NumericInterval chartInt(chartBegin, chartEnd);
      newAvailInt += chartInt;
    }
  }
  UtcDateTimeInterval newDateTimeAvailInt(pointToDt(newAvailInt.begin()),
                                          pointToDt(newAvailInt.end()));
  ui->widget_SelectionPanel->setAvailableInterval(newDateTimeAvailInt);

  if (m_syncChartsByAxisX || (m_charts.size() == 1))
  {
    if (!m_charts.first()->curves().isEmpty())
    {
      ui->label_stat_points->setText(QString::number(m_charts.first()->curve(0)->dataSize()));

      QString beginEnd("%1 - %2");
      beginEnd = beginEnd.arg(newDateTimeAvailInt.begin().toStdString(0),
                              newDateTimeAvailInt.end().toStdString(0));
      ui->label_stat_beginDt_endDt->setText(beginEnd);

      ui->label_stat_duration->setText(newDateTimeAvailInt.length().toString());
    }
  }
}

void ChartsGroupWidget::onAction_autoZoom_clicked()
{
  foreach(ChartXYWidget *chart, m_charts)
    chart->autoZoom();

  scaleDivChanged();
}

void ChartsGroupWidget::onAction_grid_toggled(bool checked)
{
  m_settings.grid = checked;
  foreach(ChartXYWidget *chart, m_charts)
    chart->setGrid(checked);
}

void ChartsGroupWidget::onAction_clearChart_clicked()
{
    foreach(ChartXYWidget *chart, m_charts)
      chart->fullReplot();
}

void ChartsGroupWidget::onAction_selectIntervalByMouse_toggled(bool checked)
{
  if (checked)
  {
    QAction *selTargetAct = m_actionsToolBar->getChartAction(caSelectTarget);
    if (selTargetAct->isChecked())
      selTargetAct->setChecked(false);
  }

  foreach(ChartXYWidget *cc, m_charts)
      cc->selectionModel()->setIntervalSelection(checked);
}

void ChartsGroupWidget::alignAxes(int axis)
{
  if ( axis == QwtPlot::yLeft || axis == QwtPlot::yRight )
  {
      //double maxExtent = 0;
      int maxAxisWidth = 0;
      //QList<double> chartsExtents;
      for (int row = 0; row < m_charts.size(); row++)
      {
          QwtPlot *p = m_charts.at(row)->getPlot();
          if (p)
          {
              QwtScaleWidget *scaleWidget = p->axisWidget( axis );
              //QwtScaleDraw *sd = scaleWidget->scaleDraw();
              //sd->setMinimumExtent( 0.0 );

              int axisFullWidth = scaleWidget->width();
              if (axisFullWidth > maxAxisWidth)
                maxAxisWidth = axisFullWidth;
              /*
              double extent = sd->extent( scaleWidget->font() );
              chartsExtents << extent;
              if ( extent > maxExtent )
                  maxExtent = extent;
                  */
          }
      }

      for (int row = 0; row < m_charts.size(); row++)
      {
          QwtPlot *p = m_charts.at(row)->getPlot();
          if (p)
          {
              QwtScaleWidget *scaleWidget = p->axisWidget(axis);
              //scaleWidget->scaleDraw()->setMinimumExtent(maxExtent);
              //int margin = maxAxisWidth - scaleWidget->width() - (maxExtent - chartsExtents.at(row));
              int margin = maxAxisWidth - scaleWidget->width();
              if (axis == QwtPlot::yLeft)
                m_charts.at(row)->setLeftAxisMargin(margin);
              else
                m_charts.at(row)->setRightAxisMargin(margin);
          }
      }
  }
}

void ChartsGroupWidget::alignScaleBorder(int axis)
{
    int startDist = 0;
    int endDist = 0;

    if ( axis == QwtPlot::xTop )
    {
        QwtPlot *p = m_charts.last()->getPlot();
        if ( p )
            p->axisWidget( axis )->getBorderDistHint( startDist, endDist );

        for ( int row = 1; row < m_charts.size(); row++ )
        {
            QwtPlot *p = m_charts.at(row)->getPlot();
            if ( p )
                p->axisWidget( axis )->setMinBorderDist( startDist, endDist );
        }
    }
    else if ( axis == QwtPlot::xBottom )
    {
        QwtPlot *p = m_charts.last()->getPlot();

        if ( p )
            p->axisWidget( axis )->getBorderDistHint( startDist, endDist );

        for ( int row = 0; row < m_charts.size() - 1; row++ )
        {
            QwtPlot *p = m_charts.at(row)->getPlot();
            if ( p )
                p->axisWidget( axis )->setMinBorderDist( startDist, endDist );
        }

    }
}

void ChartsGroupWidget::onAction_selectTarget_toggled(bool checked)
{
  if (checked)
  {
    QAction *selIntervalAct = m_actionsToolBar->getChartAction(caSelectInterval);
    if (selIntervalAct->isChecked())
      selIntervalAct->setChecked(false);
  }

  foreach (ChartXYWidget *chart, m_charts)
  {
    chart->selectionModel()->setSelectionModeTargetingPoint(checked);
  }
}

void ChartsGroupWidget::onAction_chartSettings_triggered()
{
  QList<QwtPlotCurve *> curves;
  foreach(ChartXYWidget *chart, m_charts)
    curves << chart->curves();

  ChartSettingsDlg dlg(m_settings, curves, this);
  if (dlg.exec() == QDialog::Accepted)
  {    
    setSettings(dlg.getChartSettings());
  }
}

void ChartsGroupWidget::onAction_panelCurveDetails_toggled(bool checked)
{
  m_settings.detailsPanelVisible = checked;

  foreach(ChartXYWidget *cc, m_charts)
    cc->setPanelCurveDetailsVisible(m_settings.detailsPanelVisible);

  alignPanelsDetails();
}
