/*!
 * \file
 * \brief Header with default implementation of AbstractGanttFactory
 *
 * Class GanttFactory implements abstract methods of AbstractGanttFactory,
 * using realisation of IModelWrapper interface (GanttFactory follows
 * Template Method pattern).
*/
#ifndef GANTTFACTORY_H
#define GANTTFACTORY_H

#include "ganttproject_global.h"
#include "extensions-gui/interfaces/abstractganttfactory.h"
#include "extensions-gui/interfaces/imodelwrapper.h"

class GANTTMODELSHARED_EXPORT GanttFactory : public AbstractGanttFactory
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
