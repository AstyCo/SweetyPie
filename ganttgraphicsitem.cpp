#include "ganttgraphicsitem.h"

#include <QDebug>

//#include "QMetaType"

/*
GanttGraphicsItem::GanttGraphicsItem()
{
    //qRegisterMetaType<GanttGraphicsItem>("GanttGraphicsItem");
    setOrientation(Qt::Horizontal);
}

GanttGraphicsItem::GanttGraphicsItem(QList<GanttGraphicsItemStage *> stages)
{
    //qRegisterMetaType<GanttGraphicsItem>("GanttGraphicsItem");
    setOrientation(Qt::Horizontal);
    m_stageList = stages;
    int i = 0;
    foreach (m_stage, m_stageList) {
        m_stage = m_stageList.at(i);
        this->addItem(m_stage);
        i++;
    }
}
*/

GanttGraphicsItem::GanttGraphicsItem(GanttItem *gItem, Scale scale, QDateTime headerBegin, QDateTime headerEnd)
{
    setOrientation(Qt::Horizontal);

    m_scale = scale;

    m_headerBegin = headerBegin;
    m_headerEnd = headerEnd;

    m_ganttItem = gItem;
    setGraphicsStages();
}

GanttGraphicsItem::~GanttGraphicsItem()
{
    delete m_ganttItem;
    delete m_graphicsStage;
}


void GanttGraphicsItem::setGraphicsStages()
{
    //qreal spacer = 0;
    //setGeometry();
    //for(int i = 0; i < m_ganttItem->stageList().count(); i++)
    //{
        qreal durationTillBegin = m_headerBegin.secsTo(m_ganttItem->commonBegin());//->stageList().at(i)->begin());

        qDebug()<<durationTillBegin;

        m_graphicsStage = new GanttGraphicsItemStage(m_ganttItem/*->stageList().at(i)*/, m_scale, durationTillBegin);


        addItem(m_graphicsStage);

        //устаналиваем промежуток после блока
//        if ( i+1 < m_ganttItem->stageList().count() )
//        {
//            spacer = m_ganttItem->stageList().at(i)->end().secsTo(m_ganttItem->stageList().at(i+1)->begin())*20; // 20 ширина одной секунды
//            setItemSpacing(i, spacer);
//        }

        //qDebug()<<"stages"<<this->count();
    //}
}

//void GanttGraphicsItem::createGraphicsStage(GanttItemStage *stage)
//{
//    GanttGraphicsItemStage * newGIS = new GanttGraphicsItemStage(stage);
//}
