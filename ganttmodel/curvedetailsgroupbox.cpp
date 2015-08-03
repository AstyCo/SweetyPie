#include "curvedetailsgroupbox.h"
#include "ui_curvedetailsgroupbox.h"

#include <QColorDialog>

CurveDetailsGroupBox::CurveDetailsGroupBox(QwtPlotCurve *curve, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::CurveDetailsGroupBox)
{
    ui->setupUi(this);

    m_curve = 0;

    setCurrentIndex(-1);

    setMinMaxValue(0,0);

    setInterval(0,0);

    connect(ui->pushButtonColor,SIGNAL(clicked()),this,SLOT(changeColor()));
    connect(this,SIGNAL(toggled(bool)),this,SLOT(setCurveVisible(bool)));

    m_curve = curve;
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
long CurveDetailsGroupBox::minValue() const
{
    return m_minValue;
}

void CurveDetailsGroupBox::setMinMaxValue(long minValue,long maxValue)
{
    m_minValue = minValue;
    m_maxValue = maxValue;
    if(m_minValue!=0 || m_maxValue!=0)
    {
        ui->labelForMaxCapture->setVisible(true);
        ui->labelForMaxValue->setVisible(true);
        ui->labelForMinCapture->setVisible(true);
        ui->labelForMaxValue->setVisible(true);
        calcCurrentValuesData();
    }
    else
    {
        ui->labelForMaxCapture->setVisible(false);
        ui->labelForMaxValue->setVisible(false);
        ui->labelForMinCapture->setVisible(false);
        ui->labelForMaxValue->setVisible(false);
    }

}
long CurveDetailsGroupBox::maxValue() const
{
    return m_maxValue;
}


long CurveDetailsGroupBox::beginInterval() const
{
    return m_beginInterval;
}

void CurveDetailsGroupBox::setInterval(long beginInterval, long endInterval)
{
    m_beginInterval = beginInterval;
    m_endInterval = endInterval;
    if(m_beginInterval!=0 || m_endInterval!=0)
    {
        ui->labelAvgCapture_2->setVisible(true);
        ui->labelAvgValue_2->setVisible(true);
        ui->labelCountPointCapture_2->setVisible(true);
        ui->labelCountPoint_2->setVisible(true);
        ui->labelMaxCapture_2->setVisible(true);
        ui->labelMaxValue_2->setVisible(true);
        ui->labelMinCapture_2->setVisible(true);
        ui->labelMinValue_2->setVisible(true);
        calcIntervals();
    }
    else
    {
        ui->labelAvgCapture_2->setVisible(false);
        ui->labelAvgValue_2->setVisible(false);
        ui->labelCountPointCapture_2->setVisible(false);
        ui->labelCountPoint_2->setVisible(false);
        ui->labelMaxCapture_2->setVisible(false);
        ui->labelMaxValue_2->setVisible(false);
        ui->labelMinCapture_2->setVisible(false);
        ui->labelMinValue_2->setVisible(false);
    }
}
int CurveDetailsGroupBox::currentIndex() const
{
    return m_currentIndex;
}

void CurveDetailsGroupBox::setCurrentIndex(int currentIndex)
{
    m_currentIndex = currentIndex;
    if(m_currentIndex!=-1)
    {
        ui->labelCurentValue->setVisible(true);
        ui->labelCurentCaption->setVisible(true);
        ui->labelForMaxCapture->setVisible(true);
        ui->labelForMaxValue->setVisible(true);
        ui->labelForMinCapture->setVisible(true);
        ui->labelForMinValue->setVisible(true);
        calcCurrentValuesData();
    }
    else
    {
        ui->labelCurentValue->setVisible(false);
        ui->labelCurentCaption->setVisible(false);
        ui->labelForMaxCapture->setVisible(false);
        ui->labelForMaxValue->setVisible(false);
        ui->labelForMinCapture->setVisible(false);
        ui->labelForMinValue->setVisible(false);
    }
}

long CurveDetailsGroupBox::endInterval() const
{
    return m_endInterval;
}

void CurveDetailsGroupBox::updateData()
{
    if(m_curve==0)
        return;

    QString cl("background:%1;");
    ui->pushButtonColor->setStyleSheet(cl.arg(m_curve->pen().color().name()));
    ui->pushButtonColor->update();

    setTitle(m_curve->title().text());
    ui->labelTitle->setText(m_curve->title().text());

    double max=-2147483647, min=2147483647;
    double startVal, endVal;
    QString sMax, sMean, sMin, sCount;
    long startIdx = 0;
    long endIdx = m_curve->dataSize() - 1;
    long count = m_curve->dataSize();

    if (startIdx > endIdx)
    {
        // ни одна точка не попала в выделенный интервал
        sMax = sMean = sMin = "-";
    }
    else
    {
        startVal = max = min = m_curve->sample(startIdx).y();
        endVal = m_curve->sample(endIdx).y();
        double sumVal = startVal/count;
        for(long i = startIdx + 1; i <= endIdx; i++)
        {
            double y = m_curve->sample(i).y();
            sumVal+=y/count;
            if (y > max)
                max = y;
            else if (y < min)
                min = y;
        }

        sMax = QString::number(max, 'f', 6);
        sMin = QString::number(min, 'f', 6);
        sMean = QString::number(sumVal, 'f', 6);
        sCount = QString::number(count);
    }

    ui->labelMaxValue->setText(sMax);
    ui->labelAvgValue->setText(sMean);
    ui->labelMinValue->setText(sMin);
    ui->labelCountPoint->setText(sCount);
}

void CurveDetailsGroupBox::calcIntervals()
{
    if(m_curve==0)
        return;

    double max=-2147483647, min=2147483647;
    QString sMax, sMean, sMin, sCount;
    long count = 0;
    double sumVal = 0;

    for(long i = 0; i < m_curve->dataSize()-1; i++)
    {
        if(m_beginInterval<m_curve->sample(i).x())
        {
            if(m_endInterval<m_curve->sample(i).x())
                break;

            double y = m_curve->sample(i).y();
            count++;
            sumVal+=y/count;
            if (y > max)
                max = y;
            else if (y < min)
                min = y;
        }
    }

    if(count!=0)
    {
        sMax = QString::number(max, 'f', 6);
        sMin = QString::number(min, 'f', 6);
        sMean = QString::number(sumVal, 'f', 6);
        sCount = QString::number(count);
    }
    else
    {
        sMax = "-";
        sMin ="-";
        sMean = "-";
        sCount = QString::number(count);
    }

    ui->labelMaxValue_2->setText(sMax);
    ui->labelAvgValue_2->setText(sMean);
    ui->labelMinValue_2->setText(sMin);
    ui->labelCountPoint_2->setText(sCount);
}

void CurveDetailsGroupBox::calcCurrentValuesData()
{
    if(m_curve==0)
        return;
    if(m_currentIndex==-1)
        return;

    QPointF cur = m_curve->sample(m_currentIndex);
    ui->labelCurentValue->setText(QString::number(cur.y(),'f', 6));

    if(m_minValue!=0 || m_maxValue!=0)
    {
        ui->labelForMaxValue->setText(QString::number(m_maxValue - cur.y(),'f', 6));
        ui->labelForMinValue->setText(QString::number(cur.y() - m_minValue, 'f', 6));
    }

}

void CurveDetailsGroupBox::changeColor()
{
    if(m_curve!=0)
    {
        QColor color;
        if(m_curve->pen().color()==Qt::black)
            color = QColorDialog::getColor(Qt::white,this);
        else
            color = QColorDialog::getColor(m_curve->pen().color(),this);
        if(color.isValid())
        {
            m_curve->setPen(QPen(color));


            QString cl("background:%1;");
            ui->pushButtonColor->setStyleSheet(cl.arg(color.name()));
            ui->pushButtonColor->update();
            emit colorChanged(color);
        }
    }

}

void CurveDetailsGroupBox::setCurveVisible(bool b)
{
    if(m_curve!=0)
    {
        m_curve->setVisible(b);
        emit visibledChanged(b);
    }
}
