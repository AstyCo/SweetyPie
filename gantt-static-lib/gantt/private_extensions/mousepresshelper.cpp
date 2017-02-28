#include "mousepresshelper.h"
#include "gantt-lib_global_values.h"

#include <QDateTime>
#include <QLine>
#include <QDebug>

// if   passed less than _slideDelay milliseconds,
// and  the mouse has not moved farther than _slideDistance
// or   mouse wasn't moved at all
// then     user wants to Press , else user wants to Slide
//
qint64 MousePressHelper::_slideDelay = 0.5 * _MILISECONDS_IN_SECOND;

// distance - euclidean norm
int MousePressHelper::_slideDistance = 15;
int MousePressHelper::_updateDistance = 40;

// emit clickDelayElapsed after _doubleClickDelay[msec]
int MousePressHelper::_doubleClickDelay = 300;

void MousePressHelper::init()
{
    _pressed = false;

    _clickTimer.setSingleShot(true);
    _clickTimer.setInterval(_doubleClickDelay);
    connect(&_clickTimer, SIGNAL(timeout()), this, SLOT(emitClickDelayElapsed()));
}

QVector

MousePressHelper::MousePressHelper(QObject *parent)
    :QObject(parent)
{

}

void MousePressHelper::press(const QPointF &pos)
{
    _pressed = true;
    _nextPressedAtPos = _pressedAtPos = pos;
    _pressedAtMSecs = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "timer Started";
    _clickTimer.start();

    _horSlide = _verSlide = true;   // helpers
}

void MousePressHelper::doubleClick(const QPointF &pos)
{
    qDebug() << "doubleClick stop";
    _clickTimer.stop();

    emit doubleClicked(pos);
}

void MousePressHelper::move(const QPointF &pos)
{
    if (_pressed && _pressedAtPos != pos) {
        qDebug() << "move stop";
        _clickTimer.stop();
    }
}

void MousePressHelper::release()
{
    qDebug() << "release stop";

    _clickTimer.stop();
    _pressed = false;
}

//void MousePressHelper::updatePressedAtPos(const QPointF &newPos)
//{
//    if(!_pressed)
//        return;
//    if( _pressedAtPos == _nextPressedAtPos
//            && QLineF(_nextPressedAtPos, newPos).length() > _updateDistance / 2)
//        _nextPressedAtPos = newPos;

//    if(QLineF(_pressedAtPos, newPos).length() > _updateDistance)
//        _pressedAtPos = _nextPressedAtPos;
//}

bool MousePressHelper::isClick(const QPointF &pos) const
{
    if(!_pressed)
        return false;
    if((QDateTime::currentMSecsSinceEpoch() - _pressedAtMSecs < _slideDelay
            && QLineF(_pressedAtPos, pos).length() < _slideDistance)
            || _pressedAtPos == pos)
        return true;

    return false;
}

bool MousePressHelper::isSlide(const QPointF &pos) const
{
    if(!_pressed)
        return false;
    return !isClick(pos);
}

bool MousePressHelper::isHorSlide(const QPointF &pos)
{
    if(!_pressed || !_horSlide)
        return false;
    QLineF direction(_pressedAtPos, pos);
    qreal angle = direction.angle();
    static const int delta = 15;
    if(angle < delta
            || angle > 360 - delta
            || (angle > 180 - delta && angle < 180 + delta))
        return true;
    else{
        _horSlide = false;
        return false;
    }
}

bool MousePressHelper::isVerSlide(const QPointF &pos)
{
    QLineF direction(_pressedAtPos, pos);
    qreal angle = direction.angle();

    if(!_pressed || !_verSlide)
        return false;
    static const int delta = 20;
    if( (angle > 90 - delta && angle < 90 + delta)
            || (angle > 270 - delta && angle < 270 + delta) )
        return true;
    else{
        _verSlide = false;
        return false;
    }
}

void MousePressHelper::emitClickDelayElapsed()
{
    qDebug() << "emitClickDelayElapsed";
    emit clickDelayElapsed(_pressedAtPos);
}


