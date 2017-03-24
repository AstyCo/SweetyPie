/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию GanttDtCrossObject class.
 * \~englist
 * \brief File contains realization of GanttDtCrossObject class.
 */
#include "ganttdtcrossobject.h"
#include "gantt/plot/ganttscene.h"

#include <QGraphicsWidget>
#include <QPen>
#include <QFontMetrics>
#include <QPainter>

void GanttDtCrossObject::init()
{

    setZValue(10000);
    m_scene = NULL;
}

//void GanttDtCrossObject::setVisible(bool visible)
//{
//    if(scene() || !scene()->views().isEmpty()){
//        QWidget *vp = scene()->views()[0]->viewport();
//        if(visible){
//            _savedCursor = vp->cursor();
//            vp->setCursor(Qt::PointingHandCursor);
//        }
//        else
//            vp->setCursor(Qt::ArrowCursor);
//    }

//    QGraphicsObject::setVisible(visible);
//}

GanttDtCrossObject::GanttDtCrossObject(QGraphicsItem *parent)
    :QGraphicsObject(parent)
{
    init();
}

GanttDtCrossObject::GanttDtCrossObject(GanttScene *scene, QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    init();
    setScene(scene);
}

QRectF GanttDtCrossObject::boundingRect() const
{
    return (m_linesPath.controlPointRect() | m_textRect).adjusted(-100,-100,100,100);
}

//QPainterPath GanttDtCrossObject::shape() const
//{
//    return m_linesPath + m_datePath;
//}

void GanttDtCrossObject::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem */*option*/,
                               QWidget */*widget*/)
{
    if(!m_scene)
        return;

    QPen dashPen(Qt::black, 1, Qt::DashLine);

    painter->save();
    painter->setBrush(QBrush(Qt::white));
    painter->drawRect(m_textRect);
    painter->setFont(m_scene->font());
    painter->drawText(m_textRect,m_text,QTextOption(Qt::AlignCenter));
    painter->restore();

//    painter->setPen(dashPen);
//    painter->drawPath(m_linesPath);
}

void GanttDtCrossObject::setScene(GanttScene *scene)
{
    m_scene = scene;
    if(m_scene)
    {
        m_scene->addItem(this);
    }
}

void GanttDtCrossObject::setPos(const QPointF &pos)
{
    QGraphicsItem::setPos(pos);
    updatePaths();
}

void GanttDtCrossObject::setPos(qreal x, qreal y)
{
    setPos(QPointF(x,y));
}

void GanttDtCrossObject::updatePaths()
{
    if(m_scene)
    {
        QPointF rel = mapFromScene(m_scene->sceneRect().topLeft());
        QPainterPath linesPath ;

//        linesPath.moveTo(rel.x(),0);
//        linesPath.lineTo(0,0);
        linesPath.moveTo(0,0);
        linesPath.lineTo(0,mapFromScene(QPointF(0,0)).y());

        qreal offsetX = 4,
                offsetY = 0;

        m_text = m_scene->posToDt(scenePos().x()).toString(
                    QString::fromUtf8("HH:mm (ssсек.)  dd.MM.yyyy"));

        QFontMetrics fontMetrics(m_scene->font());
        QRect textRect;
        textRect =  fontMetrics.tightBoundingRect(m_text);
        int adjust = 5;
        textRect.adjust(-adjust,-adjust,adjust,adjust);
        offsetY = - textRect.height();
        textRect.moveTo(offsetX,offsetY);
        QPointF mappedTopRight = mapFromScene(m_scene->sceneRect().topRight());
        if(-mappedTopRight.y() < textRect.height())
            textRect.moveTo(10, offsetY = 5);
        if(mappedTopRight.x() < textRect.width())
            textRect.moveTo(-textRect.width()-offsetX,offsetY);
        prepareGeometryChange();
        m_linesPath = linesPath;
        m_textRect = textRect;

        update();
    }
}
