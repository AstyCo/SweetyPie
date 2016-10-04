#include "chartintervalselectionwidget.h"
#include "ui_chartintervalselectionwidget.h"

ChartIntervalSelectionWidget::ChartIntervalSelectionWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ChartIntervalSelectionWidget)
{
  ui->setupUi(this);

  ui->doubleSpinBox_intervalLength->setLocale(QLocale::C);

  ui->toolButton_SetDtIntervalSelection->hide();
  ui->toolButton_CancelDtIntervalSelection->hide();
  ui->toolButton_SetIntervalLength->hide();
  ui->toolButton_CancelIntervalLength->hide();

  m_intState = iesNone;

  m_curSelTimeIntType = (TimeSpan::TimeIntervalTypes) ui->comboBox_IntervalTimeDim->currentIndex();
}

ChartIntervalSelectionWidget::~ChartIntervalSelectionWidget()
{
  delete ui;
}


void ChartIntervalSelectionWidget::on_dateTimeEdit_intervalBegin_dateTimeChanged(const QDateTime &dateTime)
{
  Q_UNUSED(dateTime)
  ui->toolButton_SetDtIntervalSelection->show();
  ui->toolButton_CancelDtIntervalSelection->show();
  m_intState = iesBegin;
}

void ChartIntervalSelectionWidget::on_dateTimeEdit_intervalEnd_dateTimeChanged(const QDateTime &dateTime)
{
  Q_UNUSED(dateTime)
  ui->toolButton_SetDtIntervalSelection->show();
  ui->toolButton_CancelDtIntervalSelection->show();
  m_intState = iesEnd;
}

void ChartIntervalSelectionWidget::on_doubleSpinBox_intervalLength_valueChanged(double arg1)
{
  Q_UNUSED(arg1)
  ui->toolButton_SetIntervalLength->show();
  ui->toolButton_CancelIntervalLength->show();
  m_intState = iesInt;
}

void ChartIntervalSelectionWidget::on_toolButton_SetDtIntervalSelection_clicked()
{
  UtcDateTime startDt(ui->dateTimeEdit_intervalBegin->dateTime());
  UtcDateTime endDt(ui->dateTimeEdit_intervalEnd->dateTime());

  UtcDateTimeInterval newInt(startDt, endDt);
  setSelectedInterval(newInt);

  hideEditIntervalButtons();

  emit intervalChanged();
}

void ChartIntervalSelectionWidget::on_toolButton_CancelDtIntervalSelection_clicked()
{
  updateSelectIntervalPanelDates(m_selectedInterval);

  hideEditIntervalButtons();
}

void ChartIntervalSelectionWidget::on_toolButton_SetIntervalLength_clicked()
{
  UtcDateTime startDt(ui->dateTimeEdit_intervalBegin->dateTime());
  TimeSpan newIntLength(ui->doubleSpinBox_intervalLength->value(), m_curSelTimeIntType);
  UtcDateTime endDt = startDt + newIntLength;

  UtcDateTimeInterval newInt(startDt, endDt);
  if (! m_availableInterval.contains(newInt))
    newInt.setEnd(m_availableInterval.end());

  setSelectedInterval(newInt);

  hideEditIntervalButtons();

  emit intervalChanged();
}

void ChartIntervalSelectionWidget::on_toolButton_CancelIntervalLength_clicked()
{
  updateIntervalLength(m_selectedInterval.length());

  hideEditIntervalButtons();
}

void ChartIntervalSelectionWidget::on_comboBox_IntervalTimeDim_currentIndexChanged(int index)
{
  m_curSelTimeIntType = (TimeSpan::TimeIntervalTypes)index;

  bool b = ui->doubleSpinBox_intervalLength->blockSignals(true);
  ui->doubleSpinBox_intervalLength->setValue(m_selectedInterval.length()
                                             .totalTime(m_curSelTimeIntType));
  ui->doubleSpinBox_intervalLength->blockSignals(b);
}

void ChartIntervalSelectionWidget::updateIntervalLength(TimeSpan selInt)
{
  bool b = ui->doubleSpinBox_intervalLength->blockSignals(true);
  ui->doubleSpinBox_intervalLength->setValue(selInt.totalTime(m_curSelTimeIntType));
  ui->doubleSpinBox_intervalLength->blockSignals(b);
}

void ChartIntervalSelectionWidget::updateSelectIntervalPanelDates(const UtcDateTimeInterval &newInt)
{
  bool b = ui->dateTimeEdit_intervalBegin->blockSignals(true);
  ui->dateTimeEdit_intervalBegin->setDateTime(newInt.begin().dateTime());
  ui->dateTimeEdit_intervalBegin->blockSignals(b);

  b = ui->dateTimeEdit_intervalEnd->blockSignals(true);
  ui->dateTimeEdit_intervalEnd->setDateTime(newInt.end().dateTime());
  ui->dateTimeEdit_intervalEnd->blockSignals(b);

  updateIntervalLength(newInt.length());
}

void ChartIntervalSelectionWidget::hideEditIntervalButtons()
{
  ui->toolButton_SetDtIntervalSelection->hide();
  ui->toolButton_CancelDtIntervalSelection->hide();

  ui->toolButton_SetIntervalLength->hide();
  ui->toolButton_CancelIntervalLength->hide();

  m_intState = iesNone;
}

void ChartIntervalSelectionWidget::setSelectedInterval(const UtcDateTimeInterval &selectedInterval)
{
  if (! m_availableInterval.contains(selectedInterval))
    m_selectedInterval = m_availableInterval.intersectionInterval(selectedInterval);
  else
    m_selectedInterval = selectedInterval;

  updateSelectIntervalPanelDates(m_selectedInterval);
}

UtcDateTimeInterval ChartIntervalSelectionWidget::selectedInterval() const
{
  return m_selectedInterval;
}

UtcDateTimeInterval ChartIntervalSelectionWidget::availableInterval() const
{
  return m_availableInterval;
}

void ChartIntervalSelectionWidget::setAvailableInterval(const UtcDateTimeInterval &availableInterval)
{
  m_availableInterval = availableInterval;

  if (! m_selectedInterval.isValid())
    setSelectedInterval(m_availableInterval);
  else if (! m_availableInterval.contains(m_selectedInterval))
    setSelectedInterval(m_selectedInterval);
}
