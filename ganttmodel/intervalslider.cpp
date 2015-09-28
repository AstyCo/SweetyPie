#include "intervalslider.h"
#include "QPainter"
#include "QStyle"
#include "QStylePainter"

#include <QKeyEvent>
#include <QtGui>


IntervalSlider::IntervalSlider(QWidget *parent) :
    QWidget(parent)
{
    _handleSize=12;

    _maxValue=100;
    _minValue=0;

    _beginValue=_minValue;
    _endValue=_maxValue;

    _eventHandle = NoHandle;

    connect(this,SIGNAL(valueChanged(IntervalSlider::EventHandle,long)),this,SLOT(repaint()));
}
int IntervalSlider::endValue() const
{
    return _endValue;
}

void IntervalSlider::setEndValue(int endValue)
{
    _endValue = endValue;

    if(_endValue>_maxValue)
    {
        _endValue=_maxValue;
    }
//    if((getPoint(_endValue)-_handleSize/2)<(getPoint(_beginValue)+_handleSize/2))
//    {
//        _endValue=getValue(QPoint(getPoint(_beginValue)+_handleSize,0));
//    }

    emit valueChanged(IntervalSlider::EndHandle,_endValue);
    repaint();
}
long IntervalSlider::beginValue() const
{
    return _beginValue;
}

void IntervalSlider::setBeginValue(long beginValue)
{
    _beginValue = beginValue;

//    if((getPoint(_beginValue)+_handleSize/2)>(getPoint(_endValue)-_handleSize/2))
//    {
//        _beginValue=getValue(QPoint(getPoint(_endValue)-_handleSize,0));
//    }
    if(_beginValue<_minValue)
    {
        _beginValue=_minValue;
    }

    emit valueChanged(IntervalSlider::BeginHandle,_beginValue);
    repaint();
}
long IntervalSlider::maxValue() const
{
    return _maxValue;
}

void IntervalSlider::setMaxValue(long maxValue)
{
    _maxValue = maxValue;

    if(_endValue>_maxValue)
    {
        _endValue=_maxValue;
        emit valueChanged(IntervalSlider::EndHandle,_endValue);
        repaint();
    }
//    if((getPoint(_beginValue)+_handleSize/2)>(getPoint(_endValue)-_handleSize/2))
//    {
//        _beginValue=getValue(QPoint(getPoint(_endValue)-_handleSize,0));
//        emit valueChanged(BeginHandle,_beginValue);
//    }

}

long IntervalSlider::minValue() const
{
    return _minValue;
}

void IntervalSlider::setMinValue(long minValue)
{
    _minValue = minValue;

    if(_beginValue<_minValue)
    {
        _beginValue=_minValue;
        emit valueChanged(IntervalSlider::BeginHandle,_beginValue);
        repaint();
    }
//    if((getPoint(_endValue)-_handleSize/2)<(getPoint(_beginValue)+_handleSize/2))
//    {
//        _endValue=getValue(QPoint(getPoint(_beginValue)+_handleSize,0));
//        emit valueChanged(EndHandle,_endValue);
//    }
}




long IntervalSlider::getValue(const QPoint &p) const
{
    return (((double)p.x()) / width() * _maxValue);
}

int IntervalSlider::getPoint(long value) const
{
    return ((double)value/(double)_maxValue *width());
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

void IntervalSlider::drawHandle(QPainter *painter, const QRect &sliderRect, int pos, EventHandle handle) const
{
    const int bw = 1;

    int x;
    if (handle == BeginHandle)
      x = pos;
    else if (handle == EndHandle)
      x = pos - _handleSize;
    else
      return;

    QRect handleRect(
                x,
                sliderRect.y(),
                _handleSize,
                sliderRect.height()
                );

    qDrawShadePanel( painter,
                     handleRect, palette(), false, bw,
                     &palette().brush( QPalette::Button ) );

    int lineX = handleRect.center().x();
    qDrawShadeLine( painter, lineX, sliderRect.top() + bw,
                    lineX, sliderRect.bottom() - bw,
                    palette(), true, 1 );
}

void IntervalSlider::drawSlider(QPainter *painter, const QRect &sliderRect) const
{
    QRect innerRect( sliderRect );


    int ws = 8;
    int ds = _handleSize / 2; //- 8;
    if ( ds < 1 )
        ds = 1;

    QRect rSlot;

    if ( innerRect.height() & 1 )
        ws++;

    rSlot = QRect( innerRect.x() + ds,
                   innerRect.y() + ( innerRect.height() - ws ) / 2,
                   innerRect.width() - 2 * ds, ws );


    QBrush brush = palette().brush( QPalette::Dark );
    qDrawShadePanel( painter, rSlot, palette(), true, 1 , &brush );

    drawHandle( painter, innerRect, getPoint(_beginValue), BeginHandle);
    drawHandle( painter, innerRect, getPoint(_endValue), EndHandle);

}

void IntervalSlider::keyPressEvent(QKeyEvent *e)
{
    return;
}

void IntervalSlider::wheelEvent(QWheelEvent *e)
{
    return;
}

void IntervalSlider::mouseMoveEvent(QMouseEvent *e)
{
    int val = getValue(e->pos());
    if(_eventHandle==BeginHandle)
    {
        if(val<_minValue)
          _beginValue = _minValue;
        else if((getPoint(val)+_handleSize)>(getPoint(_endValue)-_handleSize))
          return;
        else if(val>_maxValue)
          _beginValue = _maxValue;
        else
          _beginValue = val;

        emit valueChanged(IntervalSlider::BeginHandle, _beginValue);
    }
    else if(_eventHandle==EndHandle)
    {
        if(val>_maxValue)
          _endValue = _maxValue;
        else if((getPoint(val)-_handleSize)<(getPoint(_beginValue)+_handleSize))
          return;
        else if(val<_minValue)
          _endValue = _minValue;
        else
          _endValue = val;

        emit valueChanged(IntervalSlider::EndHandle, _endValue);
    }
}

void IntervalSlider::mouseReleaseEvent(QMouseEvent *e)
{
    mouseMoveEvent(e);
    _eventHandle=NoHandle;
}

void IntervalSlider::mousePressEvent(QMouseEvent *e)
{
        const QPoint &p = e->pos();

        long beginSlpiter1 = getPoint(_beginValue);
        long beginSlpiter2 = beginSlpiter1 + _handleSize;

        long endSlpiter2 = getPoint(_endValue);
        long endSlpiter1 = endSlpiter2 - _handleSize;

        if(p.x()>beginSlpiter1 && p.x()<beginSlpiter2)
        {
            _eventHandle=BeginHandle;
        }
        else if(p.x()>endSlpiter1 && p.x()<endSlpiter2)
        {
            _eventHandle=EndHandle;
        }
        else
            _eventHandle=NoHandle;
    return;
}
