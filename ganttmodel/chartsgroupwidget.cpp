#include "chartsgroupwidget.h"
#include "ui_chartsgroupwidget.h"

#include "qwt_scale_widget.h"
#include "qwt_plot_layout.h"

ChartsGroupWidget::ChartsGroupWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ChartsGroupWidget)
{
  ui->setupUi(this);

  //m_settingsChanged = false;

  m_zoomActive = 0;
  m_syncChartsByAxisX = true;
  m_detailsPanelVisible = false;
  m_frameStatsEnabled = false;
  setSelectIntervalPanelEnabled(false);

  ui->frame_stats->hide();
  ui->toolButton_details->setIcon(QIcon(":/icons/icons/back.png"));

  ui->doubleSpinBox_SelInterval->setLocale(QLocale::C);

  ui->toolButton_SetDtIntervalSelection->hide();
  ui->toolButton_CancelDtIntervalSelection->hide();
  ui->toolButton_SetSelectionInterval->hide();
  ui->toolButton_CancelSelectionInterval->hide();

  m_intState = iesNone;

  m_curSelTimeIntType = (TimeSpan::TimeIntervalTypes) ui->comboBox_IntervalTimeDim->currentIndex();

  connect(ui->splitter_charts, SIGNAL(splitterMoved(int,int)), SLOT(scaleDivChanged()));
  on_toolButton_SelectionPanel_clicked();
}

ChartsGroupWidget::~ChartsGroupWidget()
{ 
  delete ui;
}

void ChartsGroupWidget::addChart(ChartWidget *chart)
{
  insertChart(m_charts.size(), chart);
}

void ChartsGroupWidget::insertChart(int index, ChartWidget *chart)
{
  QToolButton * zoomButton = new QToolButton();
  zoomButton->setCheckable(true);
  zoomButton->setFixedSize(30, 30);
  int chartNum = m_charts.size() + 1;
  zoomButton->setText(QString::number(chartNum));
  connect(zoomButton, SIGNAL(clicked()), SLOT(onToolButton_Zoom_clicked()));
  m_chartZoomButtons << zoomButton;
  ui->verticalLayout_zoomButtons->addWidget(zoomButton);

  if ((chartNum > 1) && (m_syncChartsByAxisX))
  {
    if (index >= m_charts.size())
      m_charts.last()->getPlot()->enableAxis(QwtPlot::xBottom, false);
    else
      chart->getPlot()->enableAxis(QwtPlot::xBottom, false);
  }

  connectChart(chart);

  //chart->SetIntervalFieldsVisible(ui->frame_SelectionPanel->isVisible());
  chart->setDetailsPaneVisible(m_detailsPanelVisible);

  ui->splitter_charts->insertWidget(index, chart);
  m_charts.insert(index, chart);

  updateSelectionPanel();
}

bool ChartsGroupWidget::frameStatsEnabled() const
{
  return m_frameStatsEnabled;
}

void ChartsGroupWidget::setFrameStatsEnabled(bool frameStatsEnabled)
{
  m_frameStatsEnabled = frameStatsEnabled;
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
  UtcDateTime beginDt = senderChart->getSelIntervalBeginDt();
  UtcDateTime endDt = senderChart->getSelIntervalEndDt();
  updateSelectIntervalPanelDates(beginDt, endDt);
  setSelectionInterval(endDt - beginDt);
  bool b = ui->toolButton_SelectIntervalByMouse->blockSignals(true);
  ui->toolButton_SelectIntervalByMouse->setChecked(false);
  ui->toolButton_SelectIntervalByMouse->blockSignals(b);

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

  bool b = ui->toolButton_SelectTarget->blockSignals(true);
  ui->toolButton_SelectTarget->setChecked(false);
  ui->toolButton_SelectTarget->blockSignals(b);

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

void ChartsGroupWidget::resizeEvent(QResizeEvent *ev)
{
  QWidget::resizeEvent(ev);
  scaleDivChanged();
}

ChartWidget * ChartsGroupWidget::removeChartAt(int index)
{
  if ((index < 0) || (index >= m_charts.size()))
      return NULL;

  bool chartWithXAxisLabel = (index == (m_charts.size() - 1));

  ChartWidget *chartCtrl = m_charts.takeAt(index);
  QToolButton * zoomButton = m_chartZoomButtons.takeLast();

  if ((! m_charts.isEmpty()) && (m_syncChartsByAxisX))
  {
    if (chartWithXAxisLabel)
      m_charts.last()->getPlot()->enableAxis(QwtPlot::xBottom, true);
  }

  chartCtrl->setParent(NULL);
  ui->verticalLayout_zoomButtons->removeWidget(zoomButton);
  delete zoomButton;

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

void ChartsGroupWidget::onToolButton_Zoom_clicked()
{
  QToolButton * btn = static_cast<QToolButton *>(QObject::sender());
  int chartNum = btn->text().toInt();
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

      m_chartZoomButtons.at(idx)->setChecked(false);
      sizes << base.height() / m_charts.size();
    }
    else
    {
      if ((idx + 1) == newZoom)
      {
        visible = true;
        sizes << base.height();
      }
      else
      {
        visible = false;
        m_chartZoomButtons.at(idx)->setChecked(false);
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
    connect(chart, SIGNAL(panned(int,int)), SLOT(onChartPanned(int,int)));
  }
}
bool ChartsGroupWidget::getSelectIntervalPanelEnabled() const
{
  return m_selectIntervalPanelEnabled;
}

void ChartsGroupWidget::setSelectIntervalPanelEnabled(bool selectIntervalPanelEnabled)
{
  m_selectIntervalPanelEnabled = selectIntervalPanelEnabled;
  if (!selectIntervalPanelEnabled)
    ui->frame_SelectionPanel->hide();

  ui->toolButton_SelectionPanel->setVisible(selectIntervalPanelEnabled);
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
      cc->selectIntervalByDates(beginDt, endDt);

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


void ChartsGroupWidget::on_toolButton_details_clicked()
{
  m_detailsPanelVisible = ! m_detailsPanelVisible;
  ui->toolButton_details->setIcon(QIcon(m_detailsPanelVisible
                                        ? ":/icons/icons/forward_arrow.png"
                                        : ":/icons/icons/back.png"));

  foreach(ChartWidget *cc, m_charts)
    cc->setDetailsPaneVisible(m_detailsPanelVisible);
}

void ChartsGroupWidget::on_toolButton_SelectionPanel_clicked()
{
  bool vis = ui->toolButton_SelectionPanel->isChecked();
  ui->frame_SelectionPanel->setVisible(vis);
  /*
  on_toolButton_EndSelectionByMouse_clicked();
  foreach(ChartWidget *cc, m_chartControls)
  {
    cc->SetIntervalFieldsVisible(vis);
  }
  */
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
    cc->selectIntervalByDates(startDt, endDt);
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
    cc->selectIntervalByDates(startDt, endDt);
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

void ChartsGroupWidget::on_toolButton_AutoZoom_clicked()
{
  foreach(ChartWidget *chart, m_charts)
    chart->autoZoom();
}

void ChartsGroupWidget::on_toolButton_Grid_toggled(bool checked)
{
  foreach(ChartWidget *chart, m_charts)
    chart->setGrid(checked);
}

void ChartsGroupWidget::on_toolButton_Clear_clicked()
{
    foreach(ChartWidget *chart, m_charts)
      chart->fullReplot();
}

void ChartsGroupWidget::on_toolButton_SelectIntervalByMouse_toggled(bool checked)
{
  if (checked)
  {
    if (ui->toolButton_SelectTarget->isChecked())
      ui->toolButton_SelectTarget->setChecked(false);
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

void ChartsGroupWidget::on_toolButton_SelectTarget_toggled(bool checked)
{
  if (checked)
  {
    if (ui->toolButton_SelectIntervalByMouse->isChecked())
      ui->toolButton_SelectIntervalByMouse->setChecked(false);
  }

  foreach (ChartWidget *chart, m_charts)
  {
    chart->setTargetingPointSelection(checked);
  }
}
