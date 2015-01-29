#ifndef GANTTGRAPHICSITEM_H
#define GANTTGRAPHICSITEM_H

#include <QGraphicsLinearLayout>
#include "ganttgraphicsitemstage.h"
#include "ganttitem.h"
#include "ganttgraphicsheader.h"

class GanttGraphicsItem : public QGraphicsLinearLayout
{
public:
    //GanttGraphicsItem();
    //GanttGraphicsItem(QList<GanttGraphicsItemStage*> stages);
    GanttGraphicsItem(GanttItem * gItem, Scale scale, QDateTime headerBegin, QDateTime headerEnd);
    ~GanttGraphicsItem();

private:
    //QList<GanttGraphicsItemStage*> m_graphicsStageList;
    GanttGraphicsItemStage * m_graphicsStage;
    GanttItem * m_ganttItem;
    void setGraphicsStages();
    //void createGraphicsStage(GanttItemStage * stage);
    Scale m_scale;
    QDateTime m_headerBegin;
    QDateTime m_headerEnd;
};

#endif // GANTTGRAPHICSITEM_H
