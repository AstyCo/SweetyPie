#ifndef GANTTGRAPHICSITEM_H
#define GANTTGRAPHICSITEM_H

#include "ganttgraphicsobject.h"
#include "gantt/info/ganttinfoleaf.h"
#include "extensions/intervalunion.h"
#include "extensions/qrectunion.h"

#include <QPointer>

class GanttIntervalGraphicsObject : public GanttGraphicsObject
{
    Q_OBJECT

public:
    GanttIntervalGraphicsObject(GanttInfoLeaf *info,QGraphicsItem *parent = 0);
    virtual ~GanttIntervalGraphicsObject();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    GanttInfoLeaf *innerInfo() const;

public slots:
    void setBoundingRectSize(const QSizeF &boundingRectSize);

    void updateItemGeometry();
    void updateIntersection();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    void updateVisualIntersection();


    bool _isSmall;

    QSizeF _boundingRectSize;

//    QSizeF _lastBoundingRectSize;
    QRectFUnion _intersectionVisual;
    IntervalUnion _intersection;

    static QPainterPath _circlePath;
};

#endif // GANTTGRAPHICSITEM_H
