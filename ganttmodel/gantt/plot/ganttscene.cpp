#include "ganttscene.h"
#include "extensions/dtline.h"
#include "gantt/info/ganttinfotree.h"

#include "gantt/private_extensions/gantt-lib_global_values.h"

#include "scene_objects/ganttintervalgraphicsobject.h"
#include "scene_objects/ganttcalcgraphicsobject.h"
#include "scene_objects/ganttdtcrossobject.h"
#include "scene_objects/gantthovergraphicsobject.h"

#include "gantt/info/ganttinfoleaf.h"
#include "gantt/info/ganttinfonode.h"

#include <QApplication>
#include <QGraphicsView>

#include <QScrollBar>
#include <QSize>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QtCore/qmath.h>
#include <QScrollBar>
#include <QDebug>


void GanttScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter, rect);

    drawBackgroundExpandedItems(painter, rect);
    drawBackgroundLines(painter, rect);
}


GanttGraphicsObject *GanttScene::itemForInfo(const GanttInfoItem *key) const
{
    return _itemForInfo.value(key);
}


void GanttScene::updateSceneRect()
{
    if(!sceneHaveItems())
        setSceneRect(0,0,sceneRect().width(),0);
    else
        setSceneRect(0,0,sceneRect().width(),_treeInfo->height());

    updateSliderHeight();
}

void GanttScene::updateSceneItems()
{
    updateSlider();
    _dtline->updateCurrentDtPath();
    updateItems();
}

void GanttScene::makeStep(int step)
{
    _playerCurrent->makeStep(step);
}

void GanttScene::moveSliderToNextEventStart()
{
    const GanttInfoItem* nextEventInfo = _treeInfo->nextStart(slidersDt());
    if(nextEventInfo)
    {
        _playerCurrent->setDt(nextEventInfo->start());
    }
    else
        _playerCurrent->moveToEnd();
}

void GanttScene::moveSliderToPrevEventFinish()
{
    const GanttInfoItem* prevEventInfo = _treeInfo->prevFinish(slidersDt());
    if(prevEventInfo)
    {
        _playerCurrent->setDt(prevEventInfo->finish());
    }
    else
        _playerCurrent->moveToBegin();
}

void GanttScene::moveSliderToViewStart()
{
    _playerCurrent->moveToRangeStart();
}

void GanttScene::moveSliderToViewFinish()
{
    _playerCurrent->moveToRangeFinish();
}

void GanttScene::moveSliderToStart()
{
    _playerCurrent->moveToBegin();
}

void GanttScene::setCurrentItemByInfo(GanttInfoItem *info)
{
    setCurrentItem(itemForInfo(info));
}

UtcDateTime GanttScene::slidersDt() const
{
    return _playerCurrent->dt();
}

void GanttScene::setCurrentDt(const UtcDateTime &dt)
{
    _playerCurrent->setDt(dt);
}


void GanttScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if(event->button() == Qt::LeftButton){
        if(!_playerCurrent->sceneBoundingRect().contains(event->scenePos()))
            _mousePressH.press(event->scenePos());
    }

    mouseMoveEvent(event);
}

void GanttScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseDoubleClickEvent(event);
    if(event->buttons() & Qt::LeftButton)
    {
        GanttGraphicsObject *object = objectForPos(event->scenePos());
        if(object)
        {
            GanttInfoNode *node = object->info()->node();
            if (node->parent()){    // not root
                if (node->isExpanded())
                    node->setExpanded(false);
                else{
                    emit currentItemChanged(object->info());
                    node->setExpanded(true);
                }
            }
        }
    }

    mouseMoveEvent(event);
}

void GanttScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton){
        if(_view)
        {
            QRectF viewRect = _view->mapToScene(_view->viewport()->geometry()).boundingRect();
            if(viewRect.contains(event->scenePos()))
            {
                _crossObject->setVisible(true);
                _crossObject->setPos(event->scenePos());
            }
            else
                _crossObject->setVisible(false);
        }
        else
            qCritical("m_view is null");
    }
    if(_mousePressH.isSlide(event->scenePos()))
        _dtline->slide((event->lastScenePos().x() - event->scenePos().x()) * 1. / width());     // HOR scroll

    QGraphicsScene::mouseMoveEvent(event);
}

void GanttScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        _crossObject->setVisible(false);
    }
    else if (event->button() == Qt::LeftButton){
        if(_mousePressH.isClick(event->scenePos())){    // Press
            GanttGraphicsObject *object = objectForPos(_mousePressH.pos());
            if(object){
                setCurrentItem(object);

                GanttInfoNode *node = object->info()->node();
                if (node->parent()){    // not root
                    if (node->isExpanded())
                        emit currentItemChanged(object->info());
                }
            }
        }
    }

    _mousePressH.release();
    QGraphicsScene::mouseReleaseEvent(event);
}

void GanttScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if(QApplication::keyboardModifiers() & Qt::ControlModifier){
        _dtline->zoom(event->delta(), event->scenePos().x() * 1. / width());
        event->accept();
        return; // forbid VSrollBar
    }

    QGraphicsScene::wheelEvent(event);
}

void GanttScene::onTreeInfoReset()
{
    clear();
    addInfoItem(_treeInfo->root());
    updateIntersections();
    updateSceneRect();
}

void GanttScene::connectDtLine()
{
//    connect(_dtline,SIGNAL(timeSpanChanged()),this,SLOT(updateIntersections()), Qt::QueuedConnection);

    connect(_dtline,SIGNAL(minChanged()),this,SLOT(updateSceneItems()), Qt::QueuedConnection);
    connect(_dtline,SIGNAL(timeSpanChanged()),this,SLOT(updateSceneItems()), Qt::QueuedConnection);

}

void GanttScene::onVisItemDestroyed()
{
    updateSceneRect();
}

const MousePressHelper *GanttScene::mousePressH() const
{
    return &_mousePressH;
}

void GanttScene::setTreeInfo(GanttInfoTree *treeInfo)
{
    _treeInfo = treeInfo;
}

void GanttScene::setBuilder(AbstractBuilder *builder)
{
    _builder = builder;
}

QGraphicsItem *GanttScene::currentItem() const
{
    return _currentItem;
}

bool GanttScene::isVisible(const QGraphicsItem *which) const
{
    if(!_view)
        return false;

    QRectF viewRect = _view->mapToScene(_view->viewport()->geometry()).boundingRect();
    QRectF itemBR = which->sceneBoundingRect();

    return viewRect.top() <= itemBR.top() && viewRect.bottom() >= itemBR.bottom();
}

QRectF GanttScene::elementsBoundingRect()
{
    QRectF res;
    foreach(GanttGraphicsObject* object,_items)
        res|=object->sceneBoundingRect();
    return res;
}

void GanttScene::clear()
{
//    qDebug() << "GANTT CLEAR";
    _currentItem = NULL;

    removePersistentItems();
    QGraphicsScene::clear();
    _items.clear();
    _itemForInfo.clear();
    addPersistentItems();
}


bool GanttScene::sceneHaveItems() const
{
    return !_items.isEmpty();
}

void GanttScene::onViewResized(const QSize &newSize)
{
    HFitScene::onViewResized(newSize);

    updateSceneItems();
}

void GanttScene::setCurrentItem(GanttGraphicsObject *currentItem)
{
    if(_currentItem == currentItem)
        return;

    QGraphicsItem *lastItem = _currentItem;
    GanttInfoItem *info = NULL;
    _currentItem = currentItem;
    if(lastItem){
        lastItem->setZValue(_savedZValue);
        lastItem->update();
    }
    if(_currentItem){
        _savedZValue = _currentItem->zValue();

        if(_currentItem)
            info = _currentItem->info();

        QRectF itemRect = _currentItem->mapToScene(_currentItem->boundingRect()).boundingRect();
        if(_view && !isVisible(_currentItem))
        {
            _view->ensureVisible(itemRect, 0, _view->height()/2);
        }
        _currentItem->setZValue(500);
        _currentItem->update();
    }

    _hoverObject->setItem(info);   // if no currentItem sets to NULL
}

GanttGraphicsObject *GanttScene::objectForPos(const QPointF &pos)
{
    GanttGraphicsObject *object;
    foreach(QGraphicsItem *item,items(pos))
        if((object = dynamic_cast<GanttGraphicsObject*>(item)))
            return object;

    return itemForInfo(_treeInfo->infoForVPos(pos.y()));
}

void GanttScene::connectNewInfo(GanttInfoItem *info)
{
    connect(info, SIGNAL(aboutToBeDeleted()), this, SLOT(onInfoDelete()));
    connect(info, SIGNAL(destroyed(QObject*)), this, SLOT(onVisItemDestroyed()));

    connect(info, SIGNAL(expanded()), this, SLOT(updateSceneRect()));
    connect(info, SIGNAL(collapsed()), this, SLOT(updateSceneRect()));

    connect(info, SIGNAL(expanded()), this, SLOT(invalidateBackground()));
    connect(info, SIGNAL(collapsed()), this, SLOT(invalidateBackground()));
}
const QList<GanttGraphicsObject *>& GanttScene::dtItems() const
{
    return _items;
}

void GanttScene::setDrawCurrentDtSlider(bool enable)
{
    _playerCurrent->setDraw(enable);
}


void GanttScene::updateSlider()
{
    _playerCurrent->updateScenePos();
}

void GanttScene::updateItems()
{
    foreach(GanttGraphicsObject *o, _items)
        o->updateItemGeometry();
}

void GanttScene::createPersistentItems()
{
    _playerCurrent = new GanttCurrentDtSlider(this,_dtline);
    _crossObject = new GanttDtCrossObject(this);
    _hoverObject = new GanttHoverGraphicsObject(this);

    connect(_playerCurrent, SIGNAL(dtChanged(UtcDateTime)), this, SIGNAL(currentDtChanged(UtcDateTime)));
    connect(_playerCurrent, SIGNAL(dtChanged(UtcDateTime)), _dtline, SLOT(setCurrentDt(UtcDateTime)));
    connect(_dtline, SIGNAL(dtChangedManually(UtcDateTime)), _playerCurrent, SLOT(setDt(UtcDateTime)));
    connect(_playerCurrent, SIGNAL(drawChanged(bool)), _dtline, SLOT(showCurrentDt(bool)));
}

void GanttScene::addPersistentItems()
{
    addItem(_playerCurrent);
    addItem(_crossObject);
    addItem(_hoverObject);
}

void GanttScene::removePersistentItems()
{
    removeItem(_playerCurrent);
    removeItem(_crossObject);
    removeItem(_hoverObject);
}

void GanttScene::drawBackgroundExpandedItems(QPainter *painter, const QRectF &rect)
{
    qreal   bgLeft = rect.left() - 1 ,
            bgWidth = rect.width() + 2;
    GanttInfoNode *root = _treeInfo->root();
    for(int i = 0; i < root->size(); ++i){
        if(root->at(i)->bottom() < rect.top())
            continue;
        if(root->at(i)->pos() > rect.bottom())
            break;
        GanttInfoNode *node = root->nodeAt(i);
        if(node && node->isExpanded())
            painter->fillRect(QRect(bgLeft, node->pos() + RECTANGLE_OFFSET / 2,
                                    bgWidth, node->height() - RECTANGLE_OFFSET + 2),
                              QBrush(QColor(0xFFE5CC)));

    }
}

void GanttScene::drawBackgroundLines(QPainter *painter, const QRectF &rect)
{
    qreal   bgBottom = rect.bottom() ,
            bgLeft = rect.left() - 1 ,
            bgRight = rect.right() + 1;


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

void GanttScene::invalidateBackground()
{
    invalidate(QRectF(),QGraphicsScene::BackgroundLayer);
}

void GanttScene::addInfoItem(GanttInfoItem *parent)
{
    if(!parent)
        return;
    onItemAdded(parent);
    if(GanttInfoNode *node = qobject_cast<GanttInfoNode*>(parent)){
        for(int i = 0; i < node->size(); ++i){
            addInfoItem(node->at(i));
        }
    }
}

void GanttScene::addInfoItem(GanttInfoNode *parent, int from, int to)
{
    if(!parent)
        return;

    for(int i = from; i <= to; ++i)
        onItemAdded(parent->at(i));

    updateSceneRect();
    updateIntersectionR(parent);
}

void GanttScene::onLimitsChanged(const UtcDateTime &first, const TimeSpan &ts)
{
    _playerCurrent->updateRange(first, ts);
    _playerCurrent->setToBegin();
}

void GanttScene::updateIntersectionR(GanttInfoItem *item)
{
    if(GanttInfoNode *node = qobject_cast<GanttInfoNode*>(item)){
        for(int i = 0; i < node->size(); ++i)
            updateIntersectionR(node->at(i));
    }
    else if(GanttInfoLeaf *leaf = qobject_cast<GanttInfoLeaf*>(item)){
        if(!qobject_cast<GanttIntervalGraphicsObject*>(itemForInfo(leaf))){
//            qDebug() << leaf->title() << leaf;
            foreach(const GanttInfoItem* item,_itemForInfo.keys())
                qDebug() << item->title() << item << _itemForInfo.value(item);
            Q_ASSERT(false);
        }
        ((GanttIntervalGraphicsObject*)itemForInfo(leaf))->updateIntersection();
    }
    else
        qCritical("GanttScene::updateIntersection");
}

void GanttScene::updateIntersections()
{
    updateIntersectionR(_treeInfo->root());
}

void GanttScene::onGraphicsItemPress()
{
    GanttGraphicsObject *graphicsObject = qobject_cast<GanttGraphicsObject*>(sender());
    if(graphicsObject)
        setCurrentItem(graphicsObject);
}

void GanttScene::onGraphicsItemHoverEnter()
{
    GanttGraphicsObject *graphicsObject = qobject_cast<GanttGraphicsObject*>(sender());
    if(graphicsObject)
        emit graphicsItemHoverEnter(graphicsObject->info());
}

void GanttScene::onGraphicsItemHoverLeave()
{
    GanttGraphicsObject *graphicsObject = qobject_cast<GanttGraphicsObject*>(sender());
    if(graphicsObject)
        emit graphicsItemHoverLeave(graphicsObject->info());
}

void GanttScene::onInfoDelete()
{
    GanttInfoItem* item = static_cast<GanttInfoItem*>(sender());
    onItemRemoved(item);
}

void GanttScene::onItemRemoved(GanttInfoItem *info)
{
    qDebug() << "onItemRemoved "<< info->title();
    if(!info)
        return;

    if(GanttGraphicsObject *graphicsObject = _itemForInfo.value(info))
    {
        _items.removeOne(graphicsObject);
        _itemForInfo.remove(info);
        graphicsObject->deleteLater();
    }
}


void GanttScene::onExpanded(GanttInfoNode *which)
{
    updateIntersectionR(which);
}

void GanttScene::onCollapsed(GanttInfoNode *which)
{
    updateIntersectionR(which);
}

void GanttScene::updateSliderHeight()
{
    _playerCurrent->setHeight(height());
}



void GanttScene::init()
{
    setItemIndexMethod(QGraphicsScene::NoIndex);

    connectDtLine();
    _currentItem = NULL;
    _builder = NULL;

    setSceneRect(0,0,GANTTSCENE_MIN_WIDTH,0);

    createPersistentItems();
}

GanttScene::GanttScene(GanttGraphicsView *view, GanttDtLine *dtline, QObject *parent)
    : HFitScene(view, parent),_dtline(dtline)
{
    init();
}

int GanttScene::dtToPos(const UtcDateTime &dt) const
{
    return _dtline->dtToPos(dt);
}

UtcDateTime GanttScene::posToDt(int pos) const
{
    return _dtline->posToDt(pos);
}

void GanttScene::onItemAdded(GanttInfoItem *info)
{
    if(!info)
        return;
    GanttGraphicsObject *p_object = NULL;

    if(_builder)
        p_object = _builder->createGraphicsObject(info);
    else
        qWarning("GanttScene::onItemAdded _builder is NULL");


    if(p_object){
        connectNewInfo(info);
        _items.append(p_object);
        _itemForInfo.insert(info,p_object);

        p_object->setDtLine(_dtline);
        p_object->setScene(this);

        connect(p_object,SIGNAL(graphicsItemHoverEnter()),this,SLOT(onGraphicsItemHoverEnter()));
        connect(p_object,SIGNAL(graphicsItemHoverLeave()),this,SLOT(onGraphicsItemHoverLeave()));
        connect(p_object,SIGNAL(graphicsItemPressed()),this,SLOT(onGraphicsItemPress()));

        p_object->updateItemGeometry();
    }
}

