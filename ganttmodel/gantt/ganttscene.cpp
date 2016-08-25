#include "ganttscene.h"

#include "ganttgraphicsview.h"
#include "ganttgraphicsitem.h"

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
    setItemIndexMethod(QGraphicsScene::NoIndex);

    setSceneRect(0,0,GANTTSCENE_MIN_WIDTH,0);

    m_header = new GanttHeader;
    m_header->setScene(this);
    m_slider = new GanttSlider;
    m_slider->setScene(this);

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


    qreal   /*bgTop = rect.top()
            ,*/bgBottom = rect.bottom()
            ,bgLeft = rect.left()
            ,bgRight = rect.right();

    QPen pen;
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);

//    qreal  startX = (qCeil(rect.x() / m_header->secondHeaderWidth()))*m_header->secondHeaderWidth();
//    while(startX<bgRight)
//    {
//        painter->drawLine(QPointF(startX,bgTop),QPointF(startX,bgBottom));
//        startX+=m_header->secondHeaderWidth();
//    }

    qreal startY = (qCeil(rect.y() / DEFAULT_ITEM_WIDTH))*DEFAULT_ITEM_WIDTH;

    while(startY<bgBottom)
    {
        painter->drawLine(QPointF(bgLeft,startY),QPointF(bgRight,startY));
        startY+=DEFAULT_ITEM_WIDTH;
    }
}

void GanttScene::setMode(GanttHeader::GanttPrecisionMode newMode)
{
    m_header->setMode(newMode);

    updateItems();
}

void GanttScene::addItems(GanttInfoItem* item)
{
    m_header->onItemsAddition(item);
    addItemsHelper(item);

    QRectF _sceneRect = sceneRect();
    setSceneRect(_sceneRect.x(),_sceneRect.y(),_sceneRect.width()
                 ,_sceneRect.height() + DEFAULT_ITEM_HEIGHT);
}

void GanttScene::addItems(const QList<GanttInfoItem *> &items)
{
    m_header->onItemsAddition(items);

    foreach(GanttInfoItem* item, items)
        addItemsHelper(item);

    QRectF _sceneRect = sceneRect();
    setSceneRect(_sceneRect.x(),_sceneRect.y(),_sceneRect.width()
                 ,_sceneRect.height() + items.count() * DEFAULT_ITEM_HEIGHT);
}

GanttGraphicsItem *GanttScene::itemByInfo(const GanttInfoLeaf * key) const
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

void GanttScene::onViewAdded(QGraphicsView* view)
{
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
            onViewAdded(views()[0]);
            views()[0]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
}

GanttHeader::GanttHeaderMode GanttScene::headerMode() const
{
    if(!m_header)
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
    if(!m_header || !m_slider)
        return UtcDateTime();

    return m_slider->dt();
}


void GanttScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    m_header->setPos(event->scenePos());
    QGraphicsScene::mousePressEvent(event);
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

void GanttScene::updateSlider()
{
    if(!m_slider)
        return;

    m_slider->updateScenePos();
}


void GanttScene::addItemsHelper(GanttInfoItem *item)
{
    GanttInfoLeaf *leaf = dynamic_cast<GanttInfoLeaf*>(item);
    if(leaf)
    {
        GanttGraphicsItem *p_item = new GanttGraphicsItem(leaf);


        p_item->setScene(this);
        p_item->setHeader(m_header);

        m_items.append(p_item);
        m_itemByInfo.insert(leaf,p_item);
        m_infoByStart.insert(leaf->start(),leaf);
        connect(leaf,SIGNAL(startChanged()),this,SLOT(onLeafStartChanged()));
        m_infoByFinish.insert(leaf->finish(),leaf);
        connect(leaf,SIGNAL(finishChanged()),this,SLOT(onLeafFinishChanged()));
        p_item->updateItemGeometry();

        connect(leaf,SIGNAL(changed()),this,SLOT(onInfoChanged()));

    }
    else
    {
        GanttInfoNode *node = dynamic_cast<GanttInfoNode*>(item);
        if(node)
        {
            foreach(GanttInfoItem* item, node->m_items)
                addItemsHelper(item);
        }
    }
}

void GanttScene::updateItems()
{

    for(int i = 0; i < m_items.size(); ++i)
    {
        GanttInfoLeaf* p_info = m_items[i]->info();
        qreal startPos = m_header->dtToX(p_info->start()),
              itemWidth = m_header->dtToX(p_info->finish()) - startPos;
        qreal top = m_items[i]->rect().top(),
                height = m_items[i]->rect().height();


        m_items[i]->setPos(startPos, top);
        m_items[i]->setBoundingRectSize(QSizeF(itemWidth, height));
    }


    update();
}

void GanttScene::onInfoChanged()
{
    GanttInfoLeaf * leaf = dynamic_cast<GanttInfoLeaf*>(sender());
    if(!leaf)
        return;

    bool newRange = false;
    if(m_header->verifyBoundsByLeaf(leaf))
        m_header->updateHeader();
}

GanttSlider *GanttScene::slider() const
{
    return m_slider;
}

void GanttScene::setRange(UtcDateTime min, UtcDateTime max)
{
    m_header->setRange(min,max);
    updateItems();
    updateSlider();
}

UtcDateTime GanttScene::minDt() const
{
    if(!m_header)
        return UtcDateTime();
    return m_header->minDt();
}

UtcDateTime GanttScene::maxDt() const
{
    if(!m_header)
        return UtcDateTime();
    return m_header->maxDt();
}

UtcDateTime GanttScene::startDt() const
{
    if(!m_header)
        return UtcDateTime();
    return m_header->startDt();
}

UtcDateTime GanttScene::finishDt() const
{
    if(!m_header)
        return UtcDateTime();
    return m_header->finishDt();
}

UtcDateTime GanttScene::xToDt(qreal x) const
{
    if(!m_header)
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return m_header->xToDt(x);
}

qreal GanttScene::dtToX(const UtcDateTime &dt) const
{
    if(!m_header)
    {
        Q_ASSERT(false);
        return 0;
    }

    return m_header->dtToX(dt);
}

UtcDateTime GanttScene::startByDt(const UtcDateTime &dt,GanttHeader::GanttPrecisionMode mode) const
{
    if(!m_header)
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return m_header->startByDt(dt,mode);
}

UtcDateTime GanttScene::startByDt(const UtcDateTime &dt) const
{
    if(!m_header)
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return startByDt(dt, m_header->mode());
}

UtcDateTime GanttScene::finishByDt(const UtcDateTime &dt,GanttHeader::GanttPrecisionMode mode) const
{
    if(!m_header)
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return m_header->finishByDt(dt,mode);
}

UtcDateTime GanttScene::finishByDt(const UtcDateTime &dt) const
{
    if(!m_header)
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return finishByDt(dt, m_header->mode());
}

UtcDateTime GanttScene::nextStart(const UtcDateTime &dt,GanttHeader::GanttPrecisionMode mode) const
{
    if(!m_header)
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return m_header->nextStart(dt,mode);
}

UtcDateTime GanttScene::nextStart(const UtcDateTime &dt) const
{
    if(!m_header)
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return nextStart(dt, m_header->mode());
}

UtcDateTime GanttScene::prevFinish(const UtcDateTime &dt,GanttHeader::GanttPrecisionMode mode) const
{
    if(!m_header)
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return m_header->prevFinish(dt,mode);
}

UtcDateTime GanttScene::prevFinish(const UtcDateTime &dt) const
{
    if(!m_header)
    {
        Q_ASSERT(false);
        return UtcDateTime();
    }

    return prevFinish(dt, m_header->mode());
}

GanttHeader::GanttPrecisionMode GanttScene::headerPrecisionMode() const
{
    if(!m_header)
    {
        qWarning("GanttScene::headerMode m_header is NULL");
        return (GanttHeader::GanttPrecisionMode)0;
    }
    return m_header->mode();
}

GanttHeader::GanttPrecisionMode GanttScene::calculateTimeMode(const UtcDateTime &min, const UtcDateTime &max) const
{
    if(!m_header)
    {
        qWarning("GanttScene::calculateTimeMode m_header is NULL");
        return (GanttHeader::GanttPrecisionMode)0;
    }
    return m_header->calculateTimeMode(min,max);
}

void GanttScene::emitLimitsChanged(const UtcDateTime &start, const UtcDateTime &finish)
{
    emit limitsChanged(start,finish);
}

long long GanttScene::minTimeUnit() const
{
    if(!m_header)
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

void GanttScene::updateSliderRect()
{
    if(!m_slider || !m_header)
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

