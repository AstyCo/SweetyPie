#include "ganttintervalgraphicsobject.h"
#include "gantt/plot/ganttscene.h"
#include "gantt/private_extensions/gantt-lib_global_values.h"
#include "gantt/info/ganttinfonode.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>


QPainterPath initializePath(){
    QPainterPath res;
    qreal radius = DEFAULT_ITEM_HEIGHT/2 - RECTANGLE_OFFSET;
    res.addEllipse(QPointF(0 , RECTANGLE_OFFSET + radius),
                   radius/1.3, radius);
    return res;
}

QPainterPath GanttIntervalGraphicsObject::_circlePath = initializePath();

void GanttIntervalGraphicsObject::init()
{
    _isSmall = true;

    if(info()){

        updateToolTip();
        connect(info(), SIGNAL(changed()),this,SLOT(updateItemGeometry()));
        connect(info(), SIGNAL(changed()),this,SLOT(updateToolTip()));

        info()->increaseLinkCnt();
    }

    setZValue(1);
    setAcceptHoverEvents(true);
}

GanttIntervalGraphicsObject::GanttIntervalGraphicsObject(GanttInfoLeaf *info,QGraphicsItem *parent) :
    GanttTextGraphicsObject(info,parent)
{
    init();
}


//QRectF GanttIntervalGraphicsObject::boundingRect() const
//{
//    if(!_isSmall)
//        return QRectF(QPointF(0, RECTANGLE_OFFSET),
//                      QSize(_boundingRectSize.width(), _boundingRectSize.height() - 2 * RECTANGLE_OFFSET));
//    return QRectF(QPointF( -MIN_VIS_WIDTH/2 - 1, RECTANGLE_OFFSET),
//                  QPointF(_boundingRectSize.width()+ MIN_VIS_WIDTH/2 + 1,_boundingRectSize.height() - RECTANGLE_OFFSET));
//}

QPainterPath GanttIntervalGraphicsObject::shape() const
{
    if(!_isSmall){
        QPainterPath path;
        path.addRect(QRect(QPoint(0, RECTANGLE_OFFSET),
                           QSize(_boundingRectSize.width(), _boundingRectSize.height() - 2 * RECTANGLE_OFFSET)));
        return path;    // Not small shape - rectangle
    }

    return _circlePath; // Small shape - circle
}

void GanttIntervalGraphicsObject::paint(QPainter *painter,
                                        const QStyleOptionGraphicsItem *option,
                                        QWidget *widget)
{
    if(!info())
        return;

    if(!_isSmall)
    {
        QColor color = (_current ? info()->color().darker(130)
                                : info()->color());
        painter->setPen(QPen(QColor(0x644C26)));
        QRectF drawRect = shape().controlPointRect();
        painter->fillRect(drawRect,QBrush(color));
//        painter->drawRect(drawRect);

        painter->save();
        painter->setOpacity(0.4);

        foreach(const QRectF &rect, _intersectionVisual.rects())
            painter->fillRect(rect.adjusted(0,5,0,0), QBrush(Qt::black));

        painter->restore();
    }
    else
        drawSmallInterval(painter, option, widget);

    GanttTextGraphicsObject::paint(painter, option, widget);
}


QString GanttIntervalGraphicsObject::textRight() const
{
    GanttInfoNode *parent = info()->parent();
    if(parent && parent->isExpanded())
        return info()->title();
    return QString();
}


void GanttIntervalGraphicsObject::setBoundingRectSize(const QSizeF &boundingRectSize)
{
    prepareGeometryChange();
    if(boundingRectSize.width()>MIN_VIS_WIDTH)
        _isSmall = false;
    else
        _isSmall = true;
    updateZValue();

    const bool needUpdateVisualIntersection = boundingRectSize.width() != _boundingRectSize.width();
    _boundingRectSize = boundingRectSize;

    if(needUpdateVisualIntersection)
        updateVisualIntersection();
}

void GanttIntervalGraphicsObject::updateItemGeometry()
{
    if(!info())
        return;

    qreal startPos = _dtline->dtToPos(info()->start()),
          itemWidth = _dtline->dtToPos(info()->finish()) - startPos;

    // UPDATE geometry
    setPos(startPos, info()->calcPos());
    setBoundingRectSize(QSizeF(itemWidth, DEFAULT_ITEM_HEIGHT));
}

void GanttIntervalGraphicsObject::updateToolTip()
{
    setToolTip( QString::fromUtf8("Операция:") + '\t' + '\t' + info()->title()
                + '\n' + QString::fromUtf8("Начало:") + '\t' + info()->start().toString("dd.MM.yyyy HH:mm:ss")
                + '\n' + QString::fromUtf8("Окончание:") + '\t' + info()->finish().toString("dd.MM.yyyy HH:mm:ss"));
}


void GanttIntervalGraphicsObject::updateIntersection()
{
    _intersection.clear();
    GanttInfoNode *parentNode = info()->parent();
//    qDebug() << "updateIntersection " << info()->title() << parentNode->isExpanded();
    if(parentNode && !parentNode->isExpanded()){
        MyUtcDateTimeInterval thisInterval = info()->getInterval();
        for(int i = 0; i < parentNode->size(); ++i){
            if(i == info()->row())
                continue; // == this

            _intersection |= thisInterval.intersection(parentNode->at(i)->getInterval());
//            qDebug() << "add interval" << thisInterval.intersection(parentNode->at(i)->getInterval());
        }
    }
    updateVisualIntersection();
}

void GanttIntervalGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Hi, My boundingRect:" << boundingRect()
             << "My _intersection:"<<_intersection
             << "My _current:" << _current;
    QGraphicsItem::mousePressEvent(event);
}

void GanttIntervalGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{

    emit graphicsItemHoverEnter();

    QGraphicsItem::hoverEnterEvent(event);
}

void GanttIntervalGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    emit graphicsItemHoverLeave();

    QGraphicsItem::hoverLeaveEvent(event);
}

void GanttIntervalGraphicsObject::drawSmallInterval(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    QColor color = (_current ? info()->color().darker(130)
                            : info()->color());

    painter->setRenderHints(QPainter::HighQualityAntialiasing | QPainter::Antialiasing);
    painter->drawPath(_circlePath);
    painter->fillPath(_circlePath,QBrush(color));

    if(!_intersection.isEmpty()){
        QPainterPath smallIntersectionPath;
        QRectF tmp = _circlePath.controlPointRect();

        tmp.adjust(0, tmp.height() / 2, 0, 0);
        smallIntersectionPath.addRect(tmp);
        smallIntersectionPath &= _circlePath;

        painter->save();
        painter->setOpacity(0.4);

        painter->fillPath(smallIntersectionPath, QBrush(Qt::black));

        painter->restore();

    }
}

void GanttIntervalGraphicsObject::updateVisualIntersection()
{
    _intersectionVisual.clear();
    foreach(const MyUtcDateTimeInterval &interval, _intersection.intervals()){
        int beginPos = _dtline->dtToPos(interval.min()),
            rectWidth = _dtline->dtToPos(interval.end()) - beginPos;
        beginPos -= scenePos().x();

        _intersectionVisual |= QRectF(beginPos, RECTANGLE_OFFSET ,
                                      rectWidth, _boundingRectSize.height() - 2 * RECTANGLE_OFFSET);
    }
}


//void GanttIntervalGraphicsObject::updateIntersectionRelative(const qreal &itemWidth)
//{
//    if(!_intersection.isEmpty()){
//        qreal coef = itemWidth / _lastBoundingRectSize.width();

//        qDebug() << info()->title() << "updateRelative";
//        qDebug()<<"last"<<_lastBoundingRectSize.width()<<"new"<<itemWidth;
//        _intersection *= coef;

//        _lastBoundingRectSize.setWidth(itemWidth);
//    }
//}






