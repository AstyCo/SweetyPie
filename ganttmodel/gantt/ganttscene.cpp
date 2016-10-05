#include "ganttscene.h"
#include "ganttheader.h"

#include "ganttgraphicsview.h"
#include "ganttintervalgraphicsobject.h"
#include "ganttcalcgraphicsobject.h"
#include "ganttdtcrossobject.h"
#include "gantthovergraphicsobject.h"

#include "ganttinfoleaf.h"
#include "ganttinfonode.h"


#include <QGraphicsView>

#include <QScrollBar>
#include <QSize>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QtCore/qmath.h>
#include <QDebug>

GanttScene::GanttScene(QObject * parent) :
    QGraphicsScene(parent)
{
    m_currentItem = NULL;

    setSceneRect(0,0,GANTTSCENE_MIN_WIDTH,0);

    m_header = new GanttHeader;
    m_header->setScene(this);
    m_slider = new GanttCurrentDtSlider;
    m_slider->setScene(this);
    m_crossObject = new GanttDtCrossObject;
    m_crossObject->setScene(this);
    m_hoverObject = new GanttHoverGraphicsObject;
    m_hoverObject->setScene(this);

    connect(m_slider,SIGNAL(dtChanged(UtcDateTime)),this,SIGNAL(currentDtChanged(UtcDateTime)));

    updateSliderRect();

//    QBrush backgroundBrush(Qt::HorPattern);
//    backgroundBrush.setMatrix(QMatrix().scale(1,DEFAULT_ITEM_HEIGHT*1.0/15));

//    setBackgroundBrush(backgroundBrush);


}

void GanttScene::updateWidth(int w)
{
    if(w < GANTTSCENE_MIN_WIDTH)
        w = GANTTSCENE_MIN_WIDTH;

    setSceneRect(0,0,w,sceneRect().height());

    if(views().isEmpty())
        return;

    if(m_header->headerMode() == GanttHeader::TimelineMode)
        return;
}

void GanttScene::updateHeight(int h)
{

    setSceneRect(0,0,sceneRect().width(),h);
}

void GanttScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter,rect);

    qreal   bgBottom = rect.bottom()
            ,bgLeft = rect.left() - 1
            ,bgRight = rect.right() + 1;

    QPen pen(Qt::gray, qreal(0.5));
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);

    qreal startY = (qCeil(rect.y() / DEFAULT_ITEM_HEIGHT))*DEFAULT_ITEM_HEIGHT;

    while(startY<bgBottom)
    {
        painter->drawLine(QPointF(bgLeft,startY),QPointF(bgRight,startY));
        startY+=DEFAULT_ITEM_HEIGHT;
    }
}

void GanttScene::setMode(GanttHeader::GanttPrecisionMode newMode)
{
    m_header->setMode(newMode);

    updateItems();
}

void GanttScene::addItems(GanttInfoItem* item)
{
//    m_header->onItemsAddition(item);
    if(m_header)
    {
        setEmpty(false);
        m_header->updateHeader();
    }

    addItemsHelper(item);
    {
        m_slider->updateRange(root());
        m_slider->setToBegin();
    }


    updateSceneRect();


//    QRectF _sceneRect = sceneRect();
//    setSceneRect(_sceneRect.x(),_sceneRect.y(),_sceneRect.width()
//                 ,_sceneRect.height() + DEFAULT_ITEM_HEIGHT);
}

void GanttScene::addItems(const QList<GanttInfoItem *> &items)
{
//    m_header->onItemsAddition(items);
    if(m_header)
    {
        setEmpty(false);
        m_header->updateHeader();
    }

    foreach(GanttInfoItem* item, items)
        addItemsHelper(item);
    {
        m_slider->updateRange(root());
        m_slider->setToBegin();
    }
    updateSceneRect();



//    QRectF _sceneRect = sceneRect();
//    setSceneRect(_sceneRect.x(),_sceneRect.y(),_sceneRect.width()
//                 ,_sceneRect.height() + items.count() * DEFAULT_ITEM_HEIGHT);
}

QGraphicsObject *GanttScene::itemByInfo(const GanttInfoItem *key) const
{
    return m_itemByInfo.value(key);
}

void GanttScene::updateHeaderPos(int dy)
{
    m_header->setPos(m_header->scenePos().x(),/*m_header->scenePos().y()+*/dy);
    updateSliderRect();
}

void GanttScene::onViewResize(const QSize&newSize)
{
    if(m_header->headerMode() == GanttHeader::TimelineMode)
    {
        updateWidth(newSize.width());
        m_header->setCurrentWidth(newSize.width());
        updateItems();
    }
    updateSliderRect();
    emit viewResized();

}

void GanttScene::updateSceneRect()
{

    if(!sceneHaveItems())
    {
        if(m_header)
            setSceneRect(m_header->mapRectToScene(m_header->boundingRect()));
    }
    else
    {
        setSceneRect(elementsBoundingRect());
    }

    updateSliderRect();
}

void GanttScene::makeStep(long long step)
{
    if(m_slider)
        m_slider->makeStep(step);
}

void GanttScene::moveSliderToNextEventStart()
{
    if(m_slider)
    {
        const GanttInfoLeaf* nextEventInfo = nextEvent(slidersDt());
        if(nextEventInfo)
        {
            m_slider->setDt(nextEventInfo->start());
        }
        else
            m_slider->moveToEnd();
    }
}

void GanttScene::moveSliderToPrevEventFinish()
{
    if(m_slider)
    {
        const GanttInfoLeaf* prevEventInfo = prevEvent(slidersDt());
        if(prevEventInfo)
        {
            m_slider->setDt(prevEventInfo->finish());
        }
        else
            m_slider->moveToBegin();
    }
}

void GanttScene::moveSliderToViewStart()
{
    if(m_slider)
        m_slider->moveToRangeStart();
}

void GanttScene::moveSliderToViewFinish()
{
    if(m_slider)
        m_slider->moveToRangeFinish();
}

void GanttScene::moveSliderToStart()
{
    if(m_slider)
        m_slider->moveToBegin();
}

void GanttScene::onViewAdded(GanttGraphicsView* view)
{

    if(!view)
        return;
    m_view = view;
    m_header->init();
    onViewResize(view->size());
}

void GanttScene::setHeaderMode(GanttHeader::GanttHeaderMode mode)
{
    if(m_header->setHeaderMode(mode))
    {
        if(mode == GanttHeader::TimelineMode)
        {
            m_header->updateHeader();
        }
        else if(mode == GanttHeader::GanttDiagramMode)
        {
            updateItems();
        }
        if(!views().isEmpty())
        {
            onViewAdded(qobject_cast<GanttGraphicsView*>(views()[0]));
            views()[0]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
}

GanttHeader::GanttHeaderMode GanttScene::headerMode() const
{
    if(m_header.isNull())
    {
        Q_ASSERT(false);
        return GanttHeader::GanttHeaderMode_count;
    }

    return m_header->headerMode();
}

void GanttScene::changeExpanding(const QModelIndex &index)
{
    if(views().isEmpty())
        return;
    GanttGraphicsView* p_view = dynamic_cast<GanttGraphicsView*>(views()[0]);
    if(!p_view)
        return;

    p_view->changeExpanding(index);
}

UtcDateTime GanttScene::slidersDt() const
{
    if(m_header.isNull() || !m_slider)
        return UtcDateTime();

    return m_slider->dt();
}


void GanttScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if(event->buttons() & Qt::LeftButton)
    {
        QGraphicsObject *object = objectForPos(event->scenePos());
        if(object)
        {
            setCurrentItem(object);
        }
    }

    mouseMoveEvent(event);
}

void GanttScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton)
    {
        if(!m_view.isNull())
        {
            QRectF viewRect = m_view->mapToScene(m_view->viewport()->geometry()).boundingRect()
                    .adjusted(0,DEFAULT_HEADER_HEIGHT,0,0);
            if(viewRect.contains(event->scenePos()) && !m_crossObject.isNull())
            {
                m_crossObject->setVisible(true);
                m_crossObject->setPos(event->scenePos());
            }
            else
            {
                m_crossObject->setVisible(false);
            }
        }
    }


    QGraphicsScene::mouseMoveEvent(event);
}

void GanttScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        if(!m_crossObject.isNull())
        {
            m_crossObject->setVisible(false);
        }
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void GanttScene::onVisItemDestroyed(QObject *item)
{
    Q_UNUSED(item);
    updateSceneRect();
}

void GanttScene::onLeafStartChanged(/*const UtcDateTime& lastStart*/)
{
    GanttInfoLeaf* p_leaf = qobject_cast<GanttInfoLeaf*>(sender());

    if(!p_leaf)
        return;

    m_infoByStart.remove(m_infoByStart.key(p_leaf));
    m_infoByStart.insert(p_leaf->start(),p_leaf);

}

void GanttScene::onLeafFinishChanged(/*const UtcDateTime& lastFinish*/)
{
    GanttInfoLeaf* p_leaf = qobject_cast<GanttInfoLeaf*>(sender());

    if(!p_leaf)
        return;

    m_infoByFinish.remove(m_infoByFinish.key(p_leaf));
    m_infoByFinish.insert(p_leaf->start(),p_leaf);
}

void GanttScene::setModel(const QPointer<GanttTreeModel> &model)
{
    m_model = model;
}

QGraphicsItem *GanttScene::currentItem() const
{
    return m_currentItem;
}

void GanttScene::setSceneRect(const QRectF &rect)
{
    QGraphicsScene::setSceneRect(rect);
}

void GanttScene::setSceneRect(qreal x, qreal y, qreal width, qreal height)
{
    setSceneRect(QRectF(x,y,width,height));
}

bool GanttScene::isVisible(const QGraphicsItem *which) const
{
    if(m_view.isNull())
        return false;

    QRectF viewRect = m_view->mapToScene(m_view->viewport()->geometry()).boundingRect()
            .adjusted(0,DEFAULT_HEADER_HEIGHT,0,0);

    return viewRect.contains(which->sceneBoundingRect());
}

QRectF GanttScene::elementsBoundingRect()
{
    QRectF res = (m_header)?(m_header->sceneBoundingRect()):(QRectF());
    foreach(GanttIntervalGraphicsObject* object,m_items)
        res|=object->sceneBoundingRect();
    foreach(GanttCalcGraphicsObject *object,m_calcItems)
        res|=object->sceneBoundingRect();
    return res;
}

void GanttScene::clear()
{
    m_currentItem = NULL;
    if(!m_header.isNull())
        m_header->clear();
}

qreal GanttScene::headerBottom() const
{
    if(m_header.isNull())
        return 0;

    return m_header->sceneBoundingRect().bottom();
}

bool GanttScene::sceneHaveItems() const
{
    return !(m_items.isEmpty() && m_calcItems.isEmpty());
}

void GanttScene::setCurrentItem(QGraphicsObject *currentItem)
{
    if(m_currentItem == currentItem)
        return;

    QGraphicsItem *lastItem = m_currentItem;
    GanttInfoItem *info = NULL;
    m_currentItem = currentItem;
    if(lastItem)
    {
        lastItem->update();
    }

    if(m_currentItem)
    {

        if(GanttIntervalGraphicsObject *graphicsObject = dynamic_cast<GanttIntervalGraphicsObject*>(m_currentItem.data()))
            info = graphicsObject->info();
        if(GanttCalcGraphicsObject *graphicsObject = dynamic_cast<GanttCalcGraphicsObject*>(m_currentItem.data()))
            info = graphicsObject->info();

        GanttInfoLeaf *leaf = qobject_cast<GanttInfoLeaf*>(info);
        if(leaf && !leaf->parent()->isExpanded())
            changeExpanding(leaf->parent()->index());

        QRectF itemRect = m_currentItem->mapToScene(m_currentItem->boundingRect()).boundingRect();
        if(!m_view.isNull() && !isVisible(m_currentItem))
        {
            m_view->ensureVisible(itemRect
                                  .adjusted(0,-DEFAULT_HEADER_HEIGHT,0,0)
                                  ,0,0);
        }
        m_currentItem->update();
        if(m_hoverObject)
        {
            m_hoverObject->setPos(m_currentItem->pos());
            m_hoverObject->setVisible(true);
        }
    }
    else
        if(m_hoverObject)
            m_hoverObject->setVisible(false);
    emit currentItemChanged(info);
}

QGraphicsObject *GanttScene::objectForPos(const QPointF &pos)
{
    int y = ((int) pos.y()) / DEFAULT_ITEM_HEIGHT * DEFAULT_ITEM_HEIGHT;
    QList<QGraphicsItem*> itemsInTheArea = items(0,y,sceneRect().width(),DEFAULT_ITEM_HEIGHT);

    foreach(QGraphicsItem* item, itemsInTheArea)
    {
        if(QGraphicsObject *object = dynamic_cast<GanttIntervalGraphicsObject*>(item))
            return object;
        if(QGraphicsObject *object = dynamic_cast<GanttCalcGraphicsObject*>(item))
            return object;
    }
    return NULL;
}
const QList<GanttIntervalGraphicsObject *>& GanttScene::dtItems() const
{
    return m_items;
}

void GanttScene::removeItem(QGraphicsItem *item)
{
    //    GanttGraphicsItem* ganttItem = dynamic_cast<GanttGraphicsItem*>(item);
//    if(ganttItem)
//    {
//        m_items.removeOne(ganttItem);
//    }
    QGraphicsScene::removeItem(item);
}

void GanttScene::setDrawCurrentDtSlider(bool enable)
{
    if(m_slider)
        m_slider->setDraw(enable);
}


void GanttScene::updateSlider()
{
    if(!m_slider)
        return;

    m_slider->updateScenePos();
}


void GanttScene::addItemsHelper(GanttInfoItem *item)
{
    if(!item)
        return;

    connect(item,SIGNAL(destroyed(QObject*)),this,SLOT(onVisItemDestroyed(QObject*)));

    GanttInfoLeaf *leaf = dynamic_cast<GanttInfoLeaf*>(item);
    if(leaf)
    {
        GanttIntervalGraphicsObject *p_item = new GanttIntervalGraphicsObject(leaf);

        connect(p_item,SIGNAL(graphicsItemHoverEnter()),this,SLOT(onGraphicsItemHoverEnter()));
        connect(p_item,SIGNAL(graphicsItemHoverLeave()),this,SLOT(onGraphicsItemHoverLeave()));
        connect(p_item,SIGNAL(graphicsItemPressed()),this,SLOT(onGraphicsItemPress()));


        p_item->setScene(this);
        p_item->setHeader(m_header);

        m_items.append(p_item);
        m_itemByInfo.insert(leaf,p_item);
        m_infoByStart.insert(leaf->start(),leaf);
        m_infoByFinish.insert(leaf->finish(),leaf);
        p_item->updateItemGeometry();
    }
    else
    {
        GanttInfoNode *node = dynamic_cast<GanttInfoNode*>(item);
        if(node)
        {
            if(node->hasCalcDt())
            {
                GanttCalcGraphicsObject *p_item = new GanttCalcGraphicsObject(node);

                connect(p_item,SIGNAL(graphicsItemHoverEnter()),this,SLOT(onGraphicsItemHoverEnter()));
                connect(p_item,SIGNAL(graphicsItemHoverLeave()),this,SLOT(onGraphicsItemHoverLeave()));
                connect(p_item,SIGNAL(graphicsItemPressed()),this,SLOT(onGraphicsItemPress()));


                p_item->setScene(this);
                p_item->setHeader(m_header);

                m_calcItems.append(p_item);
                m_itemByInfo.insert(node,p_item);

                p_item->updateItemGeometry();
            }

            foreach(GanttInfoItem* item, node->m_items)
                addItemsHelper(item);
        }
    }
}

void GanttScene::updateItems()
{

    for(int i = 0; i < m_items.size(); ++i)
    {
        GanttInfoLeaf* p_info = m_items[i]->innerInfo();
        if(p_info)
        {
            qreal startPos = m_header->dtToX(p_info->start()),
                    itemWidth = m_header->dtToX(p_info->finish()) - startPos;
            qreal top = m_items[i]->y(),
                    height = DEFAULT_ITEM_HEIGHT;


            m_items[i]->setPos(startPos, top);
            m_items[i]->setBoundingRectSize(QSizeF(itemWidth, height));
        }
    }

    for(int i=0; i<m_calcItems.size(); ++i)
    {
        GanttInfoNode* p_info = m_calcItems[i]->innerInfo();

        if(p_info)
        {
            qreal calcPos = m_header->dtToX(p_info->calcDt());
            qreal top = m_calcItems[i]->y();

            m_calcItems[i]->setPos(calcPos, top);
        }
    }


    update();
}

void GanttScene::onGraphicsItemPress()
{
    GanttIntervalGraphicsObject *item = qobject_cast<GanttIntervalGraphicsObject*>(sender());
    if(item)
    {
        setCurrentItem(item);
    }

    GanttCalcGraphicsObject *calcItem = qobject_cast<GanttCalcGraphicsObject*>(sender());
    if(calcItem)
    {
        setCurrentItem(calcItem);
    }
}

void GanttScene::onGraphicsItemHoverEnter()
{
    GanttIntervalGraphicsObject *item = qobject_cast<GanttIntervalGraphicsObject*>(sender());
    if(item)
    {
//        setCurrentItem(item);
        emit graphicsItemHoverEnter(item->info());
    }

    GanttCalcGraphicsObject *calcItem = qobject_cast<GanttCalcGraphicsObject*>(sender());
    if(calcItem)
    {
//        setCurrentItem(calcItem);
        emit graphicsItemHoverEnter(calcItem->info());
    }


}

void GanttScene::onGraphicsItemHoverLeave()
{
    GanttIntervalGraphicsObject *item = qobject_cast<GanttIntervalGraphicsObject*>(sender());
    if(item)
        emit graphicsItemHoverLeave(item->info());

    GanttCalcGraphicsObject *calcItem = qobject_cast<GanttCalcGraphicsObject*>(sender());
    if(calcItem)
        emit graphicsItemHoverLeave(calcItem->info());
}

void GanttScene::onInfoDelete()
{
    const GanttInfoItem* item = static_cast<const GanttInfoItem*>(sender());
    removeByInfo(item);
}



void GanttScene::onInfoLeafDelete()
{
    const GanttInfoLeaf* leaf = static_cast<const GanttInfoLeaf*>(sender());
    removeByInfoLeaf(leaf);
//    QList<QGraphicsItem*> p_items = items();
}

void GanttScene::onInfoChanged()
{
    GanttInfoLeaf * leaf = dynamic_cast<GanttInfoLeaf*>(sender());
    if(!leaf)
        return;

    if(m_header->verifyBoundsByLeaf(leaf))
        m_header->updateHeader();
}

GanttCurrentDtSlider *GanttScene::slider() const
{
    return m_slider;
}

void GanttScene::setRange(UtcDateTime min, UtcDateTime max)
{
    m_header->setRange(min,max);
    updateItems();
    updateSlider();
    emit limitsChanged(m_header->startDt(),m_header->finishDt());
}

GanttInfoNode *GanttScene::root() const
{
    if(m_model.isNull())
        return NULL;
    return m_model->root();
}

UtcDateTime GanttScene::minDt() const
{
    if(m_header.isNull())
        return UtcDateTime();
    return m_header->minDt();
}

UtcDateTime GanttScene::maxDt() const
{
    if(m_header.isNull())
        return UtcDateTime();
    return m_header->maxDt();
}

UtcDateTime GanttScene::startDt() const
{
    if(m_header.isNull())
        return UtcDateTime();
    return m_header->startDt();
}

UtcDateTime GanttScene::finishDt() const
{
    if(m_header.isNull())
        return UtcDateTime();
    return m_header->finishDt();
}

UtcDateTime GanttScene::xToDt(qreal x) const
{
    if(m_header.isNull())
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return m_header->xToDt(x);
}

qreal GanttScene::dtToX(const UtcDateTime &dt) const
{
    if(m_header.isNull())
    {
        Q_ASSERT(false);
        return 0;
    }

    return m_header->dtToX(dt);
}

UtcDateTime GanttScene::startByDt(const UtcDateTime &dt,GanttHeader::GanttPrecisionMode mode) const
{
    if(m_header.isNull())
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return m_header->startByDt(dt,mode);
}

UtcDateTime GanttScene::startByDt(const UtcDateTime &dt) const
{
    if(m_header.isNull())
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return startByDt(dt, m_header->mode());
}

UtcDateTime GanttScene::finishByDt(const UtcDateTime &dt,GanttHeader::GanttPrecisionMode mode) const
{
    if(m_header.isNull())
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return m_header->finishByDt(dt,mode);
}

UtcDateTime GanttScene::finishByDt(const UtcDateTime &dt) const
{
    if(m_header.isNull())
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return finishByDt(dt, m_header->mode());
}

UtcDateTime GanttScene::nextStart(const UtcDateTime &dt,GanttHeader::GanttPrecisionMode mode) const
{
    if(m_header.isNull())
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return m_header->nextStart(dt,mode);
}

UtcDateTime GanttScene::nextStart(const UtcDateTime &dt) const
{
    if(m_header.isNull())
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return nextStart(dt, m_header->mode());
}

UtcDateTime GanttScene::prevFinish(const UtcDateTime &dt,GanttHeader::GanttPrecisionMode mode) const
{
    if(m_header.isNull())
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return m_header->prevFinish(dt,mode);
}

UtcDateTime GanttScene::prevFinish(const UtcDateTime &dt) const
{
    if(m_header.isNull())
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return prevFinish(dt, m_header->mode());
}

GanttHeader::GanttPrecisionMode GanttScene::headerPrecisionMode() const
{
    if(m_header.isNull())
    {
        qWarning("GanttScene::headerMode m_header is NULL");
        return (GanttHeader::GanttPrecisionMode)0;
    }
    return m_header->mode();
}

GanttHeader::GanttPrecisionMode GanttScene::calculateTimeMode(const UtcDateTime &min, const UtcDateTime &max) const
{
    if(m_header.isNull())
    {
        qWarning("GanttScene::calculateTimeMode m_header is NULL");
        return (GanttHeader::GanttPrecisionMode)0;
    }
    return m_header->calculateTimeMode(min,max);
}


long long GanttScene::minTimeUnit() const
{
    if(m_header.isNull())
    {
        qWarning("GanttScene::minTimeUnit m_header is NULL");
        return 1;
    }

    return m_header->modeToMicrosecond((GanttHeader::GanttPrecisionMode)((int)GanttHeader::GanttPrecisionMode_count - 1));
}

const GanttInfoLeaf *GanttScene::nextEvent(const UtcDateTime &curDt) const
{
    if(curDt.isValid())
    {
        QMap<UtcDateTime,const GanttInfoLeaf*>::const_iterator it = m_infoByStart.constBegin();
        while (it != m_infoByStart.constEnd())
        {
            if(curDt < it.key())
                return it.value();
            ++it;
        }
    }

    return NULL;
}

const GanttInfoLeaf *GanttScene::prevEvent(const UtcDateTime &curDt) const
{
    if(curDt.isValid())
    {
        QMapIterator<UtcDateTime,const GanttInfoLeaf*> it(m_infoByFinish);
        it.toBack();
        while (it.hasPrevious()) {
            it.previous();
            if(curDt > it.key())
                return it.value();
        }
    }

    return NULL;
}

void GanttScene::removeByInfo(const GanttInfoItem *item)
{ 
    if(!item)
        return;

    const GanttInfoLeaf *leaf = qobject_cast<const GanttInfoLeaf*>(item);
    if(leaf)
    {
        GanttIntervalGraphicsObject* graphicsItem = qobject_cast<GanttIntervalGraphicsObject*>(itemByInfo(leaf));
        if(graphicsItem)
            m_items.removeOne(graphicsItem);
        m_itemByInfo.remove(leaf);
        m_infoByStart.remove(m_infoByStart.key(leaf));
        m_infoByFinish.remove(m_infoByFinish.key(leaf));
        if(graphicsItem)
            graphicsItem->deleteLater();
    }

    const GanttInfoNode *node = qobject_cast<const GanttInfoNode *>(item);
    if(node)
    {
        GanttCalcGraphicsObject* graphicsItem = qobject_cast<GanttCalcGraphicsObject*>(itemByInfo(node));
        if(graphicsItem)
            m_calcItems.removeOne(graphicsItem);
        m_itemByInfo.remove(node);
        if(graphicsItem)
            graphicsItem->deleteLater();
    }
}

void GanttScene::removeByInfoLeaf(const GanttInfoLeaf *leaf)
{
    if(!leaf)
        return;

    GanttIntervalGraphicsObject* graphicsItem = qobject_cast<GanttIntervalGraphicsObject*>(itemByInfo(leaf));
    if(graphicsItem)
        m_items.removeOne(graphicsItem);
    m_itemByInfo.remove(leaf);
    m_infoByStart.remove(m_infoByStart.key(leaf));
    m_infoByFinish.remove(m_infoByFinish.key(leaf));
    if(graphicsItem)
        graphicsItem->deleteLater();
}

void GanttScene::setEmpty(bool empty)
{
    if(m_header)
    {
        m_header->setEmpty(empty);
    }
}

void GanttScene::updateSliderRect()
{
    if(!m_slider || m_header.isNull())
        return;

    if(m_header->headerMode() == GanttHeader::TimelineMode)
    {
        if(!views().isEmpty())
        {
            QGraphicsView* p_view = views()[0];


            m_slider->setSlidersRect(QRectF( MIN_WIDTH_FOR_TIME_VISUALIZING/2
                                     ,p_view->verticalScrollBar()->value()
                                     ,width()-MIN_WIDTH_FOR_TIME_VISUALIZING
                                     ,p_view->height()
                                     ));
        }
    }
    else if(m_header->headerMode() == GanttHeader::GanttDiagramMode)
    {
        if(!views().isEmpty())
        {
            QGraphicsView* p_view = views()[0];


            m_slider->setSlidersRect(QRectF( sceneRect().left()
                                     ,p_view->verticalScrollBar()->value()
                                     ,sceneRect().width()
                                     ,p_view->height()
                                     ));
        }
    }

}

