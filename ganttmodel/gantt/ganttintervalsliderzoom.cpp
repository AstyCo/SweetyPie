#include "ganttintervalsliderzoom.h"

#include <QAction>

#include <QDebug>

GanttIntervalSliderZoom::GanttIntervalSliderZoom(QWidget *parent) :
    QToolBar(parent)
{
    setToolButtonStyle(Qt::ToolButtonIconOnly);

    setStyleSheet("QToolBar { border: 0px }");
    initActions();
}

GanttIntervalSliderZoom::~GanttIntervalSliderZoom()
{
}

void GanttIntervalSliderZoom::initActions()
{
    QAction *p_action;

    p_action = new QAction(QIcon(":/images/Buttons 64x64/reverse-arrow.png"),QString::fromUtf8("Вернуться к старым границам"),this);
    connect(p_action,SIGNAL(triggered(bool)),this,SIGNAL(restoreButtonClicked(bool)));
    addAction(p_action);

    p_action = new QAction(QIcon(":/images/Buttons 64x64/save.png"),QString::fromUtf8("Перейти к новым границам"),this);
    connect(p_action,SIGNAL(triggered(bool)),this,SIGNAL(increaseButtonClicked(bool)));
    addAction(p_action);
}
