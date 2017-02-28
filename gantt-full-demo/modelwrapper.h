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
    virtual QAbstractItemModel *model() const;
    virtual QString     tag(const QModelIndex &index) const;
    virtual QString     title(const QModelIndex &index) const;
    virtual UtcDateTime start(const QModelIndex &index) const;
    virtual TimeSpan    length(const QModelIndex &index) const;
    virtual QColor      color(const QModelIndex &index) const;
    // ---

};

#endif // MODELWRAPPER_H
