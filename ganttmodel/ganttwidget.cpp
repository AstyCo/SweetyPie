#include "ganttwidget.h"
#include "ui_ganttwidget.h"

#include "gantt/plot/ganttscene.h"

#include <QScrollBar>
#include <QAbstractItemModel>
#include <QSpacerItem>


GanttWidget::GanttWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GanttWidget)
{
    ui->setupUi(this);
    init();

    ui->playerControl->hide();
    ui->playerSettings->hide();

    if(layout())
    {
        layout()->setMargin(0);
        layout()->setSpacing(0);
    }

//    ui->treeView->setContentsMargins(0,0,0,0);
//    ui->treeView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
//    ui->treeView->verticalScrollBar()->setStyleSheet("QScrollBar {width:0px;}");

    ui->playerControl->setSettings(ui->playerSettings);

}

GanttWidget::~GanttWidget()
{
    delete ui;
    if(_factory)
        delete _factory;
}


void GanttWidget::setCurrentDt(const UtcDateTime &dt)
{
    _scene->setCurrentDt(dt);
}

void GanttWidget::setPlayerSpeeds(qreal minMultiply, qreal maxMultiply)
{
    ui->playerSettings->setMultiplies(minMultiply,maxMultiply);
}

void GanttWidget::setVisiblePlayer(bool show)
{
    m_playerShown = show;

    ui->playerControl->setVisible(show);
    ui->playerSettings->setVisible(show);
    ui->widgetIntervalSlider->setDrawCurrentDt(show);
    _scene->setDrawCurrentDtSlider(show);

    update();
}

bool GanttWidget::player() const
{
    return m_playerShown;
}

void GanttWidget::setFactory(AbstractGanttFactory *factory)
{
    _scene->setFactory(factory);
    _treeInfo->setFactory(factory);

    if(_factory)
        delete _factory;
    _factory = factory;
}

void GanttWidget::setView(QTreeView *view, bool /*inner*/)
{
    if(!_treeInfo || !_treeInfo->model())
        return;

    _treeInfo->connectTreeView(view);
}

void GanttWidget::showInterval()
{
    setVisibleInterval(true);
}

void GanttWidget::hideInterval()
{
    setVisibleInterval(false);
}

void GanttWidget::setVisibleInterval(bool value)
{
    ui->widgetIntervalSlider->setVisible(value);
}

void GanttWidget::showDtLine()
{
    setVisibleDtLine(true);
}

void GanttWidget::hideDtLine()
{
    setVisibleDtLine(false);
}

void GanttWidget::setVisibleDtLine(bool value)
{
    ui->widgetDtLine->setVisible(value);
}

void GanttWidget::showPlayer()
{
    setVisiblePlayer(true);
}

void GanttWidget::hidePlayer()
{
    setVisiblePlayer(false);
}

void GanttWidget::dataReset()
{
    _treeInfo->reset();
}

void GanttWidget::onGanttViewCustomContextMenuRequested(const QPoint &point)
{
    QPoint widgetPoint =ui->ganttView->mapTo(this,point);

    emit customContextMenuRequested(point);
}

void GanttWidget::onGanttViewMaximumHeightChanged()
{
    setMaximumHeight(ui->ganttView->maximumHeight()
                     + ui->widgetDtLine->height()
                     + ui->widgetIntervalSlider->height() );
}

void GanttWidget::connectPlayer()
{
    connect(ui->playerControl,SIGNAL(makeStep(int)),_scene,SLOT(makeStep(int)));
    connect(ui->playerControl,SIGNAL(goToNextEventStart()),_scene,SLOT(moveSliderToNextEventStart()));
    connect(ui->playerControl,SIGNAL(goToPrevEventFinish()),_scene,SLOT(moveSliderToPrevEventFinish()));
    connect(ui->playerControl,SIGNAL(goToViewStart()),_scene,SLOT(moveSliderToViewStart()));
    connect(ui->playerControl,SIGNAL(goToViewFinish()),_scene,SLOT(moveSliderToViewFinish()));
    connect(ui->playerControl,SIGNAL(stop()),_scene,SLOT(moveSliderToStart()));
}

void GanttWidget::init()
{
    _factory = NULL;
    _treeInfo = new GanttInfoTree(this);
    _scene = new GanttScene(ui->ganttView,ui->widgetDtLine,this);
    ui->ganttView->setMousePressH(_scene->mousePressH());
    connectSceneWithInfo();
    connectIntervals();
    connectPlayer();

    connect(ui->ganttView,SIGNAL(maximumHeightChanged()),this,SLOT(onGanttViewMaximumHeightChanged()));
}

void GanttWidget::connectSceneWithInfo()
{
    _scene->setTreeInfo(_treeInfo);

    connect(_treeInfo,SIGNAL(currentChanged(GanttInfoItem*)),_scene,SLOT(setCurrentItemByInfo(GanttInfoItem*)));
    connect(_treeInfo,SIGNAL(treeReset()),_scene,SLOT(onTreeInfoReset()));
    connect(_treeInfo,SIGNAL(rowsInserted(GanttInfoNode*,int,int)),_scene,SLOT(addInfoItem(GanttInfoNode*,int,int)));


    connect(_treeInfo,SIGNAL(endRemoveItems()),_scene,SLOT(updateSceneRect()));

    connect(_treeInfo,SIGNAL(expanded(GanttInfoNode*)),_scene,SLOT(onExpanded(GanttInfoNode*)));
    connect(_treeInfo,SIGNAL(collapsed(GanttInfoNode*)),_scene,SLOT(onCollapsed(GanttInfoNode*)));
    connect(_treeInfo,SIGNAL(limitsChanged(UtcDateTime,TimeSpan)),_scene,SLOT(onLimitsChanged(UtcDateTime,TimeSpan)));


    connect(_scene,SIGNAL(currentItemChanged(const GanttInfoItem*)),_treeInfo,SLOT(onCurrentItemChanged(const GanttInfoItem*)));
}

void GanttWidget::connectIntervals()
{   //DtLine shows a bit more than GanttIntervalSlider
    connect(_treeInfo,SIGNAL(limitsChanged(UtcDateTime,TimeSpan)),
            ui->widgetIntervalSlider,SLOT(setLimitsWithOffset(UtcDateTime,TimeSpan)));
    connect(_treeInfo,SIGNAL(limitsChanged(UtcDateTime,TimeSpan)),
            ui->widgetIntervalSlider,SLOT(resetHandlesManually()));
    connect(_treeInfo,SIGNAL(limitsChanged(UtcDateTime,TimeSpan)),
            ui->widgetDtLine,SLOT(setLimitsWithOffset(UtcDateTime,TimeSpan)));

    connect(ui->widgetIntervalSlider,SIGNAL(rangeChangedManually(UtcDateTime,TimeSpan)),
                ui->widgetDtLine,SLOT(setLimits(UtcDateTime,TimeSpan)));
    connect(ui->widgetDtLine,SIGNAL(rangeChangedManually(UtcDateTime,TimeSpan)),
                ui->widgetIntervalSlider,SLOT(setRangeWithExpansion(UtcDateTime,TimeSpan)));

    connect(ui->widgetDtLine,SIGNAL(dtChanged(UtcDateTime)),
                ui->widgetIntervalSlider,SLOT(setCurrentTime(UtcDateTime)));
}

