#include "ganttcalcgraphicsobject.h"
#include "gantt/private_extensions/gantt-lib_global_values.h"

#include "gantt/plot/ganttscene.h"
#include "gantt/info/ganttinfonode.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QBitmap>

#include <QDebug>

QRect initializeGeometry(){
    const int   width = 12,
                height = 14,
                topOffset = 1;

    return QRect(0, topOffset,
                 width, height);
}
QPixmap initializePixmap(const QSize &pixmapSize, const QString &filename){
    QPixmap res(pixmapSize);
    QImage img(filename);
    res = res.fromImage(img.scaled(pixmapSize.width(),pixmapSize.height(),
                                   Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    return res;
}
QPainterPath initializeGeometryPath(){
    QPainterPath path;
    path.addRect(GanttCalcGraphicsObject::_geometry);
    return path;
}

QRect GanttCalcGraphicsObject::_geometry = initializeGeometry();
QPainterPath GanttCalcGraphicsObject::_geometryPath = initializeGeometryPath();


void GanttCalcGraphicsObject::init()
{
    _pixmapImageActive = initializePixmap(_geometry.size(), ":/images/calculator_active.png");
    _pixmapImageInactive = initializePixmap(_geometry.size(), ":/images/calculator_inactive.png");

    if(info())
    {
        updateToolTip();

        connect(info(), SIGNAL(changed()),this,SLOT(updateItemGeometry()));
        connect(info(), SIGNAL(changed()),this,SLOT(updateToolTip()));

        info()->increaseLinkCnt();
    }

    setAcceptHoverEvents(true);
    setZValue(500);

}

GanttCalcGraphicsObject::GanttCalcGraphicsObject(GanttInfoNode* node, QGraphicsItem *parent)
    :GanttTextGraphicsObject(node,parent)
{
    init();
}



QPainterPath GanttCalcGraphicsObject::shape() const
{
    return _geometryPath;
}

void GanttCalcGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(_current)
        painter->drawPixmap(_geometry, _pixmapImageActive, _pixmapImageActive.rect());
    else
        painter->drawPixmap(_geometry, _pixmapImageInactive, _pixmapImageInactive.rect());

    GanttTextGraphicsObject::paint(painter, option, widget);
}

QString GanttCalcGraphicsObject::textRight() const
{
    GanttInfoNode *node = qobject_cast<GanttInfoNode*>(info());
    if(node && node->isExpanded())
        return node->title();
    return QString();
}

void GanttCalcGraphicsObject::updateItemGeometry()
{
    if(!_dtline || !info())
        return;

    qreal calcPos = _dtline->dtToPos(info()->start());

    setPos(calcPos, info()->calcPos());
}

void GanttCalcGraphicsObject::updateToolTip()
{
    setToolTip( QString::fromUtf8("НУ-РЕШ-ВИТОК:") + '\t' + '\t' + info()->title()
                + '\n' + QString::fromUtf8("Время рассчета:") + '\t' + info()->start().toString("dd.MM.yyyy HH:mm:ss"));
}

void GanttCalcGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
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
