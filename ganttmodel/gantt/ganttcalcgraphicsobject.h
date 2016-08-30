#ifndef GANTTCALCGRAPHICSITEM_H
#define GANTTCALCGRAPHICSITEM_H

#include <QGraphicsObject>
#include <QPainterPath>

class GanttHeader;
class GanttScene;
class GanttInfoNode;

class GanttCalcGraphicsObject : public QGraphicsObject
{
    Q_OBJECT

public:
    GanttCalcGraphicsObject(GanttInfoNode* node,QGraphicsItem *parent = NULL);
    ~GanttCalcGraphicsObject();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setScene(GanttScene *scene);
    void setHeader(GanttHeader *header);

    GanttInfoNode *info() const;

signals:
    void graphicsItemPressed();
    void graphicsItemHoverEnter();
    void graphicsItemHoverLeave();

public slots:
    void updateItemGeometry();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    QPainterPath m_shapePath;
    GanttHeader* m_header;
    GanttInfoNode* m_info;
    GanttScene *m_scene;
};

#endif // GANTTCALCGRAPHICSITEM_H
