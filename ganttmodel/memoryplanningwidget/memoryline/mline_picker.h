#ifndef MLINE_PICKER_H
#define MLINE_PICKER_H

#include "mline_abstractpart.h"
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>



class MLinePicker : public MLineAbstractPart
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
    explicit MLinePicker(QGraphicsItem *parent = 0);
    ~MLinePicker();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // MLINE_PICKER_H
