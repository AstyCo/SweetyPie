#include "ganttplayersettings.h"

#include <QMenu>
#include <QToolButton>

#include <QDebug>

GanttPlayerSettings::GanttPlayerSettings(QWidget *parent) :
    QToolBar(parent)
{

    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setStyleSheet("QToolBar { border: 0px }");

    initActions();
}

GanttPlayerSettings::~GanttPlayerSettings()
{
}

void GanttPlayerSettings::initActions()
{
    QAction *p_action;

    p_action = new QAction(QIcon(":/images/Buttons 64x64/reg_speed_on_64.png"),QString::fromUtf8("Выбор скорости воспроизведения"),this);

    QMenu *p_menu = new QMenu(this);

    QAction *p_menuAction = new QAction(QString::fromUtf8("скорость x1"),p_menu);
    connect(p_menuAction,SIGNAL(triggered()),this,SIGNAL(speedX1()));
    p_menu->addAction(p_menuAction);

    p_menuAction = new QAction(QString::fromUtf8("скорость x3"),p_menu);
    connect(p_menuAction,SIGNAL(triggered()),this,SIGNAL(speedX3()));
    p_menu->addAction(p_menuAction);

    p_menuAction = new QAction(QString::fromUtf8("скорость x5"),p_menu);
    connect(p_menuAction,SIGNAL(triggered()),this,SIGNAL(speedX5()));
    p_menu->addAction(p_menuAction);


    p_action->setMenu(p_menu);
    addAction(p_action);
    QToolButton *p_button = qobject_cast<QToolButton*>(widgetForAction(p_action));
    if(p_button)
        p_button->setPopupMode(QToolButton::InstantPopup);

    p_action = new QAction(QIcon(":/images/Buttons 64x64/reg_step_on_64.png"),QString::fromUtf8("выбор шага воспроизведения"),this);
    addAction(p_action);
}

