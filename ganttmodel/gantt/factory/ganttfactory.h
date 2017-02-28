#ifndef GANTTFACTORY_H
#define GANTTFACTORY_H

#include "gantt-static-lib-global.h"
#include "extensions-gui/interfaces/abstractganttfactory.h"
#include "extensions-gui/interfaces/imodelwrapper.h"

class GANTTSTATICLIB_EXPORT GanttFactory : public AbstractGanttFactory
{
public:
    enum RegisteredGraphicsObjectType{
        UnregisteredType                = 0,
        GanttIntervalGraphicsObjectType = 1,
        GanttCalcGraphicsObjectType     = 2
    };


    // Construtor
    explicit GanttFactory(IModelWrapper *wrapper);
    ~GanttFactory();

    virtual GanttInfoItem *createInfo(const QModelIndex &index);
    virtual GanttGraphicsObject *createGraphicsObject(GanttInfoItem *info);
    virtual QAbstractItemModel *model() const;

    virtual bool isEvent(GanttInfoItem *info) const;

private:
    IModelWrapper *_wrapper;
    static RegisteredGraphicsObjectType readTag(const QString &tag);
};

#endif // GANTTFACTORY_H
