#ifndef KAMEMORYPICKER_H
#define KAMEMORYPICKER_H

#include "kamemoryabstractpart.h"
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>



class KaMemoryPicker : public KaMemoryAbstractPart
{
    Q_OBJECT
private:


//    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event);
//    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event);
//    void hoverMoveEvent ( QGraphicsSceneHoverEvent * event);

    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event);
    void mousePressEvent ( QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event);
public:
    explicit KaMemoryPicker(QGraphicsItem *parent = 0);
    ~KaMemoryPicker();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // KAMEMORYPICKER_H
