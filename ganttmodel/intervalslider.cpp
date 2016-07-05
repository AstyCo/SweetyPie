#include "intervalslider.h"

#include <QPainter>
#include <QStyle>
#include <QStylePainter>

#include <QKeyEvent>
#include <QStyleOption>
#include <QApplication>

#include <QDebug>


IntervalSlider::IntervalSlider(QWidget *parent) :
    QWidget(parent)
{
    setOffsetV(7);
    setSliderV(8);
    setHandleSize(12);
    m_borderWidth = 1;

    m_maxValue=100;
    m_minValue=0;

    m_beginValue=m_minValue;
    m_endValue=m_maxValue;

    m_clippedHandle = NoHandle;

    connect(this,SIGNAL(valueChanged(ClippedHandle,long)),this,SLOT(repaint()));

    adjustSize();

    repaint();
}
long IntervalSlider::endHandle() const
{
    return m_endValue;
}

void IntervalSlider::setEndHandle(int endValue)
{
    if(m_endValue==endValue)
        return;

//    qDebug() << "setEnd: "+QString::number(endValue);

    m_endValue = endValue;

    if(m_endValue>m_maxValue)
    {
        m_endValue=m_maxValue;
    }
    if(m_endValue<m_beginValue)
    {
        m_endValue=m_beginValue;
    }

    emit valueChanged(IntervalSlider::EndHandle,m_endValue);
    emit endMoved(m_endValue);
    repaint();
}
long IntervalSlider::beginHandle() const
{
    return m_beginValue;
}

void IntervalSlider::setBeginHandle(long beginValue)
{
    if(m_beginValue==beginValue)
        return;

//    qDebug() << "setBegin: "+QString::number(beginValue);

    m_beginValue = beginValue;

    if(m_beginValue<m_minValue)
    {
        m_beginValue=m_minValue;
    }

    if(m_endValue<m_beginValue)
    {
        m_beginValue=m_endValue;
    }

    emit valueChanged(IntervalSlider::BeginHandle,m_beginValue);
    emit beginMoved(m_beginValue);
    repaint();
}
long IntervalSlider::maxValue() const
{
    return m_maxValue;
}

void IntervalSlider::setMaxValue(long maxValue)
{
    if(m_maxValue == maxValue)
        return;
    m_maxValue = maxValue;
    setEndHandle(m_maxValue);
    repaint();
}

long IntervalSlider::minValue() const
{
    return m_minValue;
}

void IntervalSlider::setMinValue(long minValue)
{
    if(m_minValue == minValue)
        return;

    m_minValue = minValue;
    setBeginHandle(m_minValue);
    repaint();
}

void IntervalSlider::setLimits(long minValue,long maxValue)
{
    setMinValue(minValue);
    setMaxValue(maxValue);
}

int IntervalSlider::handleSize() const
{
    return m_handleH;
}

void IntervalSlider::setHandleSize(int new_handle_value)
{
    m_handleH = new_handle_value;
    setMinimumWidth(2*m_handleH);
    repaint();
}

void IntervalSlider::setSliderV(int new_sliderV)
{
    if(m_sliderV == new_sliderV)
        return;
    m_sliderV = new_sliderV;
    setMinimumHeight(intervalSliderHeight());
    setMaximumHeight(intervalSliderHeight());
    repaint();
}

void IntervalSlider::setOffsetV(int new_offsetV)
{
    if(m_offsetV == new_offsetV)
        return;
    m_offsetV=new_offsetV;
    setMinimumHeight(intervalSliderHeight());
    setMaximumHeight(intervalSliderHeight());
    repaint();
}

void IntervalSlider::repaint()
{
    return QWidget::repaint();
}




long IntervalSlider::pointToValue(const QPoint &p,ClippedHandle handle) const
{
//    qDebug() << "pointToValue";
//    qDebug()<<p;

    int x = p.x() - halfHandleH();
    int offset;
    long widthDiff = m_handleH +((handle==NoHandle)?(0):(m_handleH)),
         relWidth= width() - widthDiff;
    switch(handle)
    {
    case EndHandle:
        offset = m_handleH;
        break;
    default:
        offset = 0;
        break;
    }
    long relValue =((1.0*x-offset) / relWidth * (m_maxValue-m_minValue))+0.5;
    if (relValue<0)
        relValue=0;
    if(relValue>m_maxValue-m_minValue)
        relValue=m_maxValue-m_minValue;
    return m_minValue+relValue;
}

int IntervalSlider::valueToPoint(long value,ClippedHandle handle) const
{
    long relativeValue = value-m_minValue;
    int offset= halfHandleH();
    long widthDiff = m_handleH +((handle==NoHandle)?(0):(m_handleH)),
         relWidth= width() - widthDiff;
    switch(handle)
    {
    case EndHandle:
        offset+=m_handleH;
        break;
    default:
        offset+=0;
        break;
    }

    int relPoint = relativeValue*1.0/(m_maxValue-m_minValue) * relWidth;
    if(relPoint<0)
        relPoint=0;
    if(relPoint>relWidth)
        relPoint=relWidth;

//    qDebug() << "point: "+ QString::number(offset+relPoint);
    return offset+relPoint;
}

void IntervalSlider::paintEvent(QPaintEvent *event)
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    drawSlider( &painter, opt.rect );
}

void IntervalSlider::drawHandle(QPainter *painter, const QRect &sliderRect, int pos, ClippedHandle handle) const
{
    QPalette::ColorRole handleRole,handleLineRole;
    QColor color;

//    qDebug() << "pos: "+ QString::number(pos);

    if(handle == m_clippedHandle || (QApplication::keyboardModifiers()&Qt::ShiftModifier))
    {
        handleRole = QPalette::Mid;
        handleLineRole = QPalette::Button;
    }
    else
    {
        handleRole = QPalette::Button;
        handleLineRole = QPalette::Mid;
    }
    int handlePos = pos- halfHandleH();


    QRect handleRect(
                handlePos,
                sliderRect.y(),
                m_handleH,
                sliderRect.height()
                );

    int borderWidth = 1;


//    qDebug() << palette().color(darkerThanHandleRole);
    painter->setPen(palette().color(handleLineRole));
    painter->setBrush(palette().color(handleRole));

    qDrawShadePanel( painter,
                     handleRect, palette(), false, borderWidth,
                     &palette().brush(handleRole) );

    int lineX = handleRect.center().x();
    QLineF handleMidLine(lineX, handleRect.top()+borderWidth,
                         lineX, handleRect.bottom()-borderWidth);


    painter->drawLine(handleMidLine);

//    qDrawShadeLine( painter, lineX, handleRect.top()+borderWidth,
//                    lineX, handleRect.bottom()-borderWidth,
//                    palette(), true, 0, borderWidth );
}

void IntervalSlider::drawSlider(QPainter *painter, const QRect &sliderRect) const
{
    QRect innerRect( sliderRect );
    int sliderV = m_sliderV,
        sliderH = innerRect.width();

    QRect rSlot;

    rSlot = QRect( innerRect.x() + halfHandleH(),
                   innerRect.y() + m_offsetV,
                   sliderH - 2 * halfHandleH(), sliderV );


    QBrush brush = palette().brush( QPalette::Dark );
    qDrawShadePanel( painter, rSlot, palette(), true, m_borderWidth , &brush );

    drawHandle( painter, innerRect, valueToPoint(m_beginValue,BeginHandle), BeginHandle);
    drawHandle( painter, innerRect, valueToPoint(m_endValue,EndHandle), EndHandle);

}

void IntervalSlider::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers() & Qt::ShiftModifier)
    {
        repaint();
    }
}

void IntervalSlider::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Shift)
    {
        repaint();
    }
}

int IntervalSlider::intervalSliderHeight() const
{
    return m_sliderV+2*(m_offsetV+m_borderWidth);
}

int IntervalSlider::halfHandleH() const {return m_handleH/2;}

void IntervalSlider::moveHandles(long deltaVal)
{
    if(!deltaVal)
        return;
    if(deltaVal>0)
    {
        setEndHandle(endHandle()+deltaVal);
        setBeginHandle(beginHandle()+deltaVal);
    }
    else
    {
        setBeginHandle(beginHandle()+deltaVal);
        setEndHandle(endHandle()+deltaVal);
    }
}

void IntervalSlider::wheelEvent(QWheelEvent *e)
{
    Q_UNUSED(e);
    return;
}

void IntervalSlider::mouseMoveEvent(QMouseEvent *e)
{
    long val,deltaVal;

    if(m_clippedHandle==BeginHandle)
    {
        val = pointToValue(e->pos(),BeginHandle);
        if(QApplication::keyboardModifiers()&Qt::ShiftModifier)
        {
            deltaVal = val-beginHandle();
            if(endHandle()+deltaVal>m_maxValue)
            {
                deltaVal=m_maxValue-endHandle();
            }
            moveHandles(deltaVal);
        }
        else
            setBeginHandle(val);
    }
    else if(m_clippedHandle==EndHandle)
    {
        val = pointToValue(e->pos(),EndHandle);
        if(QApplication::keyboardModifiers()&Qt::ShiftModifier)
        {
            deltaVal = val-endHandle();
            if(beginHandle()+deltaVal<m_minValue)
            {
                deltaVal=-beginHandle()+m_minValue;
            }
            moveHandles(deltaVal);
        }
        else
            setEndHandle(val);
    }
}

void IntervalSlider::mouseReleaseEvent(QMouseEvent *e)
{
    mouseMoveEvent(e);
    m_clippedHandle=NoHandle;
    repaint();
}

void IntervalSlider::mousePressEvent(QMouseEvent *e)
{
        const QPoint &p = e->pos();

        int beginSpliter1 = valueToPoint(m_beginValue,BeginHandle)-halfHandleH(),
            beginSpliter2 = beginSpliter1 + m_handleH;

        int endSpliter1 = valueToPoint(m_endValue,EndHandle)-halfHandleH(),
             endSpliter2 = endSpliter1 + m_handleH;

        if(p.x()>beginSpliter1 && p.x()<beginSpliter2)
        {
            m_clippedHandle=BeginHandle;
        }
        else if(p.x()>endSpliter1 && p.x()<endSpliter2)
        {
            m_clippedHandle=EndHandle;
        }
        else
            m_clippedHandle=NoHandle;
    return;
}
