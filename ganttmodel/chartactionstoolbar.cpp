#include "chartactionstoolbar.h"

#include <QAction>

ChartActionsToolBar::ChartActionsToolBar(QWidget *parent)
  :QToolBar(parent)
{
  initActions();
}

void ChartActionsToolBar::setChartActions(const QSet<ChartActions> &chartActions)
{
  m_enabledChartActionsSet = chartActions;

  foreach(ChartActions actType, m_actions.keys())
      m_actions.value(actType)->setVisible(m_enabledChartActionsSet.contains(actType));
}

void ChartActionsToolBar::setChartActionVisible(ChartActions type, bool vis)
{
  QAction *act = getChartAction(type);
  if (act == NULL)
    return;

  if (vis)
  {
    act->setVisible(m_enabledChartActionsSet.contains(type));
  }
  else
    act->setVisible(false);
}

QAction *ChartActionsToolBar::getChartAction(ChartActions type) const
{
  return m_actions.value(type, NULL);
}

void ChartActionsToolBar::initActions()
{

  QAction *act = new QAction(QIcon(":/icons/icons/find.png"),
                                      tr("Автоматически подобрать масштаб"), this);
  m_actions.insert(caScale, act);

  act = new QAction(QIcon(":/icons/icons/grid.png"), tr("Сетка"), this);
  act->setCheckable(true);
  m_actions.insert(caGrid, act);

  act = new QAction(QIcon(":/icons/icons/interval_visible.png"),
                                              tr("Вкл./Выкл интервалов"), this);
  m_actions.insert(caZones, act);

  act = new QAction(QIcon(":/icons/icons/selection_interval.png"),
                                            tr("Выбрать интервал времени"), this);
  act->setCheckable(true);
  m_actions.insert(caSelectInterval, act);

  act = new QAction(QIcon(":/icons/icons/max_min.png"), tr("Настройка границ"), this);
  m_actions.insert(caMaxMinLines, act);
  act = new QAction(QIcon(":/icons/icons/start.png"), tr("Обновление по времени"), this);
  act->setCheckable(true);
  connect(act, SIGNAL(toggled(bool)), SLOT(onToolButton_TimerOnline_toggled(bool)));

  m_actions.insert(caTimer, act);
  act = new QAction(QIcon(":/icons/icons/target.png"),
                                          tr("Выбрать точку прицеливания"), this);
  act->setCheckable(true);
  m_actions.insert(caSelectTarget, act);
  act = new QAction(QIcon(":/icons/icons/clear.png"),
                                   tr("Сбросить все настройки по-умолчанию"), this);
  m_actions.insert(caClear, act);

  act = new QAction(QIcon(":/icons/icons/infopanel.png"), tr("Дополнительная информация"), this);
  act->setCheckable(true);
  m_actions.insert(caDetailsPanel, act);

  act = new QAction(QIcon(":/icons/icons/options1.png"),
                                   tr("Настройки отображения"), this);
  m_actions.insert(caSettingsDlg, act);

  this->addActions(m_actions.values());
}

void ChartActionsToolBar::onToolButton_TimerOnline_toggled(bool checked)
{
  if (checked)
    m_actions.value(caTimer)->setIcon(QIcon(":/icons/icons/stop.png"));
  else
    m_actions.value(caTimer)->setIcon(QIcon(":/icons/icons/start.png"));
}
