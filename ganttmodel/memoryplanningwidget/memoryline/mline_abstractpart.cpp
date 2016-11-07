#include "mline_abstractpart.h"

#include <QPainter>
#include <QDebug>
#include <QCursor>
#include "QGraphicsScene"

#include "mline_scene.h"

int MLineAbstractPart::begin() const
{
    return _begin;
}

void MLineAbstractPart::setBegin(int begin)
{
    if(memorySize()!=0 && scene()!=0)
    {
//        if(_end-begin<0)
//            return;

        if(begin>memorySize())
            return;

        QRectF oldRec = boundingRect();

        if(begin>=0)
        {
            _begin= begin;
            setX(xByVal(_begin));

            emit beginChanged(_begin);
            emit valueChanged(_begin,_end);
        }
        else
        {
            if(_end+_begin-begin<memorySize())
            {
                emit beginChanged(_begin);
                emit valueChanged(_begin,_end);
                setEnd(_end+_begin-begin);
            }
        }
        update(oldRec);
    }
}

int MLineAbstractPart::end() const
{
    return _end;
}

void MLineAbstractPart::setEnd(int end)
{

    if(memorySize()!=0 && scene()!=0)
    {
//        if(end-_begin<0)
//            return;

        if(end<0)
            return;


        QRectF oldRec = boundingRect();

        if(end <= memorySize())
        {
            _end = end;
            emit endChanged(_end);
            emit valueChanged(_begin,_end);
        }
        else
        {
            if(_begin-end+_end >0)
            {
                emit endChanged(_end);
                emit valueChanged(_begin,_end);
                setBegin(_begin-end+_end);
            }
        }
        update(oldRec);
    }
}

int MLineAbstractPart::size() const
{
    return _end-_begin;
}

qreal MLineAbstractPart::width() const
{
    if(memorySize()==0  || scene()==0)
        return 0;

    return xByVal(_end)-xByVal(_begin);
}

int MLineAbstractPart::memorySize() const
{
    if(scene()==0)
        return 0;

    MLineScene * sc = dynamic_cast<MLineScene*>(scene());
    if(sc == 0)
    {
        return 0;
    }

     return sc->memory()->memorySize();

}

qreal MLineAbstractPart::xByVal(int val) const
{
    if(memorySize()!=0 && scene()!=0)
    {
        return ((val*(scene()->width()-2*dx_border))/memorySize())+dx_border;
    }

    return 0;
}

int MLineAbstractPart::valByX(qreal x) const
{
    if(memorySize()!=0 && scene()!=0)
    {
        return ((x-dx_border)*memorySize())/(scene()->width()-2*dx_border);
    }
    return 0;
}




MLineAbstractPart::MLineAbstractPart(QGraphicsItem *parent):
    QGraphicsObject(parent)
{
    qDebug() << "MLineAbstractPart";
    _begin=0;
    _end=0;
    y_pos=10;
    height=20;
    setY(y_pos);
}

MLineAbstractPart::MLineAbstractPart(const MLineAbstractPart &part):
    QGraphicsObject(part.parentItem())
{
    _begin=part._begin;
    _end=part._end;
    y_pos=10;
    height=20;
    setY(y_pos);
}

MLineAbstractPart::~MLineAbstractPart()
{

}

QRectF MLineAbstractPart::boundingRect() const
{
    return QRectF(0,0, width(), height+5);
}
