#ifndef MODELWRAPPER_H
#define MODELWRAPPER_H

#include "extensions-gui/interfaces/imodelwrapper.h"
#include "gantttreemodel.h"

class ModelWrapper : public IModelWrapper
{
    GanttTreeModel *_model;

public:
    explicit ModelWrapper(GanttTreeModel *model);

    // --- Interface implementation
    virtual QAbstractItemModel *model() const{
        return _model;
    }
    virtual QString     tag(const QModelIndex &index) const{
        if(_model &&_model->leafForIndex(index))
            return "GanttIntervalGraphicsObject";
        return "GanttCalcGraphicsObject";
    }
    virtual QString     title(const QModelIndex &index) const{
        if(_model){
            GanttInfoItem *item =_model->itemForIndex(index);
            if(item)
                return item->title();
        }
        return QString();
    }
    virtual UtcDateTime start(const QModelIndex &index) const{
        if(_model){
            GanttInfoItem *item =_model->itemForIndex(index);
            if(item)
                return item->start();
        }
        return UtcDateTime();
    }
    virtual TimeSpan    length(const QModelIndex &index) const{
        if(_model){
                GanttInfoItem *item =_model->itemForIndex(index);
                if(item)
                    return item->timeSpan();
            }
        return TimeSpan();
    }
    virtual QColor      color(const QModelIndex &index) const{
        if(_model){
            GanttInfoItem *item =_model->itemForIndex(index);
            if(item)
                return item->color();
        }
        return Qt::green;
    }
    virtual bool        isEvent(const QModelIndex &index) const{
        if( tag(index) == "GanttIntervalGraphicsObject")
            return true;
        return false;
    }

    // ---

};

#endif // MODELWRAPPER_H
