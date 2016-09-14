#include "ganttgraphicsobject.h"
#include "ganttscene.h"

#include "ganttheader.h"
#include "ganttinfonode.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#define MIN_VIS_WIDTH 8

GanttGraphicsObject::GanttGraphicsObject(GanttInfoLeaf *info,QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    m_scene = NULL;
    m_header = NULL;
    m_isSmall = true;

    m_info = info;
    if(m_info)
    {
        setToolTip( QString::fromUtf8("Операция:") + '\t' + '\t' + m_info->title()
                    + '\n' + QString::fromUtf8("Начало:") + '\t' + m_info->start().toString("dd.MM.yyyy HH:mm:ss")
                    + '\n' + QString::fromUtf8("Окончание:") + '\t' + m_info->finish().toString("dd.MM.yyyy HH:mm:ss"));
        connect(m_info, SIGNAL(changed()),this,SLOT(updateItemGeometry()));

        m_info->increaseLinkCnt();
    }

    setAcceptHoverEvents(true);
}

GanttGraphicsObject::~GanttGraphicsObject()
{

    if(m_scene)
    {
        m_scene->removeItem(this);
        setParentItem(NULL);
        m_scene->update(sceneBoundingRect());
    }

    if(m_info)
        m_info->reduceLinkCnt();
}

QRectF GanttGraphicsObject::boundingRect() const
{
    if(!m_isSmall)
        return QRectF(QPointF(0,0),m_boundingRectSize);
    return QRectF(QPointF(-MIN_VIS_WIDTH/2 - 1,0),
                  QPointF(m_boundingRectSize.width()+MIN_VIS_WIDTH/2 + 1,m_boundingRectSize.height()));
}

void GanttGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if(!m_info || !m_scene)
        return;
    QColor color = (m_scene->currentItem() == this)?(m_info->getColor().darker(130)):(m_info->getColor());

    if(!m_isSmall)
    {
        QRectF drawRect = boundingRect().adjusted(0,5,0,-5);
        painter->fillRect(drawRect,QBrush(color));
        painter->drawRect(drawRect);
    }
    else
    {
        painter->setRenderHint(QPainter::Antialiasing,true);
        QRectF drawRect = QRectF(QPointF(0,0),m_boundingRectSize).adjusted(0,5,0,-5);
        painter->fillRect(drawRect,QBrush(color));
        painter->drawRect(drawRect);
        QPainterPath path;

        path.moveTo(-MIN_VIS_WIDTH/2,   drawRect.top());
        path.lineTo(-1,                  drawRect.top() + drawRect.height()/2);
        path.lineTo(-MIN_VIS_WIDTH/2,   drawRect.bottom());
        path.lineTo(-MIN_VIS_WIDTH/2,   drawRect.top());

//        path.moveTo(m_boundingRectSize.width(),
//                                        drawRect.top() + drawRect.height()/2);
        path.moveTo(m_boundingRectSize.width() + MIN_VIS_WIDTH/2,
                                        drawRect.top());
        path.lineTo(m_boundingRectSize.width() + 1,
                                        drawRect.top() + drawRect.height()/2);
        path.lineTo(m_boundingRectSize.width() + MIN_VIS_WIDTH/2,
                                        drawRect.bottom());
        path.lineTo(m_boundingRectSize.width() + MIN_VIS_WIDTH/2,
                    drawRect.top());

        painter->setOpacity(0.5);
        painter->fillPath(path,QBrush(Qt::black));
        painter->setOpacity(1);

    }
}

void GanttGraphicsObject::setScene(GanttScene *scene)
{
    m_scene = scene;
    m_scene->addItem(this);
}
void GanttGraphicsObject::setBoundingRectSize(const QSizeF &boundingRectSize)
{
    prepareGeometryChange();
    if(boundingRectSize.width()>MIN_VIS_WIDTH)
        m_isSmall = false;
    else
        m_isSmall = true;

    m_boundingRectSize = boundingRectSize;
}

void GanttGraphicsObject::updateItemGeometry()
{
    if(!m_header || !m_info)
        return;

    qreal startPos = m_header->dtToX(m_info->start()),
          itemWidth = m_header->dtToX(m_info->finish()) - startPos;

    setBoundingRectSize(QSizeF(itemWidth, DEFAULT_ITEM_WIDTH));
    setPos(startPos, 2*DEFAULT_ITEM_WIDTH + m_info->pos());
}

GanttInfoLeaf *GanttGraphicsObject::info() const
{
    return m_info;
}

void GanttGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    if(!m_scene || !m_info)
//        return;

//    if(m_info->parent() && (this == m_scene->itemAt(event->scenePos())))
//        if((!m_info->parent()->isExpanded() && (event->button() == Qt::LeftButton))
//                /*|| (m_info->parent()->isExpanded() && (event->button() == Qt::RightButton))*/)
//        {
//            m_scene->changeExpanding(m_info->parent()->index());
//            emit graphicsItemPressed();
//        }

    QGraphicsItem::mousePressEvent(event);
}

void GanttGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit graphicsItemHoverEnter();

    QGraphicsItem::hoverEnterEvent(event);
}

void GanttGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    emit graphicsItemHoverLeave();

    QGraphicsItem::hoverLeaveEvent(event);
}

void GanttGraphicsObject::setHeader(GanttHeader *header)
{
    m_header = header;
}





