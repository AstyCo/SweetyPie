#ifndef GANTTITEMSTAGE_H
#define GANTTITEMSTAGE_H

#include <QDateTime>
#include <QGraphicsItem>
//#include "ganttgraphicsitemstage.h"

class GanttItemStage
{
public:
    GanttItemStage(QDateTime begin, QDateTime end);
//    GanttItemStage(QDateTime begin, QDateTime end);
//    GanttItemStage(QDateTime begin, QDateTime duration);
//    GanttItemStage(QDateTime end, QDateTime duration);


    QDateTime begin() const;
    void setBegin(const QDateTime &begin);

    QDateTime end() const;
    void setEnd(const QDateTime &end);

    int duration() const;
    void setDuration(int duration);

private:
    QDateTime m_begin;
    QDateTime m_end;
    int m_duration; // длительность в секундах
    //GanttGraphicsItemStage * m_stage;
};

#endif // GANTTITEMSTAGE_H
