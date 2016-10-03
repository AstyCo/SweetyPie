#include "chartintervalselectionmodel.h"

#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_symbol.h>

#include "chartxywidget.h"
#include "plotnavigator.h"
#include "plotkeyeventhandler.h"

QPointF dtToPoint(const UtcDateTime &dt, double y = 0)
{
  return QPointF(double(dt.toMicrosecondsSinceEpoch() / 1000), y);
}

UtcDateTime pointToDt(const QPointF &p)
{
  return UtcDateTime::fromMicrosecondsSinceEpoch((long long)p.x() * 1000);
}


ChartIntervalSelectionModel::ChartIntervalSelectionModel(ChartXYWidget *chart, PlotNavigator *nav)
  : QObject(chart), m_chart(chart), m_plot(chart->getPlot()), m_navigator(nav)
{
  m_selectedPointIndex = CurveIndex();
  m_selectionState = ssNone;
  m_hasSelection = false;

  m_keyEventHandler = new PlotKeyEventHandler(m_plot);
  m_plot->installEventFilter(m_keyEventHandler);

  connect(m_chart->m_picker, SIGNAL(selected(const QPointF&)), this, SLOT(onCurvePointSelected(const QPointF&)));
  connect(m_keyEventHandler, SIGNAL(nextPointSelected(bool)), SLOT(onNextCurvePointSelected(bool)));

  m_selIntAct = m_chart->getActionsToolBar()->getChartAction(caSelectInterval);
  // создать маркеры, которыми будем помечать выбранную точку
  m_pMarker[0] = new QwtPlotMarker();
  m_pMarker[0]->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
  QwtSymbol * sym = new QwtSymbol(QwtSymbol::Diamond, QBrush(Qt::magenta, Qt::SolidPattern),
                                  QPen(Qt::black, 0.0, Qt::SolidLine), QSize(7, 7));
  m_pMarker[0]->setSymbol(sym);
  m_pMarker[0]->setLineStyle(QwtPlotMarker::VLine);
  m_pMarker[0]->setLinePen(QPen( Qt::black, 0, Qt::DashLine ));
  m_pMarker[0]->hide();
  m_pMarker[0]->attach(m_plot);

  m_pMarker[1] = new QwtPlotMarker();
  m_pMarker[1]->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
  m_pMarker[1]->setLineStyle(QwtPlotMarker::HLine);
  m_pMarker[1]->setLinePen(QPen( Qt::black, 0, Qt::DashLine ));
  m_pMarker[1]->hide();
  m_pMarker[1]->attach(m_plot);

  // создать маркеры, которыми будет обозначена область выделения
  for(int i = 0; i <= 1; i++)
  {
    m_pIntervalMarker[i] = new QwtPlotMarker();
    m_pIntervalMarker[i]->setLineStyle(QwtPlotMarker::VLine);
    m_pIntervalMarker[i]->setLinePen(QPen( Qt::red, 3, Qt::SolidLine ));
    m_pIntervalMarker[i]->hide();
    m_pIntervalMarker[i]->attach(m_plot);
  }

  m_pTargetingMarker = new QwtPlotMarker();
  m_pTargetingMarker->setLineStyle(QwtPlotMarker::VLine);
  m_pTargetingMarker->setLinePen(QPen( Qt::magenta, 3, Qt::SolidLine ));
  m_pTargetingMarker->hide();
  //m_pTargetingMarker->setZ(100);
  m_pTargetingMarker->attach(m_plot);

  connect(m_chart->m_actionsToolBar->getChartAction(caSelectInterval), SIGNAL(toggled(bool)), SLOT(setIntervalSelection(bool)));
  connect(m_chart->m_actionsToolBar->getChartAction(caSelectTarget), SIGNAL(toggled(bool)), SLOT(onAction_SelectTarget_toggled(bool)));
}


void ChartIntervalSelectionModel::onAction_SelectTarget_toggled(bool checked)
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

UtcDateTime ChartTimeIntervalSelectionModel::minimumDt()
{
  UtcDateTime rez;
  if (m_beginLimit.isValid())
    rez = pointToDt(m_chart->curves()[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint));

  return rez;
}

UtcDateTime ChartTimeIntervalSelectionModel::maximumDt()
{
  UtcDateTime rez;
  if (m_endLimit.isValid())
    rez = pointToDt(m_chart->curves()[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint));

  return rez;
}


void ChartIntervalSelectionModel::setIntervalSelection(qreal begin, qreal end)
{
  QPointF startP(begin, 0);
  QPointF endP(end, 0);
  QPointF startLimitP = m_chart->curves()[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint);
  QPointF endLimitP = m_chart->curves()[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint);

  if (startP.x() < startLimitP.x())
    startP = startLimitP;

  if (endP.x() > endLimitP.x())
    endP = endLimitP;

  m_selectionBeginPointIdx = m_chart->findClosestPointAllCurves(startP, sdRight);
  m_selectionEndPointIdx = m_chart->findClosestPointAllCurves(endP, sdLeft);

  m_hasSelection = true;

  showSelectionInterval(startP, endP);
  updateCurvesIntervalStats();

  for(int i = 0; i < m_chart->m_panelCurveDetailsList.count(); i++)
  {
    CurveDetailsGroupBox *details = m_chart->m_panelCurveDetailsList.at(i);

    ChartCurveStats stats = m_chart->m_curvesStats.at(i);
    details->setInterval(stats.intervalBeginValue.x(), stats.intervalEndValue.x());

    details->setIntervalLabelsVisible(m_hasSelection);
  }
}


void ChartIntervalSelectionModel::setSelectionModeTargetingPoint(bool b)
{
  clearTargetingPoint();

  if (b)
  {
    if (m_selectionState != ssNone)
      clearIntervalSelection();

    m_selectionState = ssTargetingPoint;
  }
}

void ChartIntervalSelectionModel::setTargetingPoint(UtcDateTime dt)
{
  if (m_selectionState == ssTargetingPoint)
    m_selectionState = ssNone;

  QAction *selTargetAct = m_chart->getActionsToolBar()->getChartAction(caSelectTarget);
  bool b = selTargetAct->blockSignals(true);
  selTargetAct->setChecked(false);
  selTargetAct->blockSignals(b);

  QPointF point = dtToPoint(dt);
  m_targetingPoint = point.x();
  if (!dt.isValid())
  {
    clearTargetingPoint();
    return;
  }
  m_pTargetingMarker->setValue(point);
  m_pTargetingMarker->show();

  QList<QwtPlotCurve*> curves = m_chart->curves();
  for(int i = 0; i < curves.size(); i++)
  {
    QwtPlotCurve *curve = curves.at(i);
    ChartCurveStats &stats = m_chart->m_curvesStats[i];
    long pointTargetingIdx = m_chart->findPointIndexByPos(point, sdRight, i);

    if (pointTargetingIdx != -1)
      stats.intervalPointingValue = curve->sample(pointTargetingIdx);
    else
      stats.intervalPointingValue = QPointF();
  }

  m_plot->replot();
}

void ChartIntervalSelectionModel::clearTargetingPoint()
{
  if (m_selectionState == ssTargetingPoint)
    m_selectionState = ssNone;

  QAction *selTargetAct = m_chart->getActionsToolBar()->getChartAction(caSelectTarget);
  bool b = selTargetAct->blockSignals(true);
  selTargetAct->setChecked(false);
  selTargetAct->blockSignals(b);

  m_pTargetingMarker->hide();
  for(int i = 0; i < m_chart->m_curves.size(); i++)
    m_chart->m_curvesStats[i].intervalPointingValue = QPointF();

  m_plot->replot();
}

void ChartIntervalSelectionModel::clearIntervalSelection()
{
  /*
  for(int i = 0; i < m_chart->curves().size(); i++)
  {
    ChartCurveStats &stats = m_curvesStats[i];
    stats.intervalBeginValue = QPointF();
    stats.intervalEndValue = QPointF();
  }
  */
  m_selectionState = ssNone;
  m_hasSelection = false;
  foreach(CurveDetailsGroupBox *panel, m_chart->detailedPanels())
    panel->setIntervalLabelsVisible(false);

  m_intervalBegin = 0;
  m_intervalEnd = 0;

  m_pIntervalMarker[0]->hide();
  m_pIntervalMarker[1]->hide();
  m_plot->replot();
}

void ChartIntervalSelectionModel::setIntervalSelectionStart(QPointF pos)
{
  m_selectionState = ssIntervalBegin;
  setIntervalSelectionByState(pos);
  m_selectionState = ssIntervalEnd;
  m_navigator->setZoomRectEnabled(false);
}

void ChartIntervalSelectionModel::setIntervalSelectionEnd(QPointF pos)
{
  m_selectionState = ssIntervalEnd;
  m_hasSelection = true;
  setIntervalSelectionByState(pos);
  m_selectionState = ssNone;
  m_navigator->setZoomRectEnabled(true);

  bool b = m_selIntAct->blockSignals(true);
  m_selIntAct->setChecked(false);
  m_selIntAct->blockSignals(b);
  updateCurvesIntervalStats();


  for(int i = 0; i < m_chart->m_panelCurveDetailsList.count(); i++)
  {
    CurveDetailsGroupBox *details = m_chart->m_panelCurveDetailsList.at(i);

    ChartCurveStats stats = m_chart->m_curvesStats.at(i);
    details->setInterval(stats.intervalBeginValue.x(), stats.intervalEndValue.x());

    details->setIntervalLabelsVisible(m_hasSelection);
  }
}

void ChartIntervalSelectionModel::selectPointByIndex(CurveIndex idx)
{
  QList<QwtPlotCurve *> curves = m_chart->curves();
  if ((idx.indexCurve < 0) || (idx.indexCurve > curves.size() - 1))
    return;

  if ((idx.indexPoint >= 0) && (idx.indexPoint < curves.at(idx.indexCurve)->dataSize()))
  {
    m_selectedPointIndex = idx;
    updateDetailsPanelsSelPoint();
    drawMarkerOnCurve((QwtPlot::Axis)curves[m_selectedPointIndex.indexCurve]->yAxis());
  }
}

void ChartIntervalSelectionModel::showSelectionPoint(QwtText xLbl, QwtText yLbl, QPointF point, QwtPlot::Axis axis)
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
  m_plot->replot();
}

void ChartIntervalSelectionModel::showSelectionInterval(QPointF start, QPointF end)
{
  m_pIntervalMarker[0]->setValue(start);
  m_pIntervalMarker[0]->show();
  m_pIntervalMarker[1]->setValue(end);
  m_pIntervalMarker[1]->show();
  m_plot->replot();
}
void ChartIntervalSelectionModel::showSelectionIntervalStart(QPointF start)
{
  m_pIntervalMarker[0]->setValue(start);
  m_pIntervalMarker[0]->show();
  m_plot->replot();
}

void ChartIntervalSelectionModel::showSelectionIntervalEnd(QPointF end)
{
  m_pIntervalMarker[1]->setValue(end);
  m_pIntervalMarker[1]->show();
  m_plot->replot();
}


void ChartIntervalSelectionModel::setIntervalSelection(bool b)
{
  if(b)
  {
    clearIntervalSelection();
    m_selectionState = ssIntervalBegin;
    m_navigator->setZoomRectEnabled(false);
  }
  else
  {
    if (m_selectionState != ssNone)
      clearIntervalSelection();

    m_navigator->setZoomRectEnabled(true);
  }
}


void ChartIntervalSelectionModel::setIntervalSelectionByState(QPointF pos)
{
  QList<QwtPlotCurve *> curves = m_chart->curves();
  if(curves.isEmpty())
    return;

  QPointF p = pos;
  CurveIndex selPointIdx;

  //this->SetSelectionIntervalDates(selDt, m_curEndDt);
  int markerIdx = 0;
  if (m_selectionState == ssIntervalBegin)
  {
    selPointIdx = m_chart->findClosestPointAllCurves(p, sdRight);
    m_selectionBeginPointIdx = selPointIdx;
    m_intervalBegin = m_chart->curves()[m_selectedPointIndex.indexCurve]->
        sample(m_selectedPointIndex.indexPoint).x();
  }
  else if (m_selectionState == ssIntervalEnd)
  {
    if (m_intervalBegin > p.x())
    {
      // если конечная точка на самом деле является началом
      // интервала, то меняем местами
      m_intervalEnd = m_intervalBegin;
      m_intervalBegin = p.x();

      // текущие маркеры начала заменяем на маркеры окончания
      m_pIntervalMarker[1]->setValue(m_pIntervalMarker[0]->value());
      m_pIntervalMarker[1]->show();
      m_plot->replot();

      markerIdx = 0;
      m_selectionBeginPointIdx = m_chart->findClosestPointAllCurves(p, sdRight);
      QPointF tmpP(m_intervalEnd, p.y());
      m_selectionEndPointIdx = m_chart->findClosestPointAllCurves(tmpP, sdLeft);
    }
    else
    {
      m_intervalEnd = p.x();
      selPointIdx = m_chart->findClosestPointAllCurves(p, sdLeft);
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

  m_plot->replot();
}

void ChartIntervalSelectionModel::clearSelectedPoint()
{
  m_pMarker[0]->hide();
  m_pMarker[1]->hide();
}

void ChartIntervalSelectionModel::clearAllSelections()
{
  // скрыть маркеры
  clearSelectedPoint();
  m_pIntervalMarker[0]->hide();
  m_pIntervalMarker[1]->hide();
  m_pTargetingMarker->hide();
}

void ChartIntervalSelectionModel::reset()
{
  m_beginLimit = CurveIndex();
  m_endLimit = CurveIndex();

  m_selectedPointIndex = CurveIndex();
  m_selectionState = ssNone;
  m_hasSelection = false;
  m_selectionBeginPointIdx = CurveIndex();
  m_selectionEndPointIdx = CurveIndex();

}

void ChartIntervalSelectionModel::onCurveAdded(QwtPlotCurve *curve)
{
  if (curve->dataSize() > 0)
  {
    if(! m_beginLimit.isValid())
    {
      m_beginLimit.indexCurve = m_chart->m_curves.size();
      m_beginLimit.indexPoint = 0;
    }
    else if(curve->sample(0).x() < m_chart->m_curves[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint).x())
    {
      m_beginLimit.indexCurve = m_chart->m_curves.size();
      m_beginLimit.indexPoint = 0;
    }

    if (! m_endLimit.isValid())
    {
      m_endLimit.indexCurve = m_chart->m_curves.size();
      m_endLimit.indexPoint = curve->dataSize() - 1;
    }
    else if(curve->sample(curve->dataSize() - 1).x() > m_chart->m_curves[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint).x())
    {
      m_endLimit.indexCurve = m_chart->m_curves.size();
      m_endLimit.indexPoint = curve->dataSize() - 1;
    }
  }
}

void ChartIntervalSelectionModel::onCurvePointSelected(const QPointF &pos)
{
  QList<QwtPlotCurve *> curves = m_chart->curves();
  if (m_selectionState == ssNone)
  {

    // найти и выделить точку с учетом выделенного интервала
    CurveIndex selPointIdx = m_chart->findClosestPointAllCurves(pos);

    if(!selPointIdx.isValid())
      return;

    if ((curves.size() > 1) && (m_selectedPointIndex.indexCurve != selPointIdx.indexCurve))
    {
      if (m_selectedPointIndex.isValid())
      {
        QwtPlotCurve *focusCurve = curves[m_selectedPointIndex.indexCurve];
        QPen focusCurvePen = focusCurve->pen();
        focusCurvePen.setWidth(focusCurvePen.width() - 1);
        focusCurve->setPen(focusCurvePen);
      }

      QwtPlotCurve *newFocusCurve = curves[selPointIdx.indexCurve];
      QPen newFocusCurvePen = newFocusCurve->pen();
      newFocusCurvePen.setWidth(newFocusCurvePen.width() + 1);
      newFocusCurve->setPen(newFocusCurvePen);
    }

    m_selectedPointIndex = selPointIdx;

    drawMarkerOnCurve((QwtPlot::Axis)curves[m_selectedPointIndex.indexCurve]->yAxis());
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


void ChartIntervalSelectionModel::drawMarkerOnCurve(QwtPlot::Axis axis)
{
  if (!m_selectedPointIndex.isValid())
    return;

  QwtText txtX, txtY;

  QPointF p = m_chart->curves()[m_selectedPointIndex.indexCurve]->sample(m_selectedPointIndex.indexPoint);
  QString valY = QString::number(p.y(), 'f', 6);

  UtcDateTime refDt = pointToDt(p);
  QString refDtStr = refDt.toStdString(3);

  txtX = tr("[%1]").arg(refDtStr);

  txtY = QString("[%1]").arg(valY);

  showSelectionPoint(txtX, txtY, p, axis);

}

void ChartIntervalSelectionModel::onCurveVisibilityChanged(int curveIdx, bool b)
{
  if(!b)
  {
    if (m_selectedPointIndex.isValid())
    {
      if (curveIdx == m_selectedPointIndex.indexCurve)
        clearSelectedPoint();
    }
  }
}

void ChartIntervalSelectionModel::onNextCurvePointSelected(bool prev)
{
  if (! m_selectedPointIndex.isValid())
    return;

  QwtPlotCurve *curve = m_chart->m_curves.at(m_selectedPointIndex.indexCurve);
  QwtSeriesData<QPointF> *samples = curve->data();

  if (samples->size() == 0)
    return;

  //int step = m_settings.keyStep;
  int step = 1;

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
  m_chart->m_panelCurveDetailsList[m_selectedPointIndex.indexCurve]
      ->setCurrentSelPointIndex(m_selectedPointIndex.indexPoint);

  emit pointSelected(m_selectedPointIndex);
}


void ChartIntervalSelectionModel::updateDetailsPanelsSelPoint()
{
  QList<QwtPlotCurve*> curves = m_chart->curves();
  for(int i = 0; i < m_chart->detailedPanels().count(); i++)
  {
    CurveDetailsGroupBox *details = m_chart->detailedPanels().at(i);
    if(m_selectedPointIndex.isValid()
      && (curves[m_selectedPointIndex.indexCurve] == details->curve()))
    {
      details->setCurrentSelPointIndex(m_selectedPointIndex.indexPoint);
    }
    else
      details->setCurrentSelPointIndex(-1);
  }
}



void ChartIntervalSelectionModel::updateCurvesIntervalStats()
{
  QPointF posBegin = m_pIntervalMarker[0]->value();
  QPointF posEnd = m_pIntervalMarker[1]->value();

  for(int i = 0; i < m_chart->m_curves.size(); i++)
  {
    QwtPlotCurve *curve = m_chart->m_curves.at(i);
    ChartCurveStats &stats = m_chart->m_curvesStats[i];
    if (m_hasSelection)
    {
      long pointBeginIdx = m_chart->findPointIndexByPos(posBegin, sdRight, i);
      long pointEndIdx = m_chart->findPointIndexByPos(posEnd, sdLeft, i);
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

ChartTimeIntervalSelectionModel::ChartTimeIntervalSelectionModel(ChartXYWidget *chart, PlotNavigator *nav)
  : ChartIntervalSelectionModel(chart, nav)
{

}

UtcDateTime ChartTimeIntervalSelectionModel::getIntervalSelectionBeginDt()
{
  return pointToDt(QPointF(m_intervalBegin, 0));
}

UtcDateTime ChartTimeIntervalSelectionModel::getIntervalSelectionEndDt()
{
  return pointToDt(QPointF(m_intervalEnd, 0));
}

void ChartTimeIntervalSelectionModel::setIntervalSelectionByDates(UtcDateTime beginDt, UtcDateTime endDt)
{
  if (beginDt > endDt)
  {
    UtcDateTime tmpDt(beginDt);
    beginDt = endDt;
    endDt = tmpDt;
  }
  if(!m_beginLimit.isValid() || !m_endLimit.isValid())
    return;

  QPointF startP = dtToPoint(beginDt);
  QPointF endP = dtToPoint(endDt);

  setIntervalSelection(startP.x(), endP.x());
}
