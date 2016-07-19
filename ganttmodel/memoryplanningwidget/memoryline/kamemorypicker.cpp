#include "kamemorypicker.h"

#include <QPainter>
#include <QDebug>
#include <QCursor>
#include "QGraphicsScene"

#include "kamemory.h"

//void KaMemoryPicker::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
//{
//    setCursor(Qt::SizeHorCursor);
//    update();
//}

//void KaMemoryPicker::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
//{
//    setCursor(Qt::ArrowCursor);
//    update();
//}

//void KaMemoryPicker::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
//{
////    if((event->pos().x() >= (boundingRect().right()-5)) || (event->pos().x() <= (boundingRect().left()+5)))
////        setCursor(Qt::SizeHorCursor);
////    else
////        if((event->pos().x() < (boundingRect().right()-5)) && (event->pos().x() > (boundingRect().left()+5)))
//            setCursor(Qt::OpenHandCursor);
//            setPos(QPointF(event->pos().x(),10));
//            //QGraphicsItem::hoverMoveEvent(event);
//            emit valueChanged(begin(),end());
//}

void KaMemoryPicker::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    if(cursor().shape() == Qt::SizeHorCursor)
//    {
//        //setWidth(event->pos().x());
//        if((event->pos().x() <= (boundingRect().left()+15)))
//        {
//            qreal begX = scenePos().x();
//            setX(event->scenePos().x());
//            setWidth(width() + (begX - x()));
//              emit valueChanged(begin(),end());
//        }
//        else if(event->pos().x() > (boundingRect().right()-15))
//        {
//            setWidth(event->pos().x());
//             emit valueChanged(begin(),end());
//        }
//    }
//    else
      if(memorySize()==0  || scene()==0)
          return;

        if(cursor().shape() == Qt::ClosedHandCursor)
        {
            int tmpmemsize = size();
            qreal tmpwidth = width();
            if(mapToScene(event->pos()).x()<dx_border)
                setPos(QPointF(dx_border,y_pos));
            else if(mapToScene(event->pos()).x()+tmpwidth>scene()->width()-dx_border)
                setPos(QPointF(scene()->width()-dx_border-tmpwidth,y_pos));
            else
                setPos(QPointF(mapToScene(event->pos()).x(),y_pos));

            _begin = valByX(x());
            _end = _begin+tmpmemsize;
            emit beginChanged(_begin);
            emit endChanged(_end);
            emit valueChanged(_begin,_end);
        }
}

void KaMemoryPicker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if(cursor().shape() == Qt::OpenHandCursor)
        setCursor(Qt::ClosedHandCursor);
}

void KaMemoryPicker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    mouseMoveEvent(event);
    setCursor(Qt::OpenHandCursor);
}


KaMemoryPicker::KaMemoryPicker(QGraphicsItem *parent):
    KaMemoryAbstractPart(parent)
{
  //  setFlags(QGraphicsItem::ItemIsMovable);
    setCursor(Qt::OpenHandCursor);
    setZValue(1);
    setX(dx_border);
}

KaMemoryPicker::~KaMemoryPicker()
{

}

void KaMemoryPicker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPen myPen;
    myPen.setColor(QColor(Qt::red));
    myPen.setWidth(3);
    painter->setPen(myPen);
    setX(xByVal(begin()));
    QRectF rect(0,0, width(), height);
    painter->drawRect(rect);
    QColor testColor(Qt::white);
    testColor.setAlpha(127);
    painter->fillRect(rect, testColor);

    painter->drawLine(rect.left(), rect.bottom(), rect.left(), rect.bottom()+5);

    painter->setPen(myPen);
    painter->drawLine(rect.right(), rect.bottom(), rect.right(), rect.bottom()+5);


    painter->drawText(rect, Qt::AlignCenter, QString::number(size())/* + QObject::trUtf8(" КБ") */);
}

