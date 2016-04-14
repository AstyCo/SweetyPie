#include "chartsgroupwidget.h"
#include "ui_chartsgroupwidget.h"

#include <QQueue>

#include <qwt/qwt_scale_widget.h>
#include <qwt/qwt_plot_layout.h>
#include <qwt/qwt_scale_div.h>

#include <verticalscrollarea.h>

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

  ui->frame_stats->hide();

  ui->doubleSpinBox_SelInterval->setLocale(QLocale::C);

  ui->toolButton_SetDtIntervalSelection->hide();
  ui->toolButton_CancelDtIntervalSelection->hide();
  ui->toolButton_SetSelectionInterval->hide();
  ui->toolButton_CancelSelectionInterval->hide();

  m_intState = iesNone;

  m_curSelTimeIntType = (TimeSpan::TimeIntervalTypes) ui->comboBox_IntervalTimeDim->currentIndex();

  connect(ui->splitter_charts, SIGNAL(splitterMoved(int,int)), SLOT(scaleDivChanged()));
  onAction_panelSelectionInterval_clicked();
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
                  << caSettingsDlg);

  connect(m_actionsToolBar->getChartAction(caScale), SIGNAL(triggered(bool)), SLOT(onAction_autoZoom_clicked()));
  connect(m_actionsToolBar->getChartAction(caGrid), SIGNAL(toggled(bool)), SLOT(onAction_grid_toggled(bool)));
  //connect(m_actionsToolBar->getChartAction(caPaintIntervals), SIGNAL(triggered(bool)), SLOT(setIntervalVisible(bool)));
  connect(m_actionsToolBar->getChartAction(caSelectInterval), SIGNAL(toggled(bool)), SLOT(onAction_selectIntervalByMouse_toggled(bool)));
  connect(m_actionsToolBar->getChartAction(caSelectTarget), SIGNAL(toggled(bool)), SLOT(onAction_selectTarget_toggled(bool)));
  //connect(m_actionsToolBar->getChartAction(caMaxMinLines), SIGNAL(triggered(bool)), SLOT(setMaxMinLeftVisible(bool)));
  connect(m_actionsToolBar->getChartAction(caClear), SIGNAL(triggered(bool)), SLOT(onAction_clearChart_clicked()));
  connect(m_actionsToolBar->getChartAction(caSettingsDlg), SIGNAL(triggered(bool)), SLOT(onAction_chartSettings_triggered()));
  m_actionsToolBar->insertSeparator(m_actionsToolBar->getChartAction(caClear));


  connect(m_actionsToolBar->getChartAction(caDetailsPanel), SIGNAL(toggled(bool)), SLOT(onAction_panelCurveDetails_toggled(bool)));

  m_actionSetPanelSelectInterval = new QAction(QIcon(":/icons/icons/selection_interval_panel.png"), tr("Панель выбора интервала"), m_actionsToolBar);
  m_actionSetPanelSelectInterval->setCheckable(true);
  connect(m_actionSetPanelSelectInterval, SIGNAL(toggled(bool)), SLOT(onAction_panelSelectionInterval_clicked()));
  m_actionsToolBar->addAction(m_actionSetPanelSelectInterval);

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
    ChartWidget *chart = m_charts.at(i);
    chart->updateChartSettings(m_settings);
  }

  m_actionsToolBar->getChartAction(caGrid)->setChecked(m_settings.grid);
  m_actionsToolBar->getChartAction(caDetailsPanel)->setChecked(m_settings.detailsPanelVisible);
}

ChartActionsToolBar *ChartsGroupWidget::getActionsToolBar() const
{
  return m_actionsToolBar;
}


void ChartsGroupWidget::addChart(ChartWidget *chart)
{
  insertChart(m_charts.size(), chart);
}

void ChartsGroupWidget::insertChart(int index, ChartWidget *chart)
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

  chart->setPanelCurveDetailsVisible(m_settings.detailsPanelVisible);
  chart->setChartToolBarVisible(false);  
  ui->splitter_charts->insertWidget(index, chart);
  m_charts.insert(index, chart);

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
  ChartWidget * senderChart = static_cast<ChartWidget *>(QObject::sender());

  foreach (ChartWidget *cc, m_charts)
    if (cc != senderChart)
      cc->selectPointByIndex(idx);

}

void ChartsGroupWidget::onIntervalSelectionStart(QPointF pos)
{
  ChartWidget * senderChart = static_cast<ChartWidget *>(QObject::sender());
  foreach (ChartWidget *cc, m_charts)
    if (cc != senderChart)
      cc->setIntervalSelectionStart(pos);
}

void ChartsGroupWidget::onIntervalSelectionEnd(QPointF pos)
{
  ChartWidget * senderChart = static_cast<ChartWidget *>(QObject::sender());
  UtcDateTime beginDt = senderChart->getIntervalSelectionBeginDt();
  UtcDateTime endDt = senderChart->getIntervalSelectionEndDt();
  updateSelectIntervalPanelDates(beginDt, endDt);
  setSelectionInterval(endDt - beginDt);
  QAction *selIntAct = m_actionsToolBar->getChartAction(caSelectInterval);
  bool b = selIntAct->blockSignals(true);
  selIntAct->setChecked(false);
  selIntAct->blockSignals(b);

  foreach (ChartWidget *cc, m_charts)
    if (cc != senderChart)
      cc->setIntervalSelectionEnd(pos);

  emit intervalSelectionEnded(pos);
}

void ChartsGroupWidget::onTargetingDtSet(UtcDateTime dt)
{
  ChartWidget * senderChart = static_cast<ChartWidget *>(QObject::sender());
  foreach (ChartWidget *cc, m_charts)
    if (cc != senderChart)
      cc->setTargetingPoint(dt);

  QAction *selTargetAct = m_actionsToolBar->getChartAction(caSelectTarget);
  bool b = selTargetAct->blockSignals(true);
  selTargetAct->setChecked(false);
  selTargetAct->blockSignals(b);

  emit targetPointSelected(dt);
}

void ChartsGroupWidget::onScaleChanged(qreal scaleFactor, QPoint anchorPoint)
{
  ChartWidget * senderChart = static_cast<ChartWidget *>(QObject::sender());
  foreach (ChartWidget *cc, m_charts)
    if (cc != senderChart)
      cc->rescale(scaleFactor, anchorPoint);

  scaleDivChanged();
}

void ChartsGroupWidget::onZoomed(const QRectF &rect)
{
  ChartWidget * senderChart = static_cast<ChartWidget *>(QObject::sender());
  foreach (ChartWidget *cc, m_charts)
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
  ChartWidget * senderChart = static_cast<ChartWidget *>(QObject::sender());
  foreach (ChartWidget *cc, m_charts)
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
  foreach(ChartWidget *cc, m_charts)
  {
    int panelWidth = cc->getPanelCurveDetails()->widget()->minimumSizeHint().width();
    if (panelWidth > minWidth)
      minWidth = panelWidth;
  }
  foreach(ChartWidget *cc, m_charts)
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

ChartWidget * ChartsGroupWidget::removeChartAt(int index)
{
  if ((index < 0) || (index >= m_charts.size()))
      return NULL;

  bool chartWithXAxisLabel = (index == (m_charts.size() - 1));

  ChartWidget *chartCtrl = m_charts.takeAt(index);
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

ChartWidget * ChartsGroupWidget::at(int index)
{
  if ((index < 0) || (index >= m_charts.size()))
      return NULL;
  else
    return m_charts.at(index);
}

void ChartsGroupWidget::clearChartsData()
{
  foreach(ChartWidget *cc, m_charts)
    cc->clearChart();
}

void ChartsGroupWidget::onToolButtonZoom_clicked()
{
  QAction * act = static_cast<QAction *>(QObject::sender());
  int chartNum = act->text().toInt();
  zoomChart(chartNum);
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
  foreach(ChartWidget *cc, m_charts)
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
  foreach(ChartWidget *chart, m_charts)
    connectChart(chart);
}

void ChartsGroupWidget::connectChart(ChartWidget *chart)
{
  disconnect(chart, 0, this, 0);

  if (m_syncChartsByAxisX)
  {
    connect(chart, SIGNAL(pointSelected(CurveIndex)), SLOT(onPointSelected(CurveIndex)));
    connect(chart, SIGNAL(intervalSelectionStarted(QPointF)), SLOT(onIntervalSelectionStart(QPointF)));
    connect(chart, SIGNAL(intervalSelectionEnded(QPointF)), SLOT(onIntervalSelectionEnd(QPointF)));
    connect(chart, SIGNAL(targetingDtSet(UtcDateTime)), SLOT(onTargetingDtSet(UtcDateTime)));
    connect(chart, SIGNAL(scaleChanged(qreal,QPoint)), SLOT(onScaleChanged(qreal,QPoint)));
    connect(chart, SIGNAL(zoomed(QRectF)), SLOT(onZoomed(QRectF)));
    connect(chart, SIGNAL(panned(int,int)), SLOT(onChartPanned(int,int)));
  }
}
bool ChartsGroupWidget::panelSelectIntervalVisible() const
{
  return m_panelSelectIntervalVisible;
}

void ChartsGroupWidget::setPanelSelectIntervalVisible(bool vis)
{  
  m_actionSetPanelSelectInterval->setChecked(vis);
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
    ChartWidget *chart = m_charts.at(i);
      chart->getPlot()->enableAxis(QwtPlot::xBottom, (! m_syncChartsByAxisX));
  }

  interconnectCharts();
}

void ChartsGroupWidget::selectIntervalByDates(UtcDateTime beginDt, UtcDateTime endDt)
{
  foreach(ChartWidget *cc, m_charts)
      cc->setIntervalSelectionByDates(beginDt, endDt);

  updateSelectIntervalPanelDates(beginDt, endDt);
}

void ChartsGroupWidget::setTargetingPoint(UtcDateTime dt)
{
  foreach(ChartWidget *cc, m_charts)
      cc->setTargetingPoint(dt);
}

void ChartsGroupWidget::clearTargetingPoint()
{
  foreach(ChartWidget *cc, m_charts)
      cc->clearTargetingPoint();
}

UtcDateTime ChartsGroupWidget::getSelIntervalBeginDt()
{
  return m_curStartDt;
}

UtcDateTime ChartsGroupWidget::getSelIntervalEndDt()
{
  return m_curEndDt;
}


void ChartsGroupWidget::onAction_panelSelectionInterval_clicked()
{  
  m_panelSelectIntervalVisible = m_actionSetPanelSelectInterval->isChecked();
  ui->frame_SelectionPanel->setVisible(m_panelSelectIntervalVisible);
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

  UtcDateTime beginDt = m_charts.first()->minimumDt();
  UtcDateTime endDt = m_charts.first()->maximumDt();

  if ((!m_syncChartsByAxisX) && (m_charts.size() > 1))
  {
    // если в наборе графиков разное количество точек,
    // или разные значения времени, то находим график
    // с минимальным временным интервалом значений
    for(int i = 1; i < m_charts.size(); i++)
    {
      ChartWidget *cc = m_charts.at(i);
      if (cc->minimumDt() > beginDt)
        beginDt = cc->minimumDt();

      if (cc->maximumDt() < endDt)
        endDt = cc->maximumDt();
    }
  }

  this->updateSelectIntervalPanelDates(beginDt, endDt);
  TimeSpan duration = endDt - beginDt;
  this->setSelectionInterval(duration);
  if (m_syncChartsByAxisX || (m_charts.size() == 1))
  {
    if (!m_charts.first()->curves().isEmpty())
    {
      ui->label_stat_points->setText(QString::number(m_charts.first()->curve(0)->dataSize()));
      QString beginEnd("%1 - %2");
      beginEnd = beginEnd.arg(beginDt.toStdString(0), endDt.toStdString(0));
      ui->label_stat_beginDt_endDt->setText(beginEnd);

      ui->label_stat_duration->setText(duration.toString());
    }
  }
}

void ChartsGroupWidget::on_dateTimeEdit_StartSelection_dateTimeChanged(const QDateTime &dateTime)
{
  Q_UNUSED(dateTime)
  ui->toolButton_SetDtIntervalSelection->show();
  ui->toolButton_CancelDtIntervalSelection->show();
  m_intState = iesBegin;
}

void ChartsGroupWidget::on_dateTimeEdit_EndSelection_dateTimeChanged(const QDateTime &dateTime)
{
  Q_UNUSED(dateTime)
  ui->toolButton_SetDtIntervalSelection->show();
  ui->toolButton_CancelDtIntervalSelection->show();
  m_intState = iesEnd;
}

void ChartsGroupWidget::on_doubleSpinBox_SelInterval_valueChanged(double arg1)
{
  Q_UNUSED(arg1)
  ui->toolButton_SetSelectionInterval->show();
  ui->toolButton_CancelSelectionInterval->show();
  m_intState = iesInt;
}

void ChartsGroupWidget::on_toolButton_SetDtIntervalSelection_clicked()
{
  if (m_charts.empty())
    return;

  UtcDateTime startDt(ui->dateTimeEdit_StartSelection->dateTime());
  UtcDateTime endDt(ui->dateTimeEdit_EndSelection->dateTime());

  if (m_syncChartsByAxisX || (m_charts.size() == 1))
  {
    UtcDateTime startLimitDt = m_charts.first()->minimumDt();
    UtcDateTime endLimitDt = m_charts.first()->maximumDt();

    bool updateUi = false;
    if (startDt > endDt)
    {
      UtcDateTime tmpDt(startDt);
      startDt = endDt;
      endDt = tmpDt;
      updateUi = true;
    }
    if (startDt < startLimitDt)
    {
      startDt = startLimitDt;
      updateUi = true;
    }
    if (endDt > endLimitDt)
    {
      endDt = endLimitDt;
      updateUi = true;
    }

    if (updateUi)
      this->updateSelectIntervalPanelDates(startDt, endDt);

    this->setSelectionInterval(endDt - startDt);
  }
  for(int i = 0; i < m_charts.size(); i++)
  {
    ChartWidget *cc = m_charts.at(i);
    cc->setIntervalSelectionByDates(startDt, endDt);
  }

  this->hideEditIntervalButtons();
}

void ChartsGroupWidget::on_toolButton_CancelDtIntervalSelection_clicked()
{
  this->updateSelectIntervalPanelDates(m_curStartDt, m_curEndDt);
  this->hideEditIntervalButtons();
}

void ChartsGroupWidget::on_toolButton_SetSelectionInterval_clicked()
{
  if (m_charts.empty())
    return;

  UtcDateTime startDt(ui->dateTimeEdit_StartSelection->dateTime());
  TimeSpan newInt(ui->doubleSpinBox_SelInterval->value(), m_curSelTimeIntType);
  UtcDateTime endDt = startDt + newInt;

  UtcDateTime endLimitDt = m_charts.first()->maximumDt();

  if (endDt > endLimitDt)
  {
    endDt = endLimitDt;
    newInt = endDt - startDt;
    setSelectionInterval(newInt);
  }
  else
    m_selInterval = newInt;

  for(int i = 0; i < m_charts.size(); i++)
  {
    ChartWidget *cc = m_charts.at(i);
    cc->setIntervalSelectionByDates(startDt, endDt);
  }

  this->hideEditIntervalButtons();
}

void ChartsGroupWidget::on_toolButton_CancelSelectionInterval_clicked()
{
  this->setSelectionInterval(m_selInterval);
  this->hideEditIntervalButtons();
}

void ChartsGroupWidget::on_comboBox_IntervalTimeDim_currentIndexChanged(int index)
{
  m_curSelTimeIntType = (TimeSpan::TimeIntervalTypes)index;

  bool b = ui->doubleSpinBox_SelInterval->blockSignals(true);
  ui->doubleSpinBox_SelInterval->setValue(m_selInterval.totalTime(m_curSelTimeIntType));
  ui->doubleSpinBox_SelInterval->blockSignals(b);
}

void ChartsGroupWidget::setSelectionInterval(TimeSpan selInt)
{
  m_selInterval = selInt;

  bool b = ui->doubleSpinBox_SelInterval->blockSignals(true);
  ui->doubleSpinBox_SelInterval->setValue(selInt.totalTime(m_curSelTimeIntType));
  ui->doubleSpinBox_SelInterval->blockSignals(b);
}

void ChartsGroupWidget::updateSelectIntervalPanelDates(UtcDateTime startDt, UtcDateTime endDt)
{
  m_curStartDt = startDt;
  bool b = ui->dateTimeEdit_StartSelection->blockSignals(true);
  ui->dateTimeEdit_StartSelection->setDateTime(startDt.dateTime());
  ui->dateTimeEdit_StartSelection->blockSignals(b);

  m_curEndDt = endDt;
  b = ui->dateTimeEdit_EndSelection->blockSignals(true);
  ui->dateTimeEdit_EndSelection->setDateTime(endDt.dateTime());
  ui->dateTimeEdit_EndSelection->blockSignals(b);
}

void ChartsGroupWidget::hideEditIntervalButtons()
{
  ui->toolButton_SetDtIntervalSelection->hide();
  ui->toolButton_CancelDtIntervalSelection->hide();

  ui->toolButton_SetSelectionInterval->hide();
  ui->toolButton_CancelSelectionInterval->hide();
  m_intState = iesNone;
}

void ChartsGroupWidget::onAction_autoZoom_clicked()
{
  foreach(ChartWidget *chart, m_charts)
    chart->autoZoom();

  scaleDivChanged();
}

void ChartsGroupWidget::onAction_grid_toggled(bool checked)
{
  m_settings.grid = checked;
  foreach(ChartWidget *chart, m_charts)
    chart->setGrid(checked);
}

void ChartsGroupWidget::onAction_clearChart_clicked()
{
    foreach(ChartWidget *chart, m_charts)
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

  foreach(ChartWidget *cc, m_charts)
      cc->setIntervalSelection(checked);
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

  foreach (ChartWidget *chart, m_charts)
  {
    chart->setSelectionModeTargetingPoint(checked);
  }
}

void ChartsGroupWidget::onAction_chartSettings_triggered()
{
  QList<QwtPlotCurve *> curves;
  foreach(ChartWidget *chart, m_charts)
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

  foreach(ChartWidget *cc, m_charts)
    cc->setPanelCurveDetailsVisible(m_settings.detailsPanelVisible);

  alignPanelsDetails();
}
