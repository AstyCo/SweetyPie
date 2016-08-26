#ifndef GANTTCALCGRAPHICSITEM_H
#define GANTTCALCGRAPHICSITEM_H

#include <QGraphicsItem>

class GanttCalcGraphicsItem : public QGraphicsItem
{
public:
    GanttCalcGraphicsItem(QGraphicsItem *parent = NULL);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // GANTTCALCGRAPHICSITEM_H
