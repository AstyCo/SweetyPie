#ifndef MOUSEPRESSHELPER_H
#define MOUSEPRESSHELPER_H

#include <QPoint>
#include <QObject>
#include <QTimer>

class MousePressHelper : public QObject
{
    Q_OBJECT

    void init();
public:
    MousePressHelper(QObject *parent = 0);

    void press(const QPointF &pos);
    void doubleClick(const QPointF &pos);
    void move(const QPointF &pos);
    void release();

    bool isClick(const QPointF &pos) const;
    bool isSlide(const QPointF &pos) const;
    bool isHorSlide(const QPointF &pos);
    bool isVerSlide(const QPointF &pos);

    inline bool pressed() const;
    inline const QPointF &pos() const;
    inline Qt::MouseButton button() const;

signals:
    void clickDelayElapsed(const QPointF &pos);
    void doubleClicked(const QPointF &pos);

private:
    bool _pressed;
    QPointF _pressedAtPos, _nextPressedAtPos;
    qint64 _pressedAtMSecs;

    bool _horSlide, _verSlide;
    QTimer _clickTimer;

private slots:
    void emitClickDelayElapsed();

/*!
 * configure constants
 */
private:
    static qint64 _slideDelay;
    static int _doubleClickDelay;
    static int _slideDistance;
    static int _updateDistance;
};


inline bool MousePressHelper::pressed() const { return _pressed;}
inline const QPointF &MousePressHelper::pos() const { return _pressedAtPos;}


#endif // MOUSEPRESSHELPER_H
