#include "ganttgraphicsobject.h"
#include "ganttscene.h"

#include "ganttheader.h"
#include "ganttinfonode.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

GanttGraphicsObject::GanttGraphicsObject(GanttInfoLeaf *info,QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    m_scene = NULL;
    m_header = NULL;

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
    return QRectF(QPointF(0,0),m_boundingRectSize);
}

void GanttGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if(!m_info || !m_scene)
        return;
    QColor color = (m_scene->currentItem() == this)?(m_info->getColor().darker(130)):(m_info->getColor());
    QRectF drawRect = boundingRect().adjusted(0,5,0,-5);
    painter->fillRect(drawRect,QBrush(color));
    painter->drawRect(drawRect);
}

void GanttGraphicsObject::setScene(GanttScene *scene)
{
    m_scene = scene;
    m_scene->addItem(this);
}
void GanttGraphicsObject::setBoundingRectSize(const QSizeF &boundingRectSize)
{
    prepareGeometryChange();
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





