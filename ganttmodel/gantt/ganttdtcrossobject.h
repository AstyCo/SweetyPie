#ifndef GANTTDTCROSSOBJECT_H
#define GANTTDTCROSSOBJECT_H

#include <QGraphicsObject>
#include <QPainterPath>
#include <QPointer>

class GanttScene;

class GanttDtCrossObject : public QGraphicsObject
{
public:
    GanttDtCrossObject(QGraphicsItem *parent = NULL);

    QRectF boundingRect() const;
//    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setScene(const QPointer<GanttScene> &scene);

    void setPos(const QPointF &pos);
    void setPos(qreal x, qreal y);
    void updatePaths();

protected:

private:
    QPainterPath m_linesPath;
    QRect m_textRect;
    QString m_text;

    QPointer<GanttScene> m_scene;

};

#endif // GANTTDTCROSSOBJECT_H
