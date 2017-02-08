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

void MousePressHelper::init()
{
    _pressed = false;
}

MousePressHelper::MousePressHelper()
{

}

void MousePressHelper::press(const QPointF &pos)
{
    _pressed = true;
    _pressedAtPos = pos;
    _pressedAtMSecs = QDateTime::currentMSecsSinceEpoch();
}

void MousePressHelper::release()
{
    _pressed = false;
}

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


