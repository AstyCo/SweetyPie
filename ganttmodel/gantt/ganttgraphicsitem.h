#ifndef GANTTGRAPHICSITEM_H
#define GANTTGRAPHICSITEM_H

#include "ganttinfoleaf.h"


#include <QGraphicsObject>

class GanttHeader;
class GanttScene;

class GanttGraphicsItem : public QGraphicsObject
{
    Q_OBJECT

public:
    GanttGraphicsItem(GanttInfoLeaf *info,QGraphicsItem *parent = 0);
    virtual ~GanttGraphicsItem();

    //
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);



    GanttInfoLeaf *info() const;

    QRectF rect() const;

    void setHeader(GanttHeader *header);

signals:
    void graphicsItemPressed();
    void graphicsItemHoverEnter();
    void graphicsItemHoverLeave();


public slots:
    void setScene(GanttScene *scene);
    void setBoundingRectSize(const QSizeF &boundingRectSize);

    void updateItemGeometry();



protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:

    GanttHeader* m_header;
    GanttInfoLeaf* m_info;
    GanttScene *m_scene;

    QSizeF m_boundingRectSize;


};

#endif // GANTTGRAPHICSITEM_H
