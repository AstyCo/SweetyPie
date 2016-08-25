#include "ganttcurrenttimeslider.h"
#include "ganttglobalvalues.h"
#include "ganttscene.h"


#include <QPainter>
#include <QGraphicsScene>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>

GanttSlider::GanttSlider(QGraphicsItem* parent) :
    QGraphicsObject(parent)
{
    m_penWidth = 2;
    setCursor(Qt::OpenHandCursor);
    setZValue(20);
    setVisible(false);
}

void GanttSlider::setScene(GanttScene *scene)
{
    if(!scene)
        return;

    m_scene = scene;
    scene->addItem(this);


    setSlidersRect(scene->sceneRect());


}

QRectF GanttSlider::boundingRect() const
{
    return m_sliderShape.controlPointRect();
}

void GanttSlider::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing,true);

    QBrush borderBrush(QColor(Qt::black).lighter(130));
    QBrush fillBrush(QColor(Qt::red));

    QPen pen(QBrush(QColor(Qt::black)), 1 ,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
    painter->setPen(pen);

    painter->fillPath(m_sliderShape,borderBrush);
    painter->fillPath(m_rhombus,fillBrush);
    painter->drawPath(m_rhombus);

}

QPainterPath GanttSlider::shape() const
{
    return m_sliderShape;
}

void GanttSlider::updateShape()
{
    QPainterPath path;
    qreal top = 0,
            bottom = m_slidersRect.height(),
            x = 0;

    path.moveTo(x - m_penWidth/2,top);
    path.lineTo(x + m_penWidth/2,top);
    path.lineTo(x + m_penWidth/2,bottom);
    path.lineTo(x - m_penWidth/2,bottom);
    path.lineTo(x - m_penWidth/2,top);

    qreal diagonal = (3.0/4)*DEFAULT_ITEM_HEIGHT;


    QPainterPath rhombus;
    rhombus.moveTo(QPointF(x - diagonal/2,DEFAULT_HEADER_HEIGHT - diagonal/2));
    rhombus.lineTo(QPointF(x ,DEFAULT_HEADER_HEIGHT - diagonal));
    rhombus.lineTo(QPointF(x + diagonal/2,DEFAULT_HEADER_HEIGHT - diagonal/2));
    rhombus.lineTo(QPointF(x ,DEFAULT_HEADER_HEIGHT));
    rhombus.lineTo(QPointF(x - diagonal/2,DEFAULT_HEADER_HEIGHT - diagonal/2));

    path.addPath(rhombus);

    prepareGeometryChange();
    m_rhombus = rhombus;
    m_sliderShape = path;
}




void GanttSlider::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!scene())
        return;

    if(cursor().shape() == Qt::ClosedHandCursor)
    {
        setDt(m_scene->xToDt(mapToScene(event->pos()).x()));
    }
}

void GanttSlider::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ClosedHandCursor);
}

void GanttSlider::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);
    QGraphicsItem::mouseReleaseEvent(event);
}

void GanttSlider::makeStep(long long deltaVal)
{
    setDt(m_dt.addMicroseconds(deltaVal));
}

void GanttSlider::updateScenePos()
{
    if(outOfRange())
    {
        setVisible(false);
    }
    else
    {
        if(!m_scene)
        {
            Q_ASSERT(false);
            return;
        }

        setVisible(true);

        setPos((m_dt.isValid())?(m_scene->dtToX(m_dt)):(m_slidersRect.left()), m_slidersRect.top());
    }
}

void GanttSlider::updateRange(const UtcDateTime &minDt, const UtcDateTime &maxDt)
{
    GanttHeader::GanttPrecisionMode mode = m_scene->calculateTimeMode(minDt,maxDt);

    m_minDt = minDt;
    m_maxDt = maxDt;
}

bool GanttSlider::outOfRange() const
{
    if(!m_scene)
    {
        Q_ASSERT(false);
        return true;
    }

    return !(m_dt.isValid()) || m_dt < m_scene->startDt() || m_dt > m_scene->finishDt();
}

bool GanttSlider::outOfBounds(const UtcDateTime &dt) const
{
    return (dt<m_minDt || dt>m_maxDt);
}
UtcDateTime GanttSlider::maxDt() const
{
    return m_maxDt;
}

void GanttSlider::moveToBegin()
{
    if(m_initialized)
        setDt(m_minDt);
}

void GanttSlider::moveToEnd()
{
    if(m_initialized)
        setDt(m_maxDt);
}

void GanttSlider::moveToRangeStart()
{
    if(!m_scene)
    {
        Q_ASSERT(false);
        return;
    }

    setDt(m_scene->startDt());
}

void GanttSlider::moveToRangeFinish()
{
    if(!m_scene)
    {
        Q_ASSERT(false);
        return;
    }

    setDt(m_scene->finishDt());
}

UtcDateTime GanttSlider::minDt() const
{
    return m_minDt;
}


bool GanttSlider::initialized() const
{
    return m_initialized;
}

qreal GanttSlider::relativePos() const
{
    if(m_dt.isValid() && m_minDt.isValid() && m_maxDt.isValid())
    {
        return (m_minDt.microsecondsTo(m_dt) * 1.0) / (m_minDt.microsecondsTo(m_maxDt));
    }
    return 0;
}

bool GanttSlider::setDt(UtcDateTime dt)
{
    if(dt < m_minDt)
        dt = m_minDt;
    if(dt > m_maxDt)
        dt = m_maxDt;

    if(dt == m_dt)
        return false;

    m_dt = dt;

    if(!m_initialized)
        m_initialized = true;

    if(!m_scene)
    {
        Q_ASSERT(false);
        return false;
    }

    updateScenePos();

    emit dtChanged(dt);
    return true;
}

UtcDateTime GanttSlider::dt() const
{
    return m_dt;
}

void GanttSlider::setSlidersRect(const QRectF &slidersRect)
{
    m_slidersRect = slidersRect;
    updateShape();
    updateScenePos();

}

void GanttSlider::setPos(const QPointF &pos)
{
    qreal x = pos.x();

    if(x < m_slidersRect.left())
        x = m_slidersRect.left();
    if(x > m_slidersRect.right())
        x = m_slidersRect.right();

    QGraphicsItem::setPos(x,pos.y());

    if(m_scene)
        m_scene->invalidate(QRectF(),QGraphicsScene::BackgroundLayer);
}

void GanttSlider::setPos(qreal x, qreal y)
{
    setPos(QPointF(x,y));
}
