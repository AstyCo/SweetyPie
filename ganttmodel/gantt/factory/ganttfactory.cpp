/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию GanttFactory class.
 * \~englist
 * \brief File contains realization of GanttFactory class.
 */
#include "ganttfactory.h"

#include "gantt/info/ganttinfoitem.h"
#include "gantt/plot/scene_objects/ganttintervalgraphicsobject.h"
#include "gantt/plot/scene_objects/ganttcalcgraphicsobject.h"

GanttFactory::GanttFactory(IModelWrapper *wrapper)
    : AbstractGanttFactory()
{
    if(!wrapper)
        qWarning("GanttFactory(IModelWrapper *wrapper) :: wrapper is NULL");

    _wrapper = wrapper;
}

GanttFactory::~GanttFactory()
{
    if(_wrapper)
        delete _wrapper;
    else
        qWarning("GanttFactory::~GanttFactory");
}

GanttInfoItem *GanttFactory::createInfo(const QModelIndex &index)
{
    if(_wrapper){
        return new GanttInfoItem( _wrapper->title(index)
                                  , _wrapper->start(index)
                                  , _wrapper->length(index)
                                  , index
                                  , _wrapper->color(index) );
    }
    qWarning("GanttFactory::createInfo called with NULL _wrapper");
    return NULL;
}

GanttGraphicsObject *GanttFactory::createGraphicsObject(GanttInfoItem *info)
{
    if(_wrapper){
        switch(readTag(_wrapper->tag(info->index()))){
        case GanttIntervalGraphicsObjectType:
//            qDebug() << "created Interval" << info->title();
            return new GanttIntervalGraphicsObject(info);
        case GanttCalcGraphicsObjectType:
//            qDebug() << "created CalcDt" << info->title();
            return new GanttCalcGraphicsObject(info);
        default:
            qWarning("GanttFactory::createGraphicsObject");
        }
    }
    qWarning("GanttFactory::createGraphicsObject called with NULL _wrapper");
    return NULL;
}

QAbstractItemModel *GanttFactory::model() const
{
    if(_wrapper)
        return _wrapper->model();
    qWarning("GanttFactory::model");
    return NULL;
}

bool GanttFactory::isEvent(GanttInfoItem *info) const
{
    if(_wrapper)
        return _wrapper->isEvent(info->index());

    return AbstractGanttFactory::isEvent(info);
}

GanttFactory::RegisteredGraphicsObjectType GanttFactory::readTag(const QString &tag)
{
    if(tag == "Interval")
        return GanttIntervalGraphicsObjectType;
    if(tag == "CalcDt")
        return GanttCalcGraphicsObjectType;

    return UnregisteredType;
}
