#ifndef GANTTCURRENTTIMESLIDER_H
#define GANTTCURRENTTIMESLIDER_H

#include "utcdatetime.h"


#include <QGraphicsObject>

class GanttHeader;
class GanttScene;

class GanttSlider : public QGraphicsObject
{
    Q_OBJECT

public:
    GanttSlider(QGraphicsItem * parent = 0);

    void setScene(GanttScene* scene);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;

    void updateShape();
    void setSlidersRect(const QRectF &slidersRect);

    void setPos(const QPointF &pos);
    void setPos(qreal x, qreal y);

    UtcDateTime dt() const;

    bool initialized() const;
    qreal relativePos() const;

    UtcDateTime minDt() const;
    UtcDateTime maxDt() const;


protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);



signals:
    void dtChanged(UtcDateTime dt);

public slots:
    void makeStep(long long deltaVal);
    bool setDt(UtcDateTime dt);
    void updateScenePos();
    void updateRange(const UtcDateTime& minDt, const UtcDateTime& maxDt);

    void moveToBegin();
    void moveToEnd();

    void moveToRangeStart();
    void moveToRangeFinish();

private:

    bool outOfRange() const;
    bool outOfBounds(const UtcDateTime &dt) const;

private:
    QRectF m_slidersRect; ///< The rectangle, where slider can actually move.

    QPainterPath m_sliderShape;
    QPainterPath m_rhombus;

    qreal m_penWidth;

    UtcDateTime m_dt;
    UtcDateTime m_minDt,m_maxDt;

    GanttScene* m_scene;

    bool m_initialized;
};

#endif // GANTTCURRENTTIMESLIDER_H
