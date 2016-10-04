#include "curvedetailsgroupbox.h"
#include "ui_curvedetailsgroupbox.h"

#include <QColorDialog>

CurveDetailsGroupBox::CurveDetailsGroupBox(QwtPlotCurve *curve, QWidget *parent) :
  QFrame(parent),
  ui(new Ui::CurveDetailsGroupBox)
{
  ui->setupUi(this);

  m_curve = curve;

  setCurrentSelPointIndex(-1);

  setDimensionsText(QString());

  connect(ui->checkBox_title, SIGNAL(toggled(bool)), this, SLOT(setCurveVisible(bool)));

  updateCurveColor();
  updateData();
}

CurveDetailsGroupBox::~CurveDetailsGroupBox()
{
  delete ui;
}

QwtPlotCurve *CurveDetailsGroupBox::curve() const
{
  return m_curve;
}

void CurveDetailsGroupBox::setCurve(QwtPlotCurve *curve)
{
  m_curve = curve;
  updateData();
}

long CurveDetailsGroupBox::beginInterval() const
{
  return m_beginSelectionValue;
}

void CurveDetailsGroupBox::setInterval(double beginInterval, double endInterval)
{
  m_beginSelectionValue = beginInterval;
  m_endSelectionValue = endInterval;

  if(m_curve == 0)
    return;

  calcStats(true);
}

void CurveDetailsGroupBox::setIntervalLabelsVisible(bool vis)
{
  ui->labelIntervalAvgCaption->setVisible(vis);
  ui->labelIntervalAvgValue->setVisible(vis);
  ui->labelIntervalAvgValueDim->setVisible(vis);
  ui->labelIntervalCountPointCaption->setVisible(vis);
  ui->labelIntervalCountPointValue->setVisible(vis);
  ui->labelIntervalMaxCaption->setVisible(vis);
  ui->labelIntervalMaxValue->setVisible(vis);
  ui->labelIntervalMaxValueDim->setVisible(vis);
  ui->labelIntervalMinCaption->setVisible(vis);
  ui->labelIntervalMinValue->setVisible(vis);
  ui->labelIntervalMinValueDim->setVisible(vis);
  ui->labelIntervalDiffCaption->setVisible(vis);
  ui->labelIntervalDiffValue->setVisible(vis);
  ui->labelIntervalDiffValueDim->setVisible(vis);
}

void CurveDetailsGroupBox::setDimensionsText(QString dim)
{
  ui->labelMaxValueDim->setText(dim);
  ui->labelAvgValueDim->setText(dim);
  ui->labelMinValueDim->setText(dim);
  ui->labelCurrentValueDim->setText(dim);

  ui->labelIntervalMaxValueDim->setText(dim);
  ui->labelIntervalAvgValueDim->setText(dim);
  ui->labelIntervalMinValueDim->setText(dim);
  ui->labelIntervalDiffValueDim->setText(dim);
}

void CurveDetailsGroupBox::setCurveCheckable(bool checkable)
{
  ui->checkBox_title->setCheckable(checkable);
  if (checkable)
  {
    ui->checkBox_title->setStyleSheet(QString::null);
    bool b = ui->checkBox_title->blockSignals(true);
    ui->checkBox_title->setChecked(m_curve->isVisible());
    ui->checkBox_title->blockSignals(b);
  }
  else
    ui->checkBox_title->setStyleSheet("QCheckBox::indicator { width: 1px; height: 1px; }");
}

int CurveDetailsGroupBox::currentSelPointIndex() const
{
  return m_currentIndex;
}

void CurveDetailsGroupBox::setCurrentSelPointIndex(int currentIndex)
{
  m_currentIndex = currentIndex;

  if ((m_currentIndex != -1) && (m_curve != NULL))
  {
    QString selValue = QString::number(m_curve->sample(m_currentIndex).y(), 'f', 6);
    ui->labelCurrentValue->setText(selValue);
  }
}

long CurveDetailsGroupBox::endInterval() const
{
  return m_endSelectionValue;
}

void CurveDetailsGroupBox::updateData()
{
  if(m_curve == 0)
    return;

  ui->checkBox_title->setText(m_curve->title().text());
  bool b = ui->checkBox_title->blockSignals(true);
  ui->checkBox_title->setChecked(m_curve->isVisible());
  ui->checkBox_title->blockSignals(b);

  calcStats(false);
}

void CurveDetailsGroupBox::calcStats(bool hasSelection)
{  
  QString sMax, sAvg, sMin, sDiff, sCount;
  long startIdx = 0;
  long endIdx = m_curve->dataSize() - 1;
  long count = m_curve->dataSize();

  if (hasSelection)
  {
    findSelectionPointIdxs(startIdx, endIdx);
    count = endIdx - startIdx;
  }

  bool validIndexes = (startIdx != -1) && (endIdx != -1);
  if (! validIndexes || (startIdx > endIdx))
  {
    // ни одна точка не попала в выделенный интервал
    sMax = sAvg = sMin = sDiff = sCount = "-";
  }
  else
  {
    double max, min;
    double startVal = max = min = m_curve->sample(startIdx).y();
    double avgVal = startVal / count;
    for(long i = startIdx + 1; i <= endIdx; i++)
    {
      double y = m_curve->sample(i).y();
      avgVal += y / count;
      if (y > max)
        max = y;
      else if (y < min)
        min = y;
    }

    sMax = QString::number(max, 'f', 6);
    sMin = QString::number(min, 'f', 6);
    sAvg = QString::number(avgVal, 'f', 6);
    sDiff = QString::number(qAbs(max - min), 'f', 6);
    sCount = QString::number(count);
  }

  if (hasSelection)
  {
    ui->labelIntervalDiffValue->setText(sDiff);
    ui->labelIntervalMaxValue->setText(sMax);
    ui->labelIntervalAvgValue->setText(sAvg);
    ui->labelIntervalMinValue->setText(sMin);
    ui->labelIntervalCountPointValue->setText(sCount);
  }
  else
  {
    ui->labelMaxValue->setText(sMax);
    ui->labelAvgValue->setText(sAvg);
    ui->labelMinValue->setText(sMin);
    ui->labelCountPoint->setText(sCount);
  }
}

void CurveDetailsGroupBox::updateCurveColor()
{
  if(m_curve == NULL)
    return;

  QColor color = m_curve->pen().color();

  QString cl("background-color:%1;");
  ui->label_curveColor->setStyleSheet(cl.arg(color.name()));
  ui->label_curveColor->update();
}

void CurveDetailsGroupBox::setCurveVisible(bool b)
{
  ui->frameAllValues->setEnabled(b);
  if(m_curve != 0)
  {
    m_curve->setVisible(b);
    emit visibledChanged(b);
  }
}

void CurveDetailsGroupBox::findSelectionPointIdxs(long &begin, long &end)
{
  begin = -1;
  end = -1;
  for(long i = 0; i < long(m_curve->dataSize()) - 1; i++)
  {
    double curValueX = m_curve->sample(i).x();
    if (begin == -1)
    {
      if(m_beginSelectionValue <= curValueX)
        begin = i;
      else
        continue;
    }

    end = i - 1;

    if(curValueX >= m_endSelectionValue)
      break;
  }
}
