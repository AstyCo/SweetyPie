#ifndef GANTTGRAPHICSITEM_H
#define GANTTGRAPHICSITEM_H

#include "gantttextgraphicsobject.h"
#include "extensions/intervalunion.h"
#include "extensions/qrectunion.h"

#include <QPointer>

class GanttIntervalGraphicsObject : public GanttTextGraphicsObject
{
    Q_OBJECT

    void init();
public:
    GanttIntervalGraphicsObject(GanttInfoItem *info,QGraphicsItem *parent = 0);

//    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QString textRight() const;

    virtual int inactiveZValue() const{
        if(!_isSmall)
            return 1;
        else
            return 600;
    }

public slots:
    void setBoundingRectSize(const QSizeF &boundingRectSize);

    void updateItemGeometry();
    void updateToolTip();
    void updateIntersection();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    void drawSmallInterval(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void updateVisualIntersection();


    bool _isSmall;

    QSizeF _boundingRectSize;

//    QSizeF _lastBoundingRectSize;
    QRectFUnion _intersectionVisual;
    IntervalUnion _intersection;

    static QPainterPath _circlePath;
};

#endif // GANTTGRAPHICSITEM_H
