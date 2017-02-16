#include "ganttbuilder.h"


#include "gantt/info/ganttinfonode.h"
#include "gantt/plot/scene_objects/ganttintervalgraphicsobject.h"
#include "gantt/plot/scene_objects/ganttcalcgraphicsobject.h"

GanttBuilder::GanttBuilder(IModelWrapper *wrapper)
    : AbstractBuilder()
{
    if(!wrapper)
        qWarning("GanttBuilder(IModelWrapper *wrapper) :: wrapper is NULL");

    _wrapper = wrapper;
}

GanttBuilder::~GanttBuilder()
{
    if(_wrapper)
        delete _wrapper;
    else
        qWarning("GanttBuilder::~GanttBuilder");
}

GanttInfoItem *GanttBuilder::createInfo(const QModelIndex &index)
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
            qWarning("GanttBuilder::createInfo");
        }

//        _infoForIndex.insert(index, item);  // fills cache
//        emit itemAdded(item);
        return item;

    }
    qWarning("GanttBuilder::createInfo");
    return NULL;
}

GanttGraphicsObject *GanttBuilder::createGraphicsObject(GanttInfoItem *info)
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
            qWarning("GanttBuilder::createGraphicsObject");
        }
    }
    qWarning("GanttBuilder::createGraphicsObject");
    return NULL;
}

QAbstractItemModel *GanttBuilder::model() const
{
    if(_wrapper)
        return _wrapper->model();
    qWarning("GanttBuilder::model");
    return NULL;
}

bool GanttBuilder::isEvent(GanttInfoItem *info) const
{
    if(_wrapper)
        return _wrapper->isEvent(info->index());

    return AbstractBuilder::isEvent(info);
}

GanttBuilder::RegisteredGraphicsObjectType GanttBuilder::readTag(const QString &tag)
{
    if(tag == "Interval")
        return GanttIntervalGraphicsObjectType;
    if(tag == "CalcDt")
        return GanttCalcGraphicsObjectType;

    return UnregisteredType;
}
