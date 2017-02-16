#include "ganttfactory.h"


#include "gantt/info/ganttinfonode.h"
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
        GanttInfoItem *item = NULL;

        switch(readTag(_wrapper->tag(index))){
        case GanttIntervalGraphicsObjectType:
            item = new GanttInfoLeaf(_wrapper->title(index)
                                     , _wrapper->start(index)
                                     , _wrapper->length(index)
                                     , index
                                     , _wrapper->color(index) );
            break;
        case GanttCalcGraphicsObjectType:
            item = new GanttInfoNode(_wrapper->title(index)
                                     , _wrapper->start(index)
                                     , index
                                     , _wrapper->color(index) );
            break;
        default:
            qWarning("GanttFactory::createInfo");
        }
        return item;

    }
    qWarning("GanttFactory::createInfo");
    return NULL;
}

GanttGraphicsObject *GanttFactory::createGraphicsObject(GanttInfoItem *info)
{
    if(_wrapper){
        switch(readTag(_wrapper->tag(info->index()))){
        case GanttIntervalGraphicsObjectType:
            if(GanttInfoLeaf *leaf = qobject_cast<GanttInfoLeaf*>(info))
                return new GanttIntervalGraphicsObject(leaf);
            else
                qWarning("not leaf");
            break;
        case GanttCalcGraphicsObjectType:
            if(GanttInfoNode *node = qobject_cast<GanttInfoNode*>(info))
                return new GanttCalcGraphicsObject(node);
            else
                qWarning("not node");
            break;
        default:
            qWarning("GanttFactory::createGraphicsObject");
        }
    }
    qWarning("GanttFactory::createGraphicsObject");
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
