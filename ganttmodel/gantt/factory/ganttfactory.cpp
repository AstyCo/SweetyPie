/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию GanttFactory class.
 * \~englist
 * \brief File contains realization of GanttFactory class.
 */
#include "ganttfactory.h"

#include "gantt/info/ganttinfotree.h"
#include "gantt/info/ganttinfoitem.h"
#include "gantt/plot/scene_objects/ganttintervalgraphicsobject.h"
#include "gantt/plot/scene_objects/ganttcalcgraphicsobject.h"

GanttFactory::GanttFactory(IModelWrapper *wrapper, QObject *parent)
    : QObject(parent), AbstractGanttFactory()
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

GanttInfoItem *GanttFactory::createInfo(const QModelIndex &index, GanttInfoItem *parent)
{
    if(_wrapper){
        return new GanttInfoItem( _wrapper->title(index)
                                  , _wrapper->start(index)
                                  , _wrapper->length(index)
                                  , _wrapper->color(index)
                                  , parent);
    }
    qWarning("GanttFactory::createInfo called with NULL _wrapper");
    return NULL;
}

GanttGraphicsObject *GanttFactory::createGraphicsObject(GanttInfoItem *info, const QModelIndex &index)
{
    if(_wrapper){
        switch(readTag(_wrapper->tag(index))){
        case GanttIntervalGraphicsObjectType:
            qDebug() << "created Interval" << info->title();
            return new GanttIntervalGraphicsObject(info);
        case GanttCalcGraphicsObjectType:
            qDebug() << "created CalcDt" << info->title();
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

GanttInfoItem *GanttFactory::infoForIndex(const QModelIndex &index) const
{
    if (index.parent().isValid())
        return infoForIndex(index.parent())->at(index.row());
    if (index.isValid())
        return _treeInfo->root()->at(index.row());
    return _treeInfo->root();
}

QModelIndex GanttFactory::indexForInfo(const GanttInfoItem *item) const
{
    if (!item || !item->parent())
        return QModelIndex();
    return model()->index(item->row(), 0, indexForInfo(item->parent()));
}

bool GanttFactory::isEvent(const QModelIndex &index) const
{
    if(_wrapper)
        return _wrapper->isEvent(index);

    return AbstractGanttFactory::isEvent(index);
}

void GanttFactory::initialize(GanttInfoTree *tree)
{
    _treeInfo = tree;
}

GanttFactory::RegisteredGraphicsObjectType GanttFactory::readTag(const QString &tag)
{
    if(tag == "Interval")
        return GanttIntervalGraphicsObjectType;
    if(tag == "CalcDt")
        return GanttCalcGraphicsObjectType;

    return UnregisteredType;
}
