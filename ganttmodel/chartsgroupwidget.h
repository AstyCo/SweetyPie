#ifndef CHARTSDLG_H
#define CHARTSDLG_H

#include <QToolButton>
#include "chartwidget.h"

/// Режимы изменения выбранного интервала
enum IntervalEditState { iesNone,  ///< Интервал не изменялся
                         iesBegin, ///< Изменилось начало интервала
                         iesEnd,   ///< Изменилось окончание интервала
                         iesInt    ///< Изменилась длина интервала
                       };

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
  bool getSelectIntervalPanelEnabled() const;
  void setSelectIntervalPanelEnabled(bool selectIntervalPanelEnabled);

signals:
  /// Выбрана вторая точка при выделении интервала
  void intervalSelectionEnded(QPointF p);
  void targetPointSelected(UtcDateTime dt);

public slots:
  void scaleDivChanged();
protected:
  void resizeEvent(QResizeEvent *ev);

private slots:
  void onToolButton_Zoom_clicked();

  //void on_toolButton_GraphSettings_clicked();

  void on_toolButton_details_clicked();

  void on_toolButton_SelectionPanel_clicked();

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

  void on_toolButton_AutoZoom_clicked();

  void on_toolButton_Grid_toggled(bool checked);

  void on_toolButton_Clear_clicked();

  void on_toolButton_SelectIntervalByMouse_toggled(bool checked);

  void on_toolButton_SelectTarget_toggled(bool checked);

private:
  void updateSelectionPanel();
  void setSelectionInterval(TimeSpan selInt);
  void updateSelectIntervalPanelDates(UtcDateTime startDt, UtcDateTime endDt);

  void hideEditIntervalButtons();

  void zoomChart(int newZoom);
  void interconnectCharts();
  void connectChart(ChartWidget *chart);

private:
  Ui::ChartsGroupWidget *ui;

  QList<ChartWidget *> m_charts;
  QList<QToolButton *> m_chartZoomButtons;
  /// Текущие отображаемые графики имеют общую ось X
  bool m_syncChartsByAxisX;
  bool m_frameStatsEnabled;
  /// Номер графика, развернутого на всю форму
  int m_zoomActive;
  bool m_selectIntervalPanelEnabled;
  bool m_detailsPanelVisible;

  UtcDateTime m_curStartDt;
  UtcDateTime m_curEndDt;

  /// Текущее состояние интервала
  IntervalEditState m_intState;
  TimeSpan::TimeIntervalTypes m_curSelTimeIntType;

  /// Текущий выделенный интервал
  TimeSpan m_selInterval;
  void alignAxes(int axis);
  void alignScaleBorder(int axis);
};

#endif // CHARTSDLG_H

