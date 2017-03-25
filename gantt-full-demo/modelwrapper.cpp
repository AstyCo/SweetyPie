#include "modelwrapper.h"

ModelWrapper::ModelWrapper(GanttTreeModel *model)
    :_model(model)
{

}

QAbstractItemModel *ModelWrapper::model() const{
    return _model;
}

QString ModelWrapper::tag(const QModelIndex &index) const{
    if(_model && _model->itemForIndex(index)->title().contains("leaf"))
        return "Interval";
    if (_model && _model->itemForIndex(index)->timeSpan().totalMilliseconds() != 0)
        return "Interval";
    return "CalcDt";
}

QString ModelWrapper::title(const QModelIndex &index) const{
    if(_model){
        GanttInfoItem *item =_model->itemForIndex(index);
        if(item)
            return item->title();
    }
    return QString();
}

UtcDateTime ModelWrapper::start(const QModelIndex &index) const{
    if(_model){
        GanttInfoItem *item =_model->itemForIndex(index);
        if(item)
            return item->start();
    }
    return UtcDateTime();
}

TimeSpan ModelWrapper::length(const QModelIndex &index) const{
    if(_model){
        GanttInfoItem *item =_model->itemForIndex(index);
        if(item)
            return item->timeSpan();
    }
    return TimeSpan();
}

QColor ModelWrapper::color(const QModelIndex &index) const{
    if(_model){
        GanttInfoItem *item =_model->itemForIndex(index);
        if(item)
            return item->color();
    }
    return Qt::green;
}
