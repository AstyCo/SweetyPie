#ifndef CHARTSGROUPWIDGET_H
#define CHARTSGROUPWIDGET_H

#include <QToolButton>

#include "chartwidget.h"

namespace Ui {
class ChartsGroupWidget;
}


class GANTTMODELSHARED_EXPORT ChartsGroupWidget : public QWidget
{
  Q_OBJECT
  
public:
  explicit ChartsGroupWidget(QWidget *parent = 0);
  ~ChartsGroupWidget();

  void addChart(ChartWidget *chart);
  void insertChart(int index, ChartWidget *chart);
  ChartWidget *removeChartAt(int index);
  ChartWidget *at(int index);
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
  void updateSelectionPanel();

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
  void onToolButtonZoom_clicked();  

  void onAction_panelSelectionInterval_clicked();

  void on_dateTimeEdit_StartSelection_dateTimeChanged(const QDateTime &dateTime);

  void on_dateTimeEdit_EndSelection_dateTimeChanged(const QDateTime &dateTime);

  void on_doubleSpinBox_SelInterval_valueChanged(double arg1);

  void on_toolButton_SetDtIntervalSelection_clicked();

  void on_toolButton_CancelDtIntervalSelection_clicked();

  void on_toolButton_SetSelectionInterval_clicked();

  void on_toolButton_CancelSelectionInterval_clicked();

  void on_comboBox_IntervalTimeDim_currentIndexChanged(int index);
  void onPointSelected(CurveIndex idx);
  void onIntervalSelectionStart(QPointF pos);
  void onIntervalSelectionEnd(QPointF pos);
  void onTargetingDtSet(UtcDateTime dt);
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

  void setSelectionInterval(TimeSpan selInt);
  void updateSelectIntervalPanelDates(UtcDateTime startDt, UtcDateTime endDt);

  void hideEditIntervalButtons();

  void zoomChart(int newZoom);
  void interconnectCharts();
  void connectChart(ChartWidget *chart);
  void alignAxes(int axis);
  void alignScaleBorder(int axis);
  void createActionsToolBar();

private:
  /// Режимы изменения выбранного интервала
  enum IntervalEditState { iesNone,  ///< Интервал не изменялся
                           iesBegin, ///< Изменилось начало интервала
                           iesEnd,   ///< Изменилось окончание интервала
                           iesInt    ///< Изменилась длина интервала
                         };

  Ui::ChartsGroupWidget *ui;

  ChartActionsToolBar *m_actionsToolBar;  
  QAction *m_actionSetPanelSelectInterval;
  QList<ChartWidget *> m_charts;
  QList<QAction *> m_chartZoomActions;
  /// Текущие отображаемые графики имеют общую ось X
  bool m_syncChartsByAxisX;
  bool m_panelStatsVisible;
  /// Номер графика, развернутого на всю форму
  int m_zoomActive;
  bool m_panelSelectIntervalVisible;  

  UtcDateTime m_curStartDt;
  UtcDateTime m_curEndDt;

  /// Текущее состояние интервала
  IntervalEditState m_intState;
  TimeSpan::TimeIntervalTypes m_curSelTimeIntType;

  /// Текущий выделенный интервал
  TimeSpan m_selInterval;

  ChartSettings m_settings;  
};

#endif // CHARTSGROUPWIDGET_H

