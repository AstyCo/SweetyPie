#include "chartactionstoolbar.h"

#include <QAction>

ChartActionsToolBar::ChartActionsToolBar(QWidget *parent)
  :QToolBar(parent)
{
  initActions();
}

void ChartActionsToolBar::setChartActions(uint chartActions)
{
  m_chartActions = chartActions;

  // TODO блокировки самих элементов и функций на графике
  m_toolButton_AutoZoom->setVisible(m_chartActions & caScale);
  m_toolButton_Grid->setVisible(m_chartActions & caGrid);
  m_toolButton_IntervalVisibled->setVisible(m_chartActions & caPaintIntervals);
  m_toolButton_SelectInterval->setVisible(m_chartActions & caSelectIntervals);
  m_toolButton_MaxMinValues->setVisible(m_chartActions & caMaxMinLines);
  m_toolButton_TimerOnline->setVisible(m_chartActions & caTimer);
  m_toolButton_SelectTarget->setVisible(m_chartActions & caSelectTarget);
}

void ChartActionsToolBar::setChartActionVisible(ChartActions type, bool vis)
{
  QAction *act = getChartAction(type);
  if (act == NULL)
    return;

  if (vis)
  {
    act->setVisible(m_chartActions & type);
  }
  else
    act->setVisible(false);
}

QAction *ChartActionsToolBar::getChartAction(ChartActions type) const
{
  switch(type)
  {
    case caScale:
      return m_toolButton_AutoZoom;
    case caGrid:
      return m_toolButton_Grid;
    case caPaintIntervals:
      return m_toolButton_IntervalVisibled;
    case caSelectIntervals:
      return m_toolButton_SelectInterval;
    case caMaxMinLines:
      return m_toolButton_MaxMinValues;
    case caTimer:
      return m_toolButton_TimerOnline;
    case caSelectTarget:
      return m_toolButton_SelectTarget;
    default:
      return NULL;
  }
}

void ChartActionsToolBar::initActions()
{
  m_toolButton_AutoZoom = new QAction(QIcon(":/icons/icons/find.png"),
                                      tr("Автоматически подобрать масштаб"), this);
  m_toolButton_Grid = new QAction(QIcon(":/icons/icons/grid.png"), tr("Сетка"), this);
  m_toolButton_Grid->setCheckable(true);
  m_toolButton_IntervalVisibled = new QAction(QIcon(":/icons/icons/interval_visible.png"),
                                              tr("Вкл./Выкл интервалов"), this);

  m_toolButton_SelectInterval = new QAction(QIcon(":/icons/icons/selection_interval.png"),
                                            tr("Выбрать интервал времени"), this);
  m_toolButton_SelectInterval->setCheckable(true);
  m_toolButton_MaxMinValues = new QAction(QIcon(":/icons/icons/max_min.png"), tr("Настройка границ"), this);

  m_toolButton_TimerOnline = new QAction(QIcon(":/icons/icons/start.png"), tr("Обновление по времени"), this);
  m_toolButton_TimerOnline->setCheckable(true);
  m_toolButton_SelectTarget = new QAction(QIcon(":/icons/icons/target.png"),
                                          tr("Выбрать точку прицеливания"), this);
  m_toolButton_SelectTarget->setCheckable(true);
  m_toolButton_Clear = new QAction(QIcon(":/icons/icons/clear.png"),
                                   tr("Сбросить все настройки по-умолчанию"), this);

  QList<QAction *> actions;
  actions << m_toolButton_AutoZoom << m_toolButton_SelectInterval << m_toolButton_SelectTarget
          << m_toolButton_Grid << m_toolButton_IntervalVisibled
          << m_toolButton_MaxMinValues << m_toolButton_TimerOnline
          << m_toolButton_Clear;

  this->addActions(actions);

}

void ChartActionsToolBar::onToolButton_TimerOnline_toggled(bool checked)
{
  if (checked)
    m_toolButton_TimerOnline->setIcon(QIcon(":/icons/icons/stop.png"));
  else
    m_toolButton_TimerOnline->setIcon(QIcon(":/icons/icons/start.png"));
}
