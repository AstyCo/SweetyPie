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

QRect GanttCalcGraphicsObject::_geometry = initializeGeometry();



void GanttCalcGraphicsObject::init()
{
    _pixmapImageActive = initializePixmap(_geometry.size(), ":/images/calculator_active.png");
    _pixmapImageInactive = initializePixmap(_geometry.size(), ":/images/calculator_inactive.png");

    if(innerInfo())
    {
        setToolTip( QString::fromUtf8("НУ-РЕШ-ВИТОК:") + '\t' + '\t' + innerInfo()->title()
                    + '\n' + QString::fromUtf8("Время рассчета:") + '\t' + innerInfo()->start().toString("dd.MM.yyyy HH:mm:ss"));

        connect(innerInfo(), SIGNAL(changed()),this,SLOT(updateItemGeometry()));

        innerInfo()->increaseLinkCnt();
    }

    setAcceptHoverEvents(true);

}

GanttCalcGraphicsObject::GanttCalcGraphicsObject(GanttInfoNode* node, QGraphicsItem *parent)
    :GanttGraphicsObject(node,parent)
{
    init();
}

GanttCalcGraphicsObject::~GanttCalcGraphicsObject()
{
    if(_scene)
    {
        _scene->removeItem(this);
        setParentItem(NULL);
    }

    if(innerInfo())
        innerInfo()->reduceLinkCnt();
}

QRectF GanttCalcGraphicsObject::boundingRect() const
{
    return _geometry;
}

void GanttCalcGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(_scene->currentItem() == this)
        painter->drawPixmap(_geometry, _pixmapImageActive, _pixmapImageActive.rect());
    else
        painter->drawPixmap(_geometry, _pixmapImageInactive, _pixmapImageInactive.rect());

//    QColor color = (_scene->currentItem() == this)?(QColor(Qt::red).darker(130)):(QColor(Qt::red));
//    painter->setRenderHints(/*QPainter::SmoothPixmapTransform |*/ QPainter::HighQualityAntialiasing | QPainter::Antialiasing);

//    painter->drawPath(m_shapePath);
//    painter->fillPath(m_shapePath,QBrush(color.lighter(130)));
}

GanttInfoNode *GanttCalcGraphicsObject::innerInfo() const
{
    return qobject_cast<GanttInfoNode*>(m_info);
}

void GanttCalcGraphicsObject::updateItemGeometry()
{
    if(!_scene || !innerInfo())
        return;

    qreal calcPos = _scene->dtToPos(innerInfo()->start());

    setPos(calcPos, innerInfo()->calcPos());
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
