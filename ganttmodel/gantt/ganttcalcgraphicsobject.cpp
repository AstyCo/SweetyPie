#include "ganttcalcgraphicsobject.h"
#include "ganttglobalvalues.h"

#include "ganttscene.h"
#include "ganttheader.h"
#include "ganttinfonode.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>

GanttCalcGraphicsObject::GanttCalcGraphicsObject(GanttInfoNode* node, QGraphicsItem *parent)
    :QGraphicsObject(parent)
{
    m_shapePath.moveTo(0,0);
    m_shapePath.lineTo(0,(DEFAULT_ITEM_HEIGHT-6)/2);
    m_shapePath.addEllipse(QPointF(0,(DEFAULT_ITEM_HEIGHT/2)),3,3);
    m_shapePath.moveTo(0,DEFAULT_ITEM_HEIGHT - (DEFAULT_ITEM_HEIGHT-6)/2);
    m_shapePath.lineTo(0,DEFAULT_ITEM_HEIGHT);

    m_info = node;

    if(m_info)
    {
        setToolTip( QString::fromUtf8("Имя:") + '\t' + '\t' + m_info->title()
                    + '\n' + QString::fromUtf8("Время рассчета:") + '\t' + m_info->calcDt().toString("dd.MM.yyyy HH:mm:ss"));

        connect(m_info, SIGNAL(changed()),this,SLOT(updateItemGeometry()));

        m_info->increaseLinkCnt();
    }

    setAcceptHoverEvents(true);

}

GanttCalcGraphicsObject::~GanttCalcGraphicsObject()
{
    if(m_scene)
    {
        m_scene->removeItem(this);
        setParentItem(NULL);
        m_scene->update(sceneBoundingRect());
        if(m_scene->dtItems().isEmpty())
        {
            m_scene->setEmpty(true);
            m_scene->updateSceneRect();
        }
    }

    if(m_info)
        m_info->reduceLinkCnt();
}

QRectF GanttCalcGraphicsObject::boundingRect() const
{
    return m_shapePath.controlPointRect();
}

void GanttCalcGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QColor color = (m_scene->currentItem() == this)?(QColor(Qt::red).darker(130)):(QColor(Qt::red));
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->drawPath(m_shapePath);
    painter->fillPath(m_shapePath,QBrush(color.lighter(130)));
}

void GanttCalcGraphicsObject::setScene(GanttScene *scene)
{
    m_scene = scene;
    m_scene->addItem(this);
}

void GanttCalcGraphicsObject::setHeader(GanttHeader *header)
{
    m_header = header;
}

void GanttCalcGraphicsObject::updateItemGeometry()
{
    if(!m_header || !m_info)
        return;

    qreal calcPos = m_header->dtToX(m_info->calcDt());

    setPos(calcPos, 2*DEFAULT_ITEM_WIDTH + m_info->pos());
}

void GanttCalcGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    if(!m_scene || !m_info)
//        return;

//    if((this == m_scene->itemAt(event->scenePos())))
//        if((!m_info->isExpanded() && (event->button() == Qt::LeftButton))
//                /*|| (m_info->isExpanded() && (event->button() == Qt::RightButton))*/)
//        {
//            m_scene->changeExpanding(m_info->index());
//            emit graphicsItemPressed();
//        }

    QGraphicsItem::mousePressEvent(event);
}

void GanttCalcGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit graphicsItemHoverEnter();

    QGraphicsItem::hoverEnterEvent(event);
}

void GanttCalcGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    emit graphicsItemHoverLeave();

    QGraphicsItem::hoverLeaveEvent(event);
}

GanttInfoNode *GanttCalcGraphicsObject::info() const
{
    return m_info;
}

