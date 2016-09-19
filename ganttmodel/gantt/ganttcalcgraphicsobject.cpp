#include "ganttcalcgraphicsobject.h"
#include "ganttglobalvalues.h"

#include "ganttscene.h"
#include "ganttheader.h"
#include "ganttinfonode.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>

GanttCalcGraphicsObject::GanttCalcGraphicsObject(GanttInfoNode* node, QGraphicsItem *parent)
    :GanttGraphicsObject(node,parent)
{
    m_shapePath.moveTo(0,0);
    m_shapePath.lineTo(0,(DEFAULT_ITEM_HEIGHT-6)/2);
    m_shapePath.addEllipse(QPointF(0,(DEFAULT_ITEM_HEIGHT/2)),3,3);
    m_shapePath.moveTo(0,DEFAULT_ITEM_HEIGHT - (DEFAULT_ITEM_HEIGHT-6)/2);
    m_shapePath.lineTo(0,DEFAULT_ITEM_HEIGHT);

    if(innerInfo())
    {
        setToolTip( QString::fromUtf8("НУ-РЕШ-ВИТОК:") + '\t' + '\t' + innerInfo()->title()
                    + '\n' + QString::fromUtf8("Время рассчета:") + '\t' + innerInfo()->calcDt().toString("dd.MM.yyyy HH:mm:ss"));

        connect(innerInfo(), SIGNAL(changed()),this,SLOT(updateItemGeometry()));

        innerInfo()->increaseLinkCnt();
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
    }

    if(innerInfo())
        innerInfo()->reduceLinkCnt();
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

GanttInfoNode *GanttCalcGraphicsObject::innerInfo() const
{
    return qobject_cast<GanttInfoNode*>(m_info);
}

void GanttCalcGraphicsObject::updateItemGeometry()
{
    if(!m_header || !innerInfo())
        return;

    qreal calcPos = m_header->dtToX(innerInfo()->calcDt());

    setPos(calcPos, 2*DEFAULT_ITEM_WIDTH + innerInfo()->pos());
}

void GanttCalcGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    if(!m_scene || !innerInfo())
//        return;

//    if((this == m_scene->itemAt(event->scenePos())))
//        if((!innerInfo()->isExpanded() && (event->button() == Qt::LeftButton))
//                /*|| (innerInfo()->isExpanded() && (event->button() == Qt::RightButton))*/)
//        {
//            m_scene->changeExpanding(innerInfo()->index());
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


