#include "ganttcurrentdtslider.h"
#include "ganttglobalvalues.h"
#include "ganttscene.h"


#include <QPainter>
#include <QGraphicsScene>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>

GanttCurrentDtSlider::GanttCurrentDtSlider(QGraphicsItem* parent) :
    QGraphicsObject(parent)
{
    m_draw = true;
    setDraw(false);

    m_penWidth = 2;
    setCursor(Qt::OpenHandCursor);
    setZValue(20);
}

void GanttCurrentDtSlider::setScene(GanttScene *scene)
{
    if(!scene)
        return;

    m_scene = scene;
    scene->addItem(this);


    setSlidersRect(scene->sceneRect());


}

QRectF GanttCurrentDtSlider::boundingRect() const
{
    return m_sliderShape.controlPointRect();
}

void GanttCurrentDtSlider::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(!m_draw)
        return;

    painter->setRenderHint(QPainter::Antialiasing,true);

    QBrush borderBrush(QColor(Qt::black).lighter(130));
    QBrush fillBrush(QColor(Qt::red));

    QPen pen(QBrush(QColor(Qt::black)), 1 ,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
    painter->setPen(pen);

    painter->fillPath(m_sliderShape,borderBrush);
    painter->fillPath(m_rhombus,fillBrush);
    painter->drawPath(m_rhombus);

}

QPainterPath GanttCurrentDtSlider::shape() const
{
    return m_sliderShape;
}

void GanttCurrentDtSlider::updateShape()
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




void GanttCurrentDtSlider::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!scene())
        return;

    if(cursor().shape() == Qt::ClosedHandCursor)
    {
        setDt(m_scene->xToDt(mapToScene(event->pos()).x()));
    }
}

void GanttCurrentDtSlider::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::ClosedHandCursor);
}

void GanttCurrentDtSlider::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);
    QGraphicsItem::mouseReleaseEvent(event);
}

void GanttCurrentDtSlider::makeStep(long long deltaVal)
{
    setDt(m_dt.addMicroseconds(deltaVal));
}

void GanttCurrentDtSlider::updateScenePos()
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

void GanttCurrentDtSlider::updateRange(const UtcDateTime &minDt, const UtcDateTime &maxDt)
{
    m_minDt = minDt;
    m_maxDt = maxDt;
}

bool GanttCurrentDtSlider::outOfRange() const
{
    if(!m_scene)
    {
        Q_ASSERT(false);
        return true;
    }

    return !(m_dt.isValid()) || m_dt < m_scene->startDt() || m_dt > m_scene->finishDt();
}

bool GanttCurrentDtSlider::outOfBounds(const UtcDateTime &dt) const
{
    return (dt<m_minDt || dt>m_maxDt);
}

void GanttCurrentDtSlider::setVisible(bool visible)
{
    if(visible == m_visible)
        return;

    m_visible = visible;
    if(m_draw)
        QGraphicsObject::setVisible(visible);
}
bool GanttCurrentDtSlider::draw() const
{
    return m_draw;
}

void GanttCurrentDtSlider::setDraw(bool draw)
{
    if(m_draw == draw)
        return;

    m_draw = draw;
    if(m_draw && m_visible)
        QGraphicsObject::setVisible(true);
    if(!m_draw)
        QGraphicsObject::setVisible(false);

    update();
    emit drawChanged(draw);
}


UtcDateTime GanttCurrentDtSlider::maxDt() const
{
    return m_maxDt;
}

void GanttCurrentDtSlider::moveToBegin()
{
    if(m_initialized)
        setDt(m_minDt);
}

void GanttCurrentDtSlider::moveToEnd()
{
    if(m_initialized)
        setDt(m_maxDt);
}

void GanttCurrentDtSlider::moveToRangeStart()
{
    if(!m_scene)
    {
        Q_ASSERT(false);
        return;
    }

    setDt(m_scene->startDt());
}

void GanttCurrentDtSlider::moveToRangeFinish()
{
    if(!m_scene)
    {
        Q_ASSERT(false);
        return;
    }

    setDt(m_scene->finishDt());
}

UtcDateTime GanttCurrentDtSlider::minDt() const
{
    return m_minDt;
}


bool GanttCurrentDtSlider::initialized() const
{
    return m_initialized;
}

qreal GanttCurrentDtSlider::relativePos() const
{
    if(m_dt.isValid() && m_minDt.isValid() && m_maxDt.isValid())
    {
        return (m_minDt.microsecondsTo(m_dt) * 1.0) / (m_minDt.microsecondsTo(m_maxDt));
    }
    return 0;
}

bool GanttCurrentDtSlider::setDt(UtcDateTime dt)
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

UtcDateTime GanttCurrentDtSlider::dt() const
{
    return m_dt;
}

void GanttCurrentDtSlider::setSlidersRect(const QRectF &slidersRect)
{
    m_slidersRect = slidersRect;
    updateShape();
    updateScenePos();

}

void GanttCurrentDtSlider::setPos(const QPointF &pos)
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

void GanttCurrentDtSlider::setPos(qreal x, qreal y)
{
    setPos(QPointF(x,y));
}
