#ifndef CURVEDETAILSGROUPBOX_H
#define CURVEDETAILSGROUPBOX_H

#include <QGroupBox>

#include <qwt/qwt_plot_curve.h>

#include "ganttmodel_global.h"


namespace Ui {
class CurveDetailsGroupBox;
}

class GANTTMODELSHARED_EXPORT CurveDetailsGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit CurveDetailsGroupBox(QwtPlotCurve * curve, QWidget *parent = 0);
    ~CurveDetailsGroupBox();

    QwtPlotCurve *curve() const;
    void setCurve(QwtPlotCurve *curve);

    long endInterval() const;
    long beginInterval() const;
    void setInterval(double beginInterval, double endInterval);

    void setIntervalLabelsVisible(bool vis);

    void setDimensionsText(QString dim);

    int currentSelPointIndex() const;
    void setCurrentSelPointIndex(int currentSelPointIndex);

public slots:
    void updateData();
    void updateCurveColor();
    void calcStats(bool hasSelection);

signals:
    void colorChanged(const QColor &color);
    void visibledChanged(bool b);

private slots:
    void setCurveVisible(bool b);

private:
    Ui::CurveDetailsGroupBox *ui;

    int m_currentIndex;

    double m_beginSelectionValue;
    double m_endSelectionValue;

    QwtPlotCurve * m_curve;

    void findSelectionPointIdxs(long &begin, long &end);
};

#endif // CURVEDETAILSGROUPBOX_H
