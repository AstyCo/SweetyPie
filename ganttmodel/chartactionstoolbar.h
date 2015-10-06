#ifndef CHARTACTIONSTOOLBAR_H
#define CHARTACTIONSTOOLBAR_H

#include <QToolBar>
#include "ganttmodel_global.h"

enum ChartActions{
    caNo=0,
    caScale=1,
    caGrid=2,
    caPaintIntervals=4,
    caSelectIntervals=8,
    caMaxMinLines=16,
    caTimer=32,
    caSelectTarget=64
};

class GANTTMODELSHARED_EXPORT ChartActionsToolBar : public QToolBar
{
public:
  ChartActionsToolBar(QWidget *parent = 0);

  void setChartActions(uint chartActions);
  int chartActions() const { return m_chartActions; }
  void setChartActionVisible(ChartActions type, bool vis);
  QAction * getChartAction(ChartActions type) const;
  QAction * getChartActionClear() const { return m_toolButton_Clear; }

private:
  QAction * m_toolButton_AutoZoom;
  QAction * m_toolButton_Grid;
  QAction * m_toolButton_IntervalVisibled;
  QAction * m_toolButton_SelectInterval;
  QAction * m_toolButton_MaxMinValues;
  QAction * m_toolButton_TimerOnline;
  QAction * m_toolButton_SelectTarget;
  QAction * m_toolButton_Clear;

  uint m_chartActions;

  void initActions();

private slots:
  void onToolButton_TimerOnline_toggled(bool checked);
};

#endif // CHARTACTIONSTOOLBAR_H
