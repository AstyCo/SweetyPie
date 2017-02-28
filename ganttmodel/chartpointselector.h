#ifndef CHARTPOINTSELECTOR_H
#define CHARTPOINTSELECTOR_H

#include "chartxywidget.h"

#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_plot_picker.h>

#include "plotnavigator.h"
#include "ganttproject_global.h"
#include "curveindex.h"

class ChartXYWidget;

class GANTTMODELSHARED_EXPORT ChartPointSelector : public QObject
{
    Q_OBJECT
public:
    ChartPointSelector(ChartXYWidget *chart, PlotNavigator *nav);
    void clearSelectedPoint();

    CurveIndex selectedPoint() const { return m_selectedPointIndex; }
    void selectPointByIndex(CurveIndex idx);

protected:
    void showSelectionPoint(QwtText xLbl, QwtText yLbl, QPointF point, QwtPlot::Axis axis);
    virtual void drawMarkerOnCurve();
    void updateDetailsPanelsSelPoint();
public slots:
    void onCurveVisibilityChanged(int curveIdx, bool b);
    void onCurvePointSelected(const QPointF &pos);
protected slots:
    void onNextCurvePointSelected(bool prev);
signals:
    /// Точка выбрана пользователем
    void pointSelected(CurveIndex index);
private:
    QwtPlotPicker *m_picker;
    /** Маркеры обозначения выделенной точки.
    * Один маркер для вертикальной, один для горизонтальной линии
    */
    QwtPlotMarker *m_pMarker[2];

    ChartXYWidget *m_chart;
    QwtPlot *m_plot;
    PlotNavigator *m_navigator;

    /// Номер текущей выделенной точки
    CurveIndex m_selectedPointIndex;
};

#endif // CHARTPOINTSELECTOR_H
