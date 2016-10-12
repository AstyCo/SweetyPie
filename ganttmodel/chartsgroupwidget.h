#ifndef CHARTSGROUPWIDGET_H
#define CHARTSGROUPWIDGET_H

#include <QToolButton>

#include <utcdatetime.h>
#include <timespan.h>

#include "chartxywidget.h"
#include "charttimexywidget.h"
#include "chartintervalselectionwidget.h"

namespace Ui {
class ChartsGroupWidget;
}


class GANTTMODELSHARED_EXPORT ChartsGroupWidget : public QWidget
{
  Q_OBJECT
  
public:
  explicit ChartsGroupWidget(QWidget *parent = 0);
  ~ChartsGroupWidget();

  void addChart(ChartXYWidget *chart);
  void insertChart(int index, ChartXYWidget *chart);
  ChartXYWidget *removeChartAt(int index);
  ChartXYWidget *at(int index);
  int chartsCount() const { return m_charts.size(); }
  void clearChartsData();

  bool frameStatsEnabled() const;
  void setFrameStatsEnabled(bool frameStatsEnabled);

  bool syncChartsByAxisX() const;
  void setSyncChartsByAxisX(bool syncChartsByAxisX);

  void setTargetingPoint(UtcDateTime dt);
  void clearTargetingPoint();

  ChartActionsToolBar *getActionsToolBar() const;

  ChartSettings getSettings() const;
  void setSettings(const ChartSettings &settings);

  QList<ChartXYWidget *> charts() const;

signals:
  /// Выбрана вторая точка при выделении интервала
  void intervalSelectionEnded(QPointF p);
  void targetPointSelected(UtcDateTime dt);

public slots:
  void scaleDivChanged();
  void alignPanelsDetails();

protected:
  void resizeEvent(QResizeEvent *ev);

protected slots:
  void onToolButtonZoom_clicked();  

  void onPointSelected(CurveIndex idx);
  void onIntervalSelectionStart(QPointF pos);
  void onIntervalSelectionEnd(QPointF pos);
  void onTargetingDtSet(qreal value);
  void onScaleChanged(qreal scaleFactor, QPoint anchorPoint);
  void onChartPanned(int dx, int dy);

  //void on_toolButton_EndSelectionByMouse_clicked();

  void onAction_autoZoom_clicked();

  void onAction_grid_toggled(bool checked);

  void onAction_clearChart_clicked();

  void onAction_selectIntervalByMouse_toggled(bool checked);

  void onAction_selectTarget_toggled(bool checked);

  void onAction_chartSettings_triggered();
  void onAction_panelCurveDetails_toggled(bool checked);
  void onZoomed(const QRectF &rect);

protected:
  void connectChart(ChartXYWidget *chart);
  void zoomChart(int newZoom);
  void interconnectCharts();
  void alignAxes(int axis);
  void alignScaleBorder(int axis);
  void createActionsToolBar();

protected:
  Ui::ChartsGroupWidget *ui;

  ChartActionsToolBar *m_actionsToolBar;  
  QList<ChartXYWidget *> m_charts;
  QList<QAction *> m_chartZoomActions;
  /// Текущие отображаемые графики имеют общую ось X
  bool m_syncChartsByAxisX;
  bool m_panelStatsVisible;
  /// Номер графика, развернутого на всю форму
  int m_zoomActive;

  ChartSettings m_settings;  
};

class GANTTMODELSHARED_EXPORT ChartTimeXYGroupWidget : public ChartsGroupWidget
{
  Q_OBJECT

public:
  explicit ChartTimeXYGroupWidget(QWidget *parent = 0);

  void addChart(ChartTimeXYWidget *chart);
  void insertChart(int index, ChartTimeXYWidget *chart);

  /// Выполняет выделение интервала
  void selectIntervalByDates(UtcDateTime beginDt, ///< Начало интервала
                             UtcDateTime endDt ///< Конец интервала
                             );
  /// Дата и время начала выделенного интервала
  UtcDateTime getSelIntervalBeginDt();

  /// Дата и время конца выделенного интервала
  UtcDateTime getSelIntervalEndDt();
  bool panelSelectIntervalVisible() const;
  void setPanelSelectIntervalVisible(bool vis);

private slots:
  void onAction_panelSelectionInterval_clicked();

  void updateSelectionPanel();
  void onIntervalSelected();

  void onChartIntervalSelectionEnd(QPointF p);

protected:
  ChartIntervalSelectionWidget *m_selectionPanel;
  bool m_panelSelectIntervalVisible;
};

#endif // CHARTSGROUPWIDGET_H

