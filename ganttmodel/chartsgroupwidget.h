#ifndef CHARTSGROUPWIDGET_H
#define CHARTSGROUPWIDGET_H

#include <QToolButton>

#include <utcdatetime.h>
#include <timespan.h>

#include "chartxywidget.h"

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


  /// Выполняет выделение интервала
  void selectIntervalByDates(UtcDateTime beginDt, ///< Начало интервала
                             UtcDateTime endDt ///< Конец интервала
                             );

  void setTargetingPoint(UtcDateTime dt);
  void clearTargetingPoint();
  /// Дата и время начала выделенного интервала
  UtcDateTime getSelIntervalBeginDt();

  /// Дата и время конца выделенного интервала
  UtcDateTime getSelIntervalEndDt();
  bool panelSelectIntervalVisible() const;
  void setPanelSelectIntervalVisible(bool vis);  

  ChartActionsToolBar *getActionsToolBar() const;

  ChartSettings getSettings() const;
  void setSettings(const ChartSettings &settings);

signals:
  /// Выбрана вторая точка при выделении интервала
  void intervalSelectionEnded(QPointF p);
  void targetPointSelected(UtcDateTime dt);

public slots:
  void scaleDivChanged();
  void alignPanelsDetails();

protected:
  void resizeEvent(QResizeEvent *ev);

private slots:
  void onAction_panelSelectionInterval_clicked();

  void onToolButtonZoom_clicked();  

  void updateSelectionPanel();
  void onIntervalSelected();

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

private:
  void connectChart(ChartXYWidget *chart);
  void zoomChart(int newZoom);
  void interconnectCharts();
  void alignAxes(int axis);
  void alignScaleBorder(int axis);
  void createActionsToolBar();

private:  
  Ui::ChartsGroupWidget *ui;

  ChartActionsToolBar *m_actionsToolBar;  
  QList<ChartXYWidget *> m_charts;
  QList<QAction *> m_chartZoomActions;
  /// Текущие отображаемые графики имеют общую ось X
  bool m_syncChartsByAxisX;
  bool m_panelStatsVisible;
  /// Номер графика, развернутого на всю форму
  int m_zoomActive;
  bool m_panelSelectIntervalVisible;  

  ChartSettings m_settings;  
};

#endif // CHARTSGROUPWIDGET_H

