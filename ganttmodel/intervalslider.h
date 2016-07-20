#ifndef INTERVALSLIDER_H
#define INTERVALSLIDER_H

#include <QPainter>
#include <QWidget>
#include "ganttmodel_global.h"
#include <QDebug>

/*!
 * \brief The IntervalSlider class
 *
 * signals: beginMoved(long val),
 *          endMoved(long val),
 *          valueChanged(ClippedHandle handle,long val) - Same to beginMoved and EndMoved, emits simultaneously
 *
 * members: long handleH - width() of handle
 *          long sliderV - height() of slider rectangle
 *          long offsetV - vertical offset of [slider rectangle] in external widget
 *
 */


class GANTTMODELSHARED_EXPORT IntervalSlider : public QWidget
{
    Q_OBJECT

public:
    enum  ClippedHandle
    {
        NoHandle,
        BeginHandle,
        EndHandle
    };


    explicit IntervalSlider(QWidget *parent = 0);
    ~IntervalSlider(){}
    long endHandle() const;
    void setEndHandle(int endHandle);

    long beginHandle() const;
    void setBeginHandle(long beginHandle);

    long maxValue() const;
    void setMaxValue(long maxValue);

    long minValue() const;
    void setMinValue(long minValue);

    void setLimits(long minValue,long maxValue);

    int handleSize() const;
    void setHandleSize(int new_handle_value);

    int sliderV() const{return m_sliderV;}
    void setSliderV(int new_sliderV);

    void setOffsetV(int new_offsetV);

    void repaint();

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    int intervalSliderHeight() const;
private:
    ClippedHandle m_clippedHandle;

    long    m_beginValue,
            m_endValue,
            m_maxValue,
            m_minValue;

    int m_handleH,
        m_sliderV,
        m_offsetV,
        m_borderWidth;

    int halfHandleH() const;
    void moveHandles(long deltaVal);

    long pointToValue(const QPoint &p,ClippedHandle handle) const;  ///< Handles exists in the own relative coordinate system, so needs to know which system before translation
    int valueToPoint(long value,ClippedHandle handle) const;        ///< Handles exists in the own relative coordinate system, so needs to know which system before translation

    void paintEvent( QPaintEvent *event );
    virtual void drawHandle( QPainter *painter,
        const QRect &sliderRect, int pos, ClippedHandle handle) const;
    virtual void drawSlider(QPainter *painter, const QRect &sliderRect ) const;
    virtual void wheelEvent( QWheelEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void mousePressEvent( QMouseEvent *e );
public:

signals:

    void beginMoved(long value);
    void endMoved(long value);

    void valueChanged(ClippedHandle e, long value);

};

#endif // INTERVALSPLIDER_H

