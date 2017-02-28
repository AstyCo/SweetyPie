#include "chartpointselector.h"

#include <qwt_symbol.h>

ChartPointSelector::ChartPointSelector(ChartXYWidget *chart, PlotNavigator *nav)
    : QObject(chart), m_chart(chart), m_plot(chart->getPlot()), m_navigator(nav)
{
    m_selectedPointIndex = CurveIndex();

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
}

void ChartPointSelector::clearSelectedPoint()
{
    m_pMarker[0]->hide();
    m_pMarker[1]->hide();
}

void ChartPointSelector::selectPointByIndex(CurveIndex idx)
{
    QList<QwtPlotCurve *> curves = m_chart->curves();
    if ((idx.indexCurve < 0) || (idx.indexCurve > curves.size() - 1))
        return;

    if ((idx.indexPoint >= 0) && (idx.indexPoint < curves.at(idx.indexCurve)->dataSize()))
    {
        m_selectedPointIndex = idx;
        updateDetailsPanelsSelPoint();
        drawMarkerOnCurve();
    }
}

void ChartPointSelector::showSelectionPoint(QwtText xLbl, QwtText yLbl, QPointF point, QwtPlot::Axis axis)
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

void ChartPointSelector::drawMarkerOnCurve()
{
    if (!m_selectedPointIndex.isValid())
        return;

    QwtPlotCurve *curve = m_chart->curves()[m_selectedPointIndex.indexCurve];
    QPointF p = curve->sample(m_selectedPointIndex.indexPoint);

    QString xLbl, yLbl;
    m_chart->getCurvePointLabel(m_selectedPointIndex, xLbl, yLbl);
    QwtText txtX(xLbl), txtY(yLbl);
    txtX.setFont(m_chart->getSettings().m_markerLabelFont);
    txtY.setFont(m_chart->getSettings().m_markerLabelFont);

    txtX.setBackgroundBrush(QBrush(Qt::white));
    txtY.setBackgroundBrush(QBrush(Qt::white));
    txtX.setRenderFlags(Qt::AlignLeft);
    txtY.setRenderFlags(Qt::AlignLeft);

    showSelectionPoint(txtX, txtY, p, (QwtPlot::Axis)curve->yAxis());
}

void ChartPointSelector::updateDetailsPanelsSelPoint()
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

void ChartPointSelector::onCurveVisibilityChanged(int curveIdx, bool b)
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

void ChartPointSelector::onCurvePointSelected(const QPointF &pos)
{
    QList<QwtPlotCurve *> curves = m_chart->curves();
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

    drawMarkerOnCurve();
    updateDetailsPanelsSelPoint();

    emit pointSelected(m_selectedPointIndex);

}

void ChartPointSelector::onNextCurvePointSelected(bool prev)
{
    if (! m_selectedPointIndex.isValid())
        return;

    QwtPlotCurve *curve = m_chart->curves().at(m_selectedPointIndex.indexCurve);
    QwtSeriesData<QPointF> *samples = curve->data();

    if (samples->size() == 0)
        return;

    //int step = m_settings.keyStep;
    int step = 1;

    long startIdx = 0;
    long endIdx = samples->size() - 1;

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
    drawMarkerOnCurve();
    m_chart->getPanelCurveDetailsList()[m_selectedPointIndex.indexCurve]
            ->setCurrentSelPointIndex(m_selectedPointIndex.indexPoint);

    emit pointSelected(m_selectedPointIndex);
}
