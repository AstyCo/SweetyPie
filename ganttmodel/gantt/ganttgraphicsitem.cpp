#include "ganttgraphicsitem.h"
#include "ganttscene.h"

#include "ganttheader.h"
#include "ganttinfonode.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

GanttGraphicsItem::GanttGraphicsItem(GanttInfoLeaf *info,QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    m_scene = NULL;
    m_header = NULL;

    m_info = info;
    if(m_info)
    {
        setToolTip( QString::fromUtf8("Имя:") + '\t' + '\t' + m_info->title()
                    + '\n' + QString::fromUtf8("Начало:") + '\t' + m_info->start().toString("dd.MM.yyyy HH:mm:ss")
                    + '\n' + QString::fromUtf8("Окончание:") + '\t' + m_info->finish().toString("dd.MM.yyyy HH:mm:ss"));
        connect(m_info, SIGNAL(changed()),this,SLOT(updateItemGeometry()));

        m_info->increaseLinkCnt();
    }

    setAcceptHoverEvents(true);
}

GanttGraphicsItem::~GanttGraphicsItem()
{

    if(m_scene)
    {
        m_scene->removeItem(this);
        setParentItem(NULL);
        m_scene->update(rect());
        if(m_scene->dtItems().isEmpty())
        {
            m_scene->setEmpty(true);
            m_scene->updateSceneRect();
        }
    }

    if(m_info)
        m_info->reduceLinkCnt();
}

QRectF GanttGraphicsItem::boundingRect() const
{
    return QRectF(QPointF(0,0),m_boundingRectSize);
}

void GanttGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if(!m_info)
        return;

    QRectF drawRect = boundingRect().adjusted(0,5,0,-5);
    painter->fillRect(drawRect,QBrush(m_info->getColor()));
    painter->drawRect(drawRect);
}

void GanttGraphicsItem::setScene(GanttScene *scene)
{
    m_scene = scene;
    m_scene->addItem(this);
}
void GanttGraphicsItem::setBoundingRectSize(const QSizeF &boundingRectSize)
{
    prepareGeometryChange();
    m_boundingRectSize = boundingRectSize;
}

void GanttGraphicsItem::updateItemGeometry()
{
    if(!m_header || !m_info)
        return;

    qreal startPos = m_header->dtToX(m_info->start()),
          itemWidth = m_header->dtToX(m_info->finish()) - startPos;

    setBoundingRectSize(QSizeF(itemWidth, DEFAULT_ITEM_WIDTH));
    setPos(startPos, 2*DEFAULT_ITEM_WIDTH + m_info->pos());
}
GanttInfoLeaf *GanttGraphicsItem::info() const
{
    return m_info;
}

QRectF GanttGraphicsItem::rect() const
{
    return QRectF(pos(),m_boundingRectSize);
}

void GanttGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    if(!m_scene || !m_info)
        return;

    if(m_info->parent() && (this == m_scene->itemAt(event->scenePos())))
        m_scene->changeExpanding(m_info->parent()->index());

    emit graphicsItemPressed();

    QGraphicsItem::mousePressEvent(event);
}

void GanttGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit graphicsItemHoverEnter();

    QGraphicsItem::hoverEnterEvent(event);
}

void GanttGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    emit graphicsItemHoverLeave();

    QGraphicsItem::hoverLeaveEvent(event);
}

void GanttGraphicsItem::setHeader(GanttHeader *header)
{
    m_header = header;
}





