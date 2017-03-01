#ifndef GANTTCALCGRAPHICSITEM_H
#define GANTTCALCGRAPHICSITEM_H

#include "gantttextgraphicsobject.h"
#include <QPainterPath>

#include <QPointer>

class GanttHeader;
class GanttScene;

class GanttCalcGraphicsObject : public GanttTextGraphicsObject
{
    Q_OBJECT
    void init();
public:
    GanttCalcGraphicsObject(GanttInfoItem* node,QGraphicsItem *parent = NULL);

    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QString textRight() const;

    virtual int inactiveZValue() const{
        return 500;
    }

public slots:
    void updateItemGeometry();
    void updateToolTip();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);


private:
    QPixmap _pixmapImageActive, _pixmapImageInactive;
    static QRect _geometry;
    static QPainterPath _geometryPath;
    friend QPainterPath initializeGeometryPath();
};

#endif // GANTTCALCGRAPHICSITEM_H
