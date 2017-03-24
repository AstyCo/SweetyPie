/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление IntervalSlider class.
 * \~englist
 * \brief File contains definition of IntervalSlider class.
 */
#ifndef INTERVALSLIDER_H
#define INTERVALSLIDER_H

#include <QPainter>
#include <QWidget>
#include "ganttproject_global.h"
#include <QDebug>

/*!
 * \~russian
 * \brief Класс IntervalSlider class представляет двух-бегунковый слайдер
 *
 * Переопределяя функции drawHandle, drawSliderLine в классе-потомке можно настроить вид слайдера.
 * \~english
 * \brief Class IntervalSlider class represents slider with two handles
 *
 * Overloading drawHandle, drawSliderLine you may set the view of slider.
 */


class GANTTMODELSHARED_EXPORT IntervalSlider : public QWidget
{
    Q_OBJECT

    void init();
public:
    enum  ClippedHandle{
        NoHandle,
        BeginHandle,
        EndHandle
    };

    explicit IntervalSlider(QWidget *parent = 0);
    ~IntervalSlider(){}

    void setHandleSize(int new_handle_value);///< Устанавливает размер бегунков

    /** Устанавливают новые значения для бегунков */
    virtual void setEndHandle(long long endHandle, bool manually = false);
    virtual void setBeginHandle(long long beginHandle, bool manually = false);


    /** Устанавливают геометрию слайдера */
    void setSliderV(int new_sliderV);
    void setOffsetV(int new_offsetV);

    /** Устанавливает смещение в пикселях левого слайдера и правого слайдера,
     *  относительно начала/конца соответственно */
    void setLeftOffset(qreal value);
    void setRightOffset(qreal value);


    long long endHandle() const;
    long long beginHandle() const;
    long long maxValue() const;
    long long minValue() const;

    virtual void setLimits(long long minValue,long long maxValue);
    virtual void setHandles(long long beginHandle,long long endHandle, bool manually = false);

    int handleSize() const;
    int sliderV() const{return m_sliderV;}
    int intervalSliderHeight() const;

    qreal begin() const;
    qreal end() const;
    qreal valueToPos(long long val) const;

    long long sliderSize() const;

    int halfHandleSize() const;

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void setVisible(bool visible);
    void setPressedAt(int pos = -1);
    void clearPressed();

public slots:
    void resetHandles();
    void resetHandlesManually();

signals:
    void beginMovedManually(long long value);
    void endMovedManually(long long value);

    void beginMoved(long long value);
    void endMoved(long long value);
protected:
    void checkForCursor(const QPoint &pos);

    QRect getIntervalRect() const;
    QRect getSliderRect() const;
    QRect getBeginHandleRect() const;
    QRect getEndHandleRect() const;


    void paintEvent( QPaintEvent *event );
    virtual void drawHandle( QPainter *painter,
        const QRect& handleRect, bool is_selected = false) const;
    virtual void drawSliderLine(QPainter *painter, const QRect &sliderRect ) const;

//    bool eventFilter(QObject *, QEvent *);

    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mousePressEvent( QMouseEvent *e );
    void mouseDoubleClickEvent(QMouseEvent *e);

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    void leaveEvent(QEvent *);

    bool posOverBeginHandle(const QPoint &pos) const;
    bool posOverEndHandle(const QPoint &pos) const;
    bool posOverInterval(const QPoint &pos) const;

private:
    /** Устанавливают область значений */
    void setMaxValue(long long maxValue);
    void setMinValue(long long minValue);

    void setBeginAndEnd(long long begin, long long end, bool manually = false);



protected:
    ClippedHandle m_clippedHandle;

    long long    m_beginValue,
            m_endValue,
            m_maxValue,
            m_minValue;

    int m_handleH,
        m_sliderV,
        m_offsetV,
        m_borderWidth,
        _pressedAt;

    qreal m_leftOffset,
        m_rightOffset;

    bool /*m_shiftModifier,*/
        m_limitsSet,
        m_isHidden,
        _pressed;

protected:
    virtual bool moveHandles(long long deltaVal, bool manually = false);

    long long pointToValue(int x, ClippedHandle handle) const;
    long long pointToValue(const QPoint &p,ClippedHandle handle) const;  ///< Handles exists in the own relative coordinate system, so needs to know which system before translation
    int valueToPoint(long long value,ClippedHandle handle) const;        ///< Handles exists in the own relative coordinate system, so needs to know which system before translation

    void checkHandlesRanges();
    void checkItemRange(long long &item);
};

#endif // INTERVALSPLIDER_H

