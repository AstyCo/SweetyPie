/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию MousePressHelper class.
 * \~englist
 * \brief File contains realization of MousePressHelper class.
 */
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
int MousePressHelper::_doubleClickDelay = 180;

void MousePressHelper::init()
{
    _pressed = false;
    _doubleClick = false;
    _slided = false;

    _clickTimer.setSingleShot(true);
    _clickTimer.setInterval(_doubleClickDelay);
    connect(&_clickTimer, SIGNAL(timeout()), this, SLOT(emitClickDelayElapsed()));
}

MousePressHelper::MousePressHelper(QObject *parent)
    :QObject(parent)
{
    init();
}

void MousePressHelper::press(const QPointF &pos)
{
    _pressed = true;
    _doubleClick = _slided = false;
    _nextPressedAtPos = _pressedAtPos = pos;
    _pressedAtMSecs = QDateTime::currentMSecsSinceEpoch();

    _horSlide = _verSlide = true;   // helpers
}

void MousePressHelper::doubleClick(const QPointF &pos)
{
    _doubleClick = true;
    _clickTimer.stop();

    emit doubleClicked(pos);
}

void MousePressHelper::move(const QPointF &pos)
{
    _currentMousePos = pos;
}

void MousePressHelper::setSlided()
{
    if(_slided)
        return;

    _slided = true;
}

void MousePressHelper::release()
{
//    qDebug() << "release";
    if (!_doubleClick)
        _clickTimer.start();

    _pressed = false;
}

bool MousePressHelper::isClick() const
{
    if(!_pressed)
        return false;

    return isClickH(_currentMousePos);
}

bool MousePressHelper::isSlide() const
{
    if(!_pressed)
        return false;

    return !isClickH(_currentMousePos);
}

bool MousePressHelper::isHorSlide()
{
    if (!_pressed || !_horSlide)
        return false;
    QLineF direction(_pressedAtPos, _currentMousePos);
    qreal angle = direction.angle();
    static const int delta = 15;
    if (angle < delta
            || angle > 360 - delta
            || (angle > 180 - delta && angle < 180 + delta))
        return true;
    else {
        _horSlide = false;
        return false;
    }
}

bool MousePressHelper::isVerSlide()
{
    QLineF direction(_pressedAtPos, _currentMousePos);
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

bool MousePressHelper::isClickH(const QPointF &pos) const
{
    return (QDateTime::currentMSecsSinceEpoch() - _pressedAtMSecs < _slideDelay
                && QLineF(_pressedAtPos, pos).length() < _slideDistance)
            || _pressedAtPos == pos;

}

void MousePressHelper::emitClickDelayElapsed()
{
//    qDebug() << "emitClickDelayElapsed";
    if(!_slided){
//        qDebug() << "!_slided";
        emit clickDelayElapsed(_pressedAtPos);
    }
}


