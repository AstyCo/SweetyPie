#include "ganttitemstage.h"

//#include <QDebug>

GanttItemStage::GanttItemStage(QDateTime begin, QDateTime end)
{
    m_begin = begin;
    m_end = end;
    m_duration = m_begin.secsTo(m_end);
    //qDebug()<<"dt2"<< begin<< end;
}
QDateTime GanttItemStage::begin() const
{
    return m_begin;
}

void GanttItemStage::setBegin(const QDateTime &begin)
{
    m_begin = begin;
}
QDateTime GanttItemStage::end() const
{
    return m_end;
}

void GanttItemStage::setEnd(const QDateTime &end)
{
    m_end = end;
}
int GanttItemStage::duration() const
{
    return m_duration;
}

void GanttItemStage::setDuration(int duration)
{
    m_duration = duration;
}




