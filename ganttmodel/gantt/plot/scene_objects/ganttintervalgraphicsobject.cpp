#include "ganttintervalgraphicsobject.h"
#include "gantt/plot/ganttscene.h"
#include "gantt/private_extensions/gantt-lib_global_values.h"
#include "gantt/info/ganttinfonode.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#define MIN_VIS_WIDTH 8
#define RECTANGLE_OFFSET 5

GanttIntervalGraphicsObject::GanttIntervalGraphicsObject(GanttInfoLeaf *info,QGraphicsItem *parent) :
    GanttGraphicsObject(info,parent)
{
    _scene = NULL;
    _isSmall = true;

    if(innerInfo())
    {
        setToolTip( QString::fromUtf8("Операция:") + '\t' + '\t' + innerInfo()->title()
                    + '\n' + QString::fromUtf8("Начало:") + '\t' + innerInfo()->start().toString("dd.MM.yyyy HH:mm:ss")
                    + '\n' + QString::fromUtf8("Окончание:") + '\t' + innerInfo()->finish().toString("dd.MM.yyyy HH:mm:ss"));
        connect(innerInfo(), SIGNAL(changed()),this,SLOT(updateItemGeometry()));

        innerInfo()->increaseLinkCnt();
    }

    setAcceptHoverEvents(true);
}

GanttIntervalGraphicsObject::~GanttIntervalGraphicsObject()
{

    if(_scene)
    {
        _scene->removeItem(this);
        setParentItem(NULL);
    }

    if(innerInfo())
        innerInfo()->reduceLinkCnt();
}

QRectF GanttIntervalGraphicsObject::boundingRect() const
{
    if(!_isSmall)
        return QRectF(QPointF(0, RECTANGLE_OFFSET),
                      QSize(_boundingRectSize.width(), _boundingRectSize.height() - 2 * RECTANGLE_OFFSET));
    return QRectF(QPointF( -MIN_VIS_WIDTH/2 - 1, RECTANGLE_OFFSET),
                  QPointF(_boundingRectSize.width()+ MIN_VIS_WIDTH/2 + 1,_boundingRectSize.height() - RECTANGLE_OFFSET));
}

QPainterPath GanttIntervalGraphicsObject::shape() const
{
    QPainterPath path;
    path.addRect(QRect(QPoint(0, RECTANGLE_OFFSET),
                  QSize(_boundingRectSize.width(), _boundingRectSize.height() - 2 * RECTANGLE_OFFSET)));
    return path;
}

void GanttIntervalGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if(!innerInfo() || !_scene)
        return;
    QColor color = (_scene->currentItem() == this)?(innerInfo()->color().darker(130)):(innerInfo()->color());

    painter->setPen(QPen(QColor(0x644C26)));
    if(!_isSmall)
    {
        QRectF drawRect = boundingRect();
        painter->fillRect(drawRect,QBrush(color));
        painter->drawRect(drawRect);

        painter->save();
        painter->setOpacity(0.5);

        if(!_intersectionVisual.isEmpty())
            _intersectionVisual.fillRects(painter, QBrush(Qt::black));

        painter->restore();

//        if(!_intersection.isEmpty())
//            _intersection.fillRects(painter, QBrush(Qt::black));

    }
    else
    {
        painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing | QPainter::Antialiasing);
        QRectF drawRect = QRectF(QPointF(0,0),_boundingRectSize).adjusted(0,RECTANGLE_OFFSET,0,-RECTANGLE_OFFSET);
        painter->fillRect(drawRect,QBrush(color));
        painter->drawRect(drawRect);
        QPainterPath path;

        path.moveTo(-MIN_VIS_WIDTH/2,   drawRect.top());
        path.lineTo(-1,                 drawRect.top() + drawRect.height()/2);
        path.lineTo(-MIN_VIS_WIDTH/2,   drawRect.bottom());
        path.lineTo(-MIN_VIS_WIDTH/2,   drawRect.top());

//        path.moveTo(m_boundingRectSize.width(),
//                                        drawRect.top() + drawRect.height()/2);
        path.moveTo(_boundingRectSize.width() + MIN_VIS_WIDTH/2,
                                        drawRect.top());
        path.lineTo(_boundingRectSize.width() + 1,
                                        drawRect.top() + drawRect.height()/2);
        path.lineTo(_boundingRectSize.width() + MIN_VIS_WIDTH/2,
                                        drawRect.bottom());
        path.lineTo(_boundingRectSize.width() + MIN_VIS_WIDTH/2,
                    drawRect.top());

        painter->setOpacity(0.5);
        painter->fillPath(path,QBrush(Qt::black));
        painter->setOpacity(1);

    }
}

GanttInfoLeaf *GanttIntervalGraphicsObject::innerInfo() const
{
    return qobject_cast<GanttInfoLeaf*>(m_info);
}


void GanttIntervalGraphicsObject::setBoundingRectSize(const QSizeF &boundingRectSize)
{
    prepareGeometryChange();
    if(boundingRectSize.width()>MIN_VIS_WIDTH){
        if(_isSmall){
            _isSmall = false;
        }
    }
    else if(!_isSmall)
        _isSmall = true;

    const bool needUpdateVisualIntersection = boundingRectSize.width() != _boundingRectSize.width();
    _boundingRectSize = boundingRectSize;

    if(needUpdateVisualIntersection)
        updateVisualIntersection();
}

void GanttIntervalGraphicsObject::updateItemGeometry()
{
    if(!innerInfo())
        return;

    qreal startPos = _dtline->dtToPos(innerInfo()->start()),
          itemWidth = _dtline->dtToPos(innerInfo()->finish()) - startPos;

    // UPDATE geometry
    setPos(startPos, innerInfo()->calcPos());
    setBoundingRectSize(QSizeF(itemWidth, DEFAULT_ITEM_HEIGHT));
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
    qDebug() << "Hi, My boundingRect:" << boundingRect() << "My _intersection:"<<_intersection;
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






