#ifndef CURVEDETAILSGROUPBOX_H
#define CURVEDETAILSGROUPBOX_H

#include <QGroupBox>
#include "qwt/qwt_plot_curve.h"




namespace Ui {
class CurveDetailsGroupBox;
}

class CurveDetailsGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit CurveDetailsGroupBox(QwtPlotCurve * curve, QWidget *parent = 0);
    ~CurveDetailsGroupBox();

    QwtPlotCurve *curve() const;
    void setCurve(QwtPlotCurve *curve);

    double minValue() const;
    double maxValue() const;
    void setMinMaxValue(double minValue, double maxValue);

    long endInterval() const;
    long beginInterval() const;
    void setInterval(long beginInterval, long endInterval);

    int currentIndex() const;
    void setCurrentIndex(int currentIndex);

private:
    Ui::CurveDetailsGroupBox *ui;

    int m_currentIndex;

    double m_minValue;
    double m_maxValue;

    long m_beginInterval;
    long m_endInterval;

    QwtPlotCurve * m_curve;
public slots:
    void updateData();
    void calcIntervals();
    void calcCurrentValuesData();

private slots:
    void changeColor();
    void setCurveVisible(bool b);


signals:
    void colorChanged(const QColor &color);
    void visibledChanged(bool b);
};

#endif // CURVEDETAILSGROUPBOX_H
