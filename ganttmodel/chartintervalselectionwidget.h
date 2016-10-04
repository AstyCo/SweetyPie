#ifndef CHARTINTERVALSELECTIONWIDGET_H
#define CHARTINTERVALSELECTIONWIDGET_H

#include <QWidget>
#include <QDateTime>

#include <utcdatetime.h>
#include <timespan.h>
#include <utcdatetimeinterval.h>

namespace Ui {
class ChartIntervalSelectionWidget;
}

class ChartIntervalSelectionWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ChartIntervalSelectionWidget(QWidget *parent = 0);
  ~ChartIntervalSelectionWidget();

  /// Разрешенный интервал времени, в пределах которого
  /// пользователь может выбирать время
  UtcDateTimeInterval availableInterval() const;
  /// Установить разрешенный интервал времени, в пределах которого
  /// пользователь может выбирать время
  void setAvailableInterval(const UtcDateTimeInterval &availableInterval);

  /// Выбранный пользователем интервал времени,
  /// всегда находится в пределах разрешенного интервала
  UtcDateTimeInterval selectedInterval() const;
  /// Установить выбранный пользователем интервал времени,
  /// всегда находится в пределах разрешенного интервала
  void setSelectedInterval(const UtcDateTimeInterval &selectedInterval);

signals:
  /// Пользователь изменил интервал
  void intervalChanged();

private slots:
  void on_dateTimeEdit_intervalBegin_dateTimeChanged(const QDateTime &dateTime);

  void on_dateTimeEdit_intervalEnd_dateTimeChanged(const QDateTime &dateTime);

  void on_doubleSpinBox_intervalLength_valueChanged(double arg1);

  void on_toolButton_SetDtIntervalSelection_clicked();

  void on_toolButton_CancelDtIntervalSelection_clicked();

  void on_toolButton_SetIntervalLength_clicked();

  void on_toolButton_CancelIntervalLength_clicked();

  void on_comboBox_IntervalTimeDim_currentIndexChanged(int index);

private:
  void updateIntervalLength(TimeSpan selInt);
  void updateSelectIntervalPanelDates(const UtcDateTimeInterval &newInt);

  void hideEditIntervalButtons();

private:
  /// Режимы изменения выбранного интервала
  enum IntervalEditState { iesNone,  ///< Интервал не изменялся
                           iesBegin, ///< Изменилось начало интервала
                           iesEnd,   ///< Изменилось окончание интервала
                           iesInt    ///< Изменилась длина интервала
                         };

  Ui::ChartIntervalSelectionWidget *ui;

  UtcDateTimeInterval m_availableInterval;

  UtcDateTimeInterval m_selectedInterval;

  /// Текущее состояние интервала
  IntervalEditState m_intState;
  TimeSpan::TimeIntervalTypes m_curSelTimeIntType;
};

#endif // CHARTINTERVALSELECTIONWIDGET_H
