/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию GanttWidget class.
 * \~englist
 * \brief File contains realization of GanttWidget class.
 */
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
    Q_INIT_RESOURCE(images);

    init();

    ui->playerControl->hide();
    ui->playerSettings->hide();

    if(layout())
    {
        layout()->setMargin(0);
        layout()->setSpacing(0);
    }

    setContextMenuPolicy(Qt::CustomContextMenu);

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

/*!
 * \~russian
 * \brief GanttWidget::setCurrentDt установливает текущее время
 * \param dt новое значение
 * \~english
 * \brief GanttWidget::setCurrentDt sets current time
 * \param dt new value
 */
void GanttWidget::setCurrentDt(const UtcDateTime &dt)
{
    _scene->setCurrentDt(dt);
}

/*!
 * \~russian
 * \brief GanttWidget::setPlayerSpeeds установливает мин./макс. множители плеера
 * \param minMultiply мин.
 * \param maxMultiply макс.
 * \~english
 * \brief GanttWidget::setPlayerSpeeds sets min/max player multiplayers
 * \param minMultiply min.
 * \param maxMultiply max.
 */
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

QAbstractItemModel *GanttWidget::model() const
{
    return _treeInfo->model();
}

UtcDateTime GanttWidget::dtForPos(const QPointF &pos) const
{
    return _scene->posToDt(pos.x());
}

QModelIndex GanttWidget::indexForPos(const QPointF &pos) const
{
    GanttInfoItem *tmp = _treeInfo->infoForVPos(pos.y());
    if (tmp)
        return tmp->index();

    qDebug() << "index not found";
    return QModelIndex();
}

QPoint GanttWidget::mapPosToGlobal(const QPoint &pos) const
{
    return ui->ganttView->mapToGlobal(pos);
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
//    QPoint widgetPoint =ui->ganttView->mapTo(this,point);

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
    connect(ui->ganttView,SIGNAL(customContextMenuRequested(QPoint)),this,SIGNAL(customContextMenuRequested(QPoint)));
}

void GanttWidget::connectSceneWithInfo()
{
    _scene->setTreeInfo(_treeInfo);

    connect(_treeInfo,SIGNAL(currentChanged(GanttInfoItem*)),_scene,SLOT(setCurrentItemByInfo(GanttInfoItem*)));
    connect(_treeInfo,SIGNAL(treeReset()),_scene,SLOT(onTreeInfoReset()));
    connect(_treeInfo,SIGNAL(rowsInserted(GanttInfoItem*,int,int)),_scene,SLOT(addInfoItem(GanttInfoItem*,int,int)));


    connect(_treeInfo,SIGNAL(endRemoveItems()),_scene,SLOT(updateSceneRect()));

    connect(_treeInfo,SIGNAL(expanded(GanttInfoItem*)),_scene,SLOT(onExpanded(GanttInfoItem*)));
    connect(_treeInfo,SIGNAL(collapsed(GanttInfoItem*)),_scene,SLOT(onCollapsed(GanttInfoItem*)));
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

