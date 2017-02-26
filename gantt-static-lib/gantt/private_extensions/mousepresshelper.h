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
//    void updatePressedAtPos(const QPointF &newPos);

    bool isClick(const QPointF &pos) const;

    bool isSlide(const QPointF &pos) const;
    bool isHorSlide(const QPointF &pos);
    bool isVerSlide(const QPointF &pos);

    inline bool pressed() const;
    inline const QPointF &pos() const;
    inline Qt::MouseButton button() const;

private:
    bool _pressed;
    QPointF _pressedAtPos, _nextPressedAtPos;
    qint64 _pressedAtMSecs;

    bool _horSlide, _verSlide;


    static qint64 _slideDelay;
    static int _slideDistance;
    static int _updateDistance;
};


inline bool MousePressHelper::pressed() const { return _pressed;}
inline const QPointF &MousePressHelper::pos() const { return _pressedAtPos;}


#endif // MOUSEPRESSHELPER_H
