#ifndef CHARTACTIONSTOOLBAR_H
#define CHARTACTIONSTOOLBAR_H

#include <QSet>
#include <QMap>
#include <QToolBar>

#include "ganttproject_global.h"

enum ChartActions
{
  caScale,
  caSelectInterval,
  caSelectTarget,
  caTimer,
  caClear,
  caGrid,
  caZones,
  caMaxMinLines,
  caDetailsPanel,
  caSettingsDlg,
  caSelectIntervalPanel
};

class GANTTMODELSHARED_EXPORT ChartActionsToolBar : public QToolBar
{
  Q_OBJECT
public:
  ChartActionsToolBar(QWidget *parent = 0);

  void setChartActions(const QSet<ChartActions> &chartActions);
  QSet<ChartActions> chartActions() const { return m_enabledChartActionsSet; }
  void setChartActionVisible(ChartActions type, bool vis);
  QAction * getChartAction(ChartActions type) const;

private:
  QMap<ChartActions, QAction*> m_actions;
  QSet<ChartActions> m_enabledChartActionsSet;

  void initActions();

public slots:
  void onToolButton_TimerOnline_toggled(bool checked);
};

#endif // CHARTACTIONSTOOLBAR_H
