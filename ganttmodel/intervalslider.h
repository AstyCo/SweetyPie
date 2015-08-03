#ifndef INTERVALSLIDER_H
#define INTERVALSLIDER_H

#include "qwt/qwt_slider.h"
#include "QPainter"
#include "ganttmodel_global.h"

class GANTTMODELSHARED_EXPORT IntervalSlider : public QWidget
{
    Q_OBJECT
public:
    enum EventHandle{
        NoHandle,
        BeginHandle,
        EndHandle
    } _eventHandle;

    explicit IntervalSlider(QWidget *parent = 0);
    int endValue() const;
    void setEndValue(int endValue);

    long beginValue() const;
    void setBeginValue(long beginValue);

    long maxValue() const;
    void setMaxValue(long maxValue);

    long minValue() const;
    void setMinValue(long minValue);

private:


    long _beginValue;
    long _endValue;

    long _maxValue;
    long _minValue;

    int _handleSize;

    long getValue(const QPoint &p) const;
    int getPoint(long value) const;
    void paintEvent( QPaintEvent *event );
    //void paintEvent(QPaintEvent *ev);
    virtual void drawHandle( QPainter *painter,
        const QRect &sliderRect, int pos ) const;
    virtual void drawSlider(QPainter *painter, const QRect &sliderRect ) const;
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void wheelEvent( QWheelEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void mousePressEvent( QMouseEvent *e );

signals:
    void valueChanged(EventHandle ev, long value);

private slots:

};

#endif // INTERVALSPLIDER_H
