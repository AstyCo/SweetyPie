#include "ganttwidget.h"
#include "ui_ganttwidget.h"

#include "ganttgraphicsview.h"
#include "ganttscene.h"
#include "gantttreemodel.h"

#include "ganttinfoleaf.h"
#include "ganttinfonode.h"

#include "ganttintervalgraphicsobject.h"
#include "ganttcalcgraphicsobject.h"

#include <QScrollBar>

QList<GanttInfoItem*> generateTest();

GanttWidget::GanttWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GanttWidget)
{
    ui->setupUi(this);

    ui->playerControl->hide();
    ui->playerSettings->hide();

    if(layout())
    {
        layout()->setMargin(0);
        layout()->setSpacing(0);
    }

    ui->treeView->setContentsMargins(0,0,0,0);
    ui->treeView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->treeView->verticalScrollBar()->setStyleSheet("QScrollBar {width:0px;}");

    ui->treeView->setGraphicsView(ui->ganttView);
    ui->ganttView->setTreeView(ui->treeView);

    m_scene = new GanttScene(this);
    m_model = new GanttTreeModel(NULL,this);

    m_scene->setModel(m_model);

    ui->ganttView->setScene(m_scene);
    ui->treeView->setModel(m_model);

    ui->intervalSlider->setScene(m_scene);
    installEventFilter(ui->intervalSlider);

    ui->playerControl->setSettings(ui->playerSettings);

    connect(m_scene,SIGNAL(graphicsItemHoverEnter(const GanttInfoItem*)),this,SIGNAL(graphicsItemHoverEnter(const GanttInfoItem*)));
    connect(m_scene,SIGNAL(graphicsItemHoverLeave(const GanttInfoItem*)),this,SIGNAL(graphicsItemHoverLeave(const GanttInfoItem*)));
    connect(m_scene,SIGNAL(currentItemChanged(const GanttInfoItem*)),this,SIGNAL(currentItemChanged(const GanttInfoItem*)));


    connect(m_scene->slider(),SIGNAL(dtChanged(UtcDateTime)),ui->intervalSlider,SLOT(setCurrentTime(UtcDateTime)));
    connect(m_scene->slider(),SIGNAL(drawChanged(bool)),ui->intervalSlider,SLOT(setDrawCurrentDt(bool)));
    connect(m_scene,SIGNAL(currentDtChanged(UtcDateTime)),this,SIGNAL(currentDtChanged(UtcDateTime)));


    connect(ui->ganttView, SIGNAL(viewResized(QSize)),m_scene,SLOT(onViewResize(QSize)));
    connect(ui->ganttView, SIGNAL(viewResized(QSize)),ui->intervalSlider,SLOT(updateRange()));
    connect(ui->ganttView, SIGNAL(viewResized(QSize)),ui->intervalSlider,SLOT(updateMinTimeSize(QSize)));
    connect(ui->ganttView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onGanttViewCustomContextMenuRequested(QPoint)));


    connect(ui->treeView,SIGNAL(clicked(QModelIndex)), this, SLOT(onTreeViewEntered(QModelIndex)));
    connect(ui->treeView,SIGNAL(expanded(QModelIndex)), this,SLOT(expanded(QModelIndex)));
    connect(ui->treeView,SIGNAL(collapsed(QModelIndex)), this,SLOT(collapsed(QModelIndex)));
//    connect(m_scene->slider(),SIGNAL(sliderPosChanged(qreal)),this,SLOT(repaintDtHeader()));

    connect(ui->intervalSlider,SIGNAL(beginMoved(long long)),this, SLOT(onSliderMoved()));
    connect(ui->intervalSlider,SIGNAL(endMoved(long long)),this, SLOT(onSliderMoved()));

    connect(ui->intervalSlizedZoom,SIGNAL(increaseButtonClicked(bool)),this,SLOT(newLimits()));
    connect(ui->intervalSlizedZoom,SIGNAL(restoreButtonClicked(bool)),this,SLOT(prevLimits()));

    connect(ui->playerControl,SIGNAL(makeStep(int)),m_scene,SLOT(makeStep(int)));
    connect(ui->playerControl,SIGNAL(goToNextEventStart()),m_scene,SLOT(moveSliderToNextEventStart()));
    connect(ui->playerControl,SIGNAL(goToPrevEventFinish()),m_scene,SLOT(moveSliderToPrevEventFinish()));
    connect(ui->playerControl,SIGNAL(goToViewStart()),m_scene,SLOT(moveSliderToViewStart()));
    connect(ui->playerControl,SIGNAL(goToViewFinish()),m_scene,SLOT(moveSliderToViewFinish()));
    connect(ui->playerControl,SIGNAL(stop()),m_scene,SLOT(moveSliderToStart()));



    connect(m_scene,SIGNAL(currentDtChanged(UtcDateTime)),ui->treeView,SLOT(repaintHeader()));

}

GanttWidget::~GanttWidget()
{
    delete ui;
}

void GanttWidget::setPlayerSpeeds(qreal minMultiply, qreal maxMultiply)
{
    ui->playerSettings->setMultiplies(minMultiply,maxMultiply);
}

void GanttWidget::showPlayer(bool show)
{
    m_playerShown = show;
    ui->playerControl->setVisible(show);
    ui->playerSettings->setVisible(show);

    if(m_scene)
        m_scene->setDrawCurrentDtSlider(show);

    update();
}

bool GanttWidget::player() const
{
    return m_playerShown;
}

void GanttWidget::addItems(GanttInfoItem* item)
{
    if(!m_scene)
    {
        Q_ASSERT(false);
        return;
    }

    m_model->addItems(item);

    updateMinMax(m_model->root());
    m_scene->m_header->setRange(m_minDt,m_maxDt);

    m_scene->addItems(item);
    ui->intervalSlider->updateRange();

    callForEachItem(item,ui->treeView,m_scene,
                    &connectSignalsToNewItems);
}

void GanttWidget::addItems(const QList<GanttInfoItem *> &items)
{
    m_model->addItems(items);

    updateMinMax(m_model->root());
    m_scene->m_header->setRange(m_minDt,m_maxDt);

    m_scene->addItems(items);
    ui->intervalSlider->updateRange();

    foreach(GanttInfoItem* item,items)
        callForEachItem(item,ui->treeView,m_scene,
                        &connectSignalsToNewItems);
}



UtcDateTime GanttWidget::slidersDt() const
{
    if(!m_scene)
        return UtcDateTime();
    return m_scene->slidersDt();
}

//void GanttWidget::on_comboBox_mode_currentIndexChanged(int index)
//{
//    GanttHeader::GanttPrecisionMode newMode;

//    switch(index)
//    {
//    case 0:
//        newMode = GanttHeader::months1;
//        break;
//    case 1:
//        newMode = GanttHeader::days1;
//        break;
//    case 2:
//        newMode = GanttHeader::hours1;
//        break;
//    case 3:
//        newMode = GanttHeader::minutes1;
//        break;
//    case 4:
//        newMode = GanttHeader::seconds1;
//        break;
//    default:
//        return;
//    }

//    m_scene->setMode(newMode);


//}

void GanttWidget::expanded(const QModelIndex &index)
{
    GanttInfoNode * node = m_model->nodeForIndex(index);
    if(node)
    {
        node->setIsExpanded(true);
        updatePos(node);
    }
}

void GanttWidget::collapsed(const QModelIndex &index)
{
    GanttInfoNode * node = m_model->nodeForIndex(index);
    if(node)
    {
        node->setIsExpanded(false);
        updatePos(node);
    }
}


void GanttWidget::onSliderMoved()
{
    if(!m_scene || !m_scene->m_header)
        return;

    m_scene->setRange(ui->intervalSlider->beginDt(),ui->intervalSlider->endDt());
}

void GanttWidget::onGanttViewCustomContextMenuRequested(const QPoint &point)
{
    QPoint widgetPoint =ui->ganttView->mapTo(this,point);

    emit customContextMenuRequested(point);
}

void GanttWidget::updateMinMax(const GanttInfoItem *root)
{
    QPair<UtcDateTime,UtcDateTime> limits = GanttInfoItem::getLimits(root);

    clearStack();
    m_minDt = limits.first;
    m_maxDt = limits.second;
    updateSliderLimits();
}

void GanttWidget::updatePos(GanttInfoNode *from)
{
    m_curSceneMax = 0;

    GanttInfoNode* p_parent;

    while((p_parent=from->parent())!=m_model->m_root)
        from = from->parent();

    for(int i = m_model->m_root->indexOf(from); i < m_model->m_root->size(); ++i )
    {
        updatePosHelper(m_model->m_root->m_items[i]);
    }

    m_scene->updateHeight(m_curSceneMax);

    m_scene->update();
}

void GanttWidget::updatePosHelper(GanttInfoItem *item)
{
    GanttInfoLeaf *leaf = qobject_cast<GanttInfoLeaf*>(item);
    if(leaf)
    {
        GanttIntervalGraphicsObject *graphicsItem = qobject_cast<GanttIntervalGraphicsObject *>(m_scene->itemByInfo(leaf));
        if(graphicsItem)
        {
            graphicsItem->setPos(graphicsItem->scenePos().x(),2*DEFAULT_ITEM_WIDTH + leaf->pos());
            if(graphicsItem->sceneBoundingRect().bottom()>m_curSceneMax)
                m_curSceneMax = graphicsItem->sceneBoundingRect().bottom();
        }
    }
    else
    {
        GanttInfoNode *node = qobject_cast<GanttInfoNode*>(item);
        if(node)
        {
            GanttCalcGraphicsObject *graphicsItem = qobject_cast<GanttCalcGraphicsObject *>(m_scene->itemByInfo(node));
            if(graphicsItem)
            {
                graphicsItem->setPos(graphicsItem->scenePos().x(),2*DEFAULT_ITEM_WIDTH + node->pos());
                if(graphicsItem->sceneBoundingRect().bottom()>m_curSceneMax)
                    m_curSceneMax = graphicsItem->sceneBoundingRect().bottom();
            }

            foreach(GanttInfoItem* item, node->m_items)
                updatePosHelper(item);
        }
    }
}

void GanttWidget::connectSignalsToNewItems(GanttInfoItem *item,GanttTreeView* view,GanttScene* scene)
{
    if(!item)
        return;

    connect(item,SIGNAL(aboutToBeDeleted()),scene,SLOT(onInfoDelete()));


    GanttInfoLeaf *leaf = qobject_cast<GanttInfoLeaf*>(item);

    if(leaf)
    {
        if(scene)
        {
            connect(leaf,SIGNAL(startChanged()),scene,SLOT(onLeafStartChanged()));
            connect(leaf,SIGNAL(finishChanged()),scene,SLOT(onLeafFinishChanged()));
            connect(leaf,SIGNAL(changed()),scene,SLOT(onInfoChanged()));

//            connect(leaf,SIGNAL(aboutToBeDeleted()),scene,SLOT(onInfoLeafDelete()));

        }

    }
}

UtcDateTime GanttWidget::maxDt() const
{
    return m_maxDt;
}

const UtcDateTime &GanttWidget::outerMaxDt() const
{
    if(m_stackLimits.isEmpty())
        return maxDt();
    return m_stackLimits.at(0).second;
}

GanttInfoNode *GanttWidget::nodeByName(const QString &title) const
{
    return dynamic_cast<GanttInfoNode*>(m_model->itemForName(title));
}

void GanttWidget::callForEachItem(GanttInfoItem *item,GanttTreeView* view,GanttScene* scene,
                                  void (*func)(GanttInfoItem *,GanttTreeView*,GanttScene*))
{
    if(!item)
        return;
    (*func)(item,view,scene);

    GanttInfoNode *p_node = qobject_cast<GanttInfoNode*>(item);
    if(p_node)
    {
        for(int i = 0; i < p_node->size(); ++i)
            callForEachItem(p_node->child(i) , view, scene
                            ,func);
    }
}

void GanttWidget::clear()
{
    if(m_model)
        m_model->clear();
    if(m_scene)
        m_scene->clear();


    //    ui->treeView->update();
}

GanttInfoItem *GanttWidget::itemAtPos(const QPoint &widgetPoint) const
{
    QPointF scenePos = ui->ganttView->mapToScene(ui->ganttView->mapFrom(
                                        const_cast<QWidget*>(reinterpret_cast<const QWidget*>(this)),widgetPoint));

    if(QGraphicsScene* scene = ui->ganttView->scene())
        if(QGraphicsItem*item= scene->itemAt(scenePos))
        {
            if(GanttIntervalGraphicsObject *rectObject = dynamic_cast<GanttIntervalGraphicsObject*>(item))
                return rectObject->info();
            if(GanttCalcGraphicsObject *calcObject = dynamic_cast<GanttCalcGraphicsObject*>(item))
                return calcObject->info();
        }

    return NULL;
}


UtcDateTime GanttWidget::minDt() const
{
    return m_minDt;
}

const UtcDateTime &GanttWidget::outerMinDt() const
{
    if(m_stackLimits.isEmpty())
        return minDt();
    return m_stackLimits.at(0).first;
}



void GanttWidget::updateRange(const UtcDateTime& min, const UtcDateTime& max)
{
    if(!m_scene)
        return;

    m_minDt = min;
    m_maxDt = max;

    m_scene->update();

    if(m_scene->m_slider)
    {
        if(!m_scene->m_slider->initialized())
        {
            m_scene->m_slider->updateRange(m_minDt,m_maxDt);
            m_scene->m_slider->setDt(m_minDt);
        }
    }
    updateSliderLimits();
}

void GanttWidget::updateSliderLimits()
{
    GanttHeader::GanttPrecisionMode mode = m_scene->calculateTimeMode(m_minDt,m_maxDt);

    ui->intervalSlider->setLimits(m_scene->startByDt(m_minDt,mode).toMicrosecondsSinceEpoch(),
                                    m_scene->finishByDt(m_maxDt,mode).toMicrosecondsSinceEpoch());

}

void GanttWidget::newLimits()
{
    if(!m_scene)
        return;

    newLimits(m_scene->minDt(),m_scene->maxDt());
}

void GanttWidget::newLimits(const UtcDateTime &min, const UtcDateTime &max)
{
    if(min == m_minDt && max == m_maxDt)
        return;

    pushLimits();
    updateRange(min,max);
}

void GanttWidget::prevLimits()
{
    if(ui->intervalSlider->beginHandle()!=ui->intervalSlider->minValue()
            || ui->intervalSlider->endHandle()!=ui->intervalSlider->maxValue())
    {
        updateSliderLimits();
        return;
    }

    if(m_stackLimits.isEmpty())
        return;

    std::pair<UtcDateTime,UtcDateTime> limits = m_stackLimits.last();
    m_stackLimits.pop_back();

    updateRange(limits.first,limits.second);
}

void GanttWidget::clearStack()
{
    if(!m_stackLimits.isEmpty())
    {
        m_minDt = outerMinDt();
        m_maxDt = outerMaxDt();
        m_stackLimits.clear();
    }
}

bool GanttWidget::setCurrentDt(const UtcDateTime &curDt)
{
    if(m_scene&&m_scene->slider())
        return m_scene->slider()->setDt(curDt);
    return false;
}

void GanttWidget::setCurrentItem(const GanttInfoItem *info)
{
    if(m_scene)
    {
        QGraphicsObject *item = m_scene->itemByInfo(info);
        if(item)
            m_scene->setCurrentItem(item);
    }
}

void GanttWidget::onTreeViewEntered(const QModelIndex &index)
{
    if(m_model)
    {
        setCurrentItem(m_model->itemForIndex(index));
    }
}

void GanttWidget::pushLimits()
{
    m_stackLimits.append(std::make_pair(m_minDt,m_maxDt));
}

QList<GanttInfoItem*> generateTest()
{
    QList<GanttInfoItem*> testList;
    for(int i = 0; i<5000; ++i)
    {
        GanttInfoNode *node = new GanttInfoNode;

        node->setTitle("node"+QString::number(i));

        QColor color = QColor::fromRgb(qrand()%255,qrand()%255,qrand()%255);
        int max = qrand()%4;

        for(int j = 0; j<max; ++j)
        {
            GanttInfoLeaf *leaf = new GanttInfoLeaf;

            int year = 2016,
                    month = 1 /*+ qrand()%6*/,
                    day = 1 + qrand()%2,
                    hour = qrand()%2,
                    minute = qrand()%60,
                    sec = qrand()%60,
                    microsec = qrand()%1000000;

            UtcDateTime start(year,month,day,hour,minute,sec,microsec),
                    finish = start
                        .addMicroseconds((hour * SECONDS_IN_HOUR * _MICROSECONDS_IN_SECOND)
                                         + minute * SECONDS_IN_MINUTE * _MICROSECONDS_IN_SECOND
                                         + sec * _MICROSECONDS_IN_SECOND
                                         + microsec)
                        .addDays(day - 1)
                        .addMonths(month - 1);

            leaf->setStart(start);
            leaf->setFinish(finish);
            leaf->setColor(color);
            leaf->setTitle("leaf"+QString::number(1 + i) + ':' + QString::number(1 + j));

            node->append(leaf);
        }


        testList.append(node);
    }

    return testList;
}
