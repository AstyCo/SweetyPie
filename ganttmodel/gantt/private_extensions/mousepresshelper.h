#ifndef MOUSEPRESSHELPER_H
#define MOUSEPRESSHELPER_H

#include <QPoint>


class MousePressHelper
{
    void init();
public:
    MousePressHelper();

    void press(const QPointF &pos);
    void release();

    bool isClick(const QPointF &pos) const;
    bool isSlide(const QPointF &pos) const;
    inline bool pressed() const;
    inline const QPointF &pos() const;

private:
    bool _pressed;
    QPointF _pressedAtPos;
    qint64 _pressedAtMSecs;

    static qint64 _slideDelay;
    static int _slideDistance;
};


inline bool MousePressHelper::pressed() const { return _pressed;}
inline const QPointF &MousePressHelper::pos() const { return _pressedAtPos;}

#endif // MOUSEPRESSHELPER_H
