#include "chartintervalselector.h"

#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_symbol.h>
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_plot_picker.h>
#include <qwt/qwt_picker_machine.h>

#include "chartxywidget.h"
#include "plotnavigator.h"
#include "plotkeyeventhandler.h"
#include "charttimexywidget.h"

ChartIntervalSelector::ChartIntervalSelector(ChartXYWidget *chart, PlotNavigator *nav)
  : QObject(chart), m_chart(chart), m_plot(chart->getPlot()), m_navigator(nav)
{

  m_selectionState = ssNone;
  m_hasSelection = false;

  m_selIntAct = m_chart->getActionsToolBar()->getChartAction(caSelectInterval);

  m_visible = true;
  _color = Qt::red;
  // создать маркеры, которыми будет обозначена область выделения
  for(int i = 0; i <= 1; i++)
  {
    m_pIntervalMarker[i] = new QwtPlotMarker();
    m_pIntervalMarker[i]->setLineStyle(QwtPlotMarker::VLine);
    m_pIntervalMarker[i]->setLinePen(QPen( _color, 3, Qt::SolidLine ));
    m_pIntervalMarker[i]->hide();
    m_pIntervalMarker[i]->attach(m_plot);
  }

  m_pTargetPointMarker = new QwtPlotMarker();
  m_pTargetPointMarker->setLineStyle(QwtPlotMarker::VLine);
  m_pTargetPointMarker->setLinePen(QPen( _color, 3, Qt::DashLine ));
  m_pTargetPointMarker->hide();
  m_pTargetPointMarker->attach(m_plot);
}

qreal ChartIntervalSelector::begin() const
{
  qreal rez;
  if (m_beginLimit.isValid())
    rez = m_chart->curves()[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint).x();

  return rez;
}

qreal ChartIntervalSelector::end() const
{
  qreal rez;
  if (m_endLimit.isValid())
    rez = m_chart->curves()[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint).x();

  return rez;
}


void ChartIntervalSelector::onAction_SelectTarget_toggled(bool checked)
{
  if (checked)
  {
    if (m_selectionState != ssNone)
      clearIntervalSelection();

    m_selectionState = ssTargetPoint;
  }
  else
    m_selectionState = ssNone;
}

void ChartIntervalSelector::setIntervalSelection(qreal begin, qreal end)
{
  if (! m_beginLimit.isValid() || ! m_endLimit.isValid())
    return;

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


void ChartIntervalSelector::setSelectionModeTargetPoint(bool b)
{  
  if (b)
  {
    clearTargetPoint();

    if (m_selectionState != ssNone)
      clearIntervalSelection();

    m_selectionState = ssTargetPoint;
  }
}

void ChartIntervalSelector::setTargetPoint(qreal value)
{
  if (m_selectionState == ssTargetPoint)
    m_selectionState = ssNone;

  QAction *selTargetAct = m_chart->getActionsToolBar()->getChartAction(caSelectTarget);
  bool b = selTargetAct->blockSignals(true);
  selTargetAct->setChecked(false);
  selTargetAct->blockSignals(b);

  m_targetPoint = value;
  QPointF point(value, 0);

  showTargetPoint(point);

  QList<QwtPlotCurve*> curves = m_chart->curves();
  for(int i = 0; i < curves.size(); i++)
  {
    QwtPlotCurve *curve = curves.at(i);
    ChartCurveStats &stats = m_chart->m_curvesStats[i];
    long pointTargetPointIdx = m_chart->findPointIndexByPos(point, sdRight, i);

    if (pointTargetPointIdx != -1)
      stats.intervalPointingValue = curve->sample(pointTargetPointIdx);
    else
      stats.intervalPointingValue = QPointF();
  }

  m_plot->replot();
}

void ChartIntervalSelector::clearTargetPoint()
{
  if (m_selectionState == ssTargetPoint)
    m_selectionState = ssNone;

  QAction *selTargetAct = m_chart->getActionsToolBar()->getChartAction(caSelectTarget);
  bool b = selTargetAct->blockSignals(true);
  selTargetAct->setChecked(false);
  selTargetAct->blockSignals(b);

  m_pTargetPointMarker->hide();
  for(int i = 0; i < m_chart->m_curves.size(); i++)
    m_chart->m_curvesStats[i].intervalPointingValue = QPointF();

  m_plot->replot();
}

void ChartIntervalSelector::clearIntervalSelection()
{
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

void ChartIntervalSelector::setIntervalSelectionStart(QPointF pos)
{
  m_selectionState = ssIntervalBegin;
  setIntervalSelectionByState(pos);
  m_selectionState = ssIntervalEnd;
  m_navigator->setZoomRectEnabled(false);
}

void ChartIntervalSelector::setIntervalSelectionEnd(QPointF pos)
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





void ChartIntervalSelector::showSelectionInterval(QPointF start, QPointF end)
{
    m_pIntervalMarker[0]->setValue(start);
    m_pIntervalMarker[1]->setValue(end);

    if(!m_visible)
        return;

    m_pIntervalMarker[0]->show();
    m_pIntervalMarker[1]->show();
    m_plot->replot();
}
void ChartIntervalSelector::showSelectionIntervalStart(QPointF start)
{
  m_pIntervalMarker[0]->setValue(start);

  if(!m_visible)
        return;

  m_pIntervalMarker[0]->show();
  m_plot->replot();
}

void ChartIntervalSelector::showSelectionIntervalEnd(QPointF end)
{
  m_pIntervalMarker[1]->setValue(end);

  if(!m_visible)
        return;

  m_pIntervalMarker[1]->show();
  m_plot->replot();
}

void ChartIntervalSelector::showTargetPoint(QPointF point)
{
    m_pTargetPointMarker->setValue(point);

    if(!m_visible)
        return;

    m_pTargetPointMarker->show();
    m_plot->replot();
}


void ChartIntervalSelector::onAction_SelectInterval_toggled(bool b)
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


void ChartIntervalSelector::setIntervalSelectionByState(QPointF pos)
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
    m_intervalBegin = m_chart->curves()[m_selectionBeginPointIdx.indexCurve]->
        sample(m_selectionBeginPointIdx.indexPoint).x();
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



void ChartIntervalSelector::clearAllSelections()
{
  // скрыть маркеры
  m_pIntervalMarker[0]->hide();
  m_pIntervalMarker[1]->hide();
  m_pTargetPointMarker->hide();
}

void ChartIntervalSelector::reset()
{
  m_beginLimit = CurveIndex();
  m_endLimit = CurveIndex();

  m_selectionState = ssNone;
  m_hasSelection = false;
  m_selectionBeginPointIdx = CurveIndex();
  m_selectionEndPointIdx = CurveIndex();

}

void ChartIntervalSelector::onCurveAdded(QwtPlotCurve *curve)
{
  if (curve->dataSize() > 0)
  {
    if(! m_beginLimit.isValid())
    {
      m_beginLimit.indexCurve = m_chart->m_curves.size()-1;
      m_beginLimit.indexPoint = 0;
    }
    else if(curve->sample(0).x() < m_chart->m_curves[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint).x())
    {
      m_beginLimit.indexCurve = m_chart->m_curves.size()-1;
      m_beginLimit.indexPoint = 0;
    }

    if (! m_endLimit.isValid())
    {
      m_endLimit.indexCurve = m_chart->m_curves.size()-1;
      m_endLimit.indexPoint = curve->dataSize() - 1;
    }
    else if(curve->sample(curve->dataSize() - 1).x() > m_chart->m_curves[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint).x())
    {
      m_endLimit.indexCurve = m_chart->m_curves.size()-1;
      m_endLimit.indexPoint = curve->dataSize() - 1;
    }
  }
}

void ChartIntervalSelector::onCurvePointSelected(const QPointF &pos)
{
  if (m_selectionState == ssIntervalBegin)
  {
    setIntervalSelectionStart(pos);

    emit intervalSelectionStarted(pos);
  }
  else if (m_selectionState == ssIntervalEnd)
  {
    setIntervalSelectionEnd(pos);

    emit intervalSelectionEnded(pos);
  }
  else if (m_selectionState == ssTargetPoint)
  {
    setTargetPoint(pos.x());

    emit targetPointSet(pos.x());
  }
}

void ChartIntervalSelector::setZTop()
{
    m_pIntervalMarker[0]->setZ(100);
    m_pIntervalMarker[1]->setZ(100);
    m_pTargetPointMarker->setZ(100);
    m_plot->replot();
}

void ChartIntervalSelector::setZBottom()
{
    m_pIntervalMarker[0]->setZ(0);
    m_pIntervalMarker[1]->setZ(0);
    m_pTargetPointMarker->setZ(0);
    m_plot->replot();
}

void ChartIntervalSelector::updateCurvesIntervalStats()
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

bool ChartIntervalSelector::visible() const
{
    return m_visible;
}

QColor ChartIntervalSelector::color() const
{
    return _color;
}

void ChartIntervalSelector::setColor(const QColor &color)
{
    _color = color;
    for(int i = 0; i <= 1; i++)
    {
        m_pIntervalMarker[i]->setLinePen(QPen( _color, 3, Qt::SolidLine ));
    }

    m_pTargetPointMarker->setLinePen(QPen( _color, 3, Qt::DashLine ));
    m_plot->replot();
}

void ChartIntervalSelector::setVisible(bool b)
{
    m_visible = b;
    m_pIntervalMarker[0]->setVisible(b);
    m_pIntervalMarker[1]->setVisible(b);
    m_pTargetPointMarker->setVisible(b);
    m_plot->replot();
}


ChartTimeIntervalSelector::ChartTimeIntervalSelector(ChartXYWidget *chart, PlotNavigator *nav)
  : ChartIntervalSelector(chart, nav)
{
  connect(this, SIGNAL(targetPointSet(qreal)), SLOT(ontargetPointSet(qreal)));
  connect(this, SIGNAL(intervalSelectionEnded(QPointF)), this, SLOT(onIntervalSelectionFinish()));
}

UtcDateTime ChartTimeIntervalSelector::getIntervalSelectionBeginDt()
{
  return ChartTimeXYWidget::pointToDt(QPointF(m_intervalBegin, 0));
}

UtcDateTime ChartTimeIntervalSelector::getIntervalSelectionEndDt()
{
  return ChartTimeXYWidget::pointToDt(QPointF(m_intervalEnd, 0));
}

void ChartTimeIntervalSelector::setIntervalSelectionByDates(UtcDateTime beginDt, UtcDateTime endDt)
{
  if (beginDt > endDt)
  {
    UtcDateTime tmpDt(beginDt);
    beginDt = endDt;
    endDt = tmpDt;
  }
  if(!m_beginLimit.isValid() || !m_endLimit.isValid())
    return;

  QPointF startP = ChartTimeXYWidget::dtToPoint(beginDt);
  QPointF endP = ChartTimeXYWidget::dtToPoint(endDt);

  setIntervalSelection(startP.x(), endP.x());
}

void ChartTimeIntervalSelector::setTargetPointByDates(UtcDateTime dt)
{
    setTargetPoint(ChartTimeXYWidget::dtToPoint(dt).x());
}

void ChartTimeIntervalSelector::ontargetPointSet(qreal point)
{
//    if(!m_beginLimit.isValid() || !m_endLimit.isValid())
//        return;

//    QPointF p = ChartTimeXYWidget::dtToPoint(dt);
//    setTargetPoint(p.x());

    emit targetPointDtSet(ChartTimeXYWidget::pointToDt(QPointF(point, 0)));
}


void ChartTimeIntervalSelector::onIntervalSelectionFinish()
{
    emit intervalSelectionFinished(ChartTimeXYWidget::pointToDt(QPointF(intervalSelectionBegin(),0)),
                                   ChartTimeXYWidget::pointToDt(QPointF(intervalSelectionEnd(),0)));
}

UtcDateTime ChartTimeIntervalSelector::minimumDt()
{
  UtcDateTime rez;
  if (m_beginLimit.isValid())
    rez = ChartTimeXYWidget::pointToDt(m_chart->curves()[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint));

  return rez;
}

UtcDateTime ChartTimeIntervalSelector::maximumDt()
{
  UtcDateTime rez;
  if (m_endLimit.isValid())
    rez = ChartTimeXYWidget::pointToDt(m_chart->curves()[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint));

  return rez;
}
