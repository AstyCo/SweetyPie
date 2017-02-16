#ifndef GANTTBUILDER_H
#define GANTTBUILDER_H

#include "abstractbuilder.h"
#include "imodelwrapper.h"

class GANTTMODELSHARED_EXPORT GanttBuilder : public AbstractBuilder
{
public:
    enum RegisteredGraphicsObjectType{
        UnregisteredType                = 0,
        GanttIntervalGraphicsObjectType = 1,
        GanttCalcGraphicsObjectType     = 2
    };


    // Construtor
    explicit GanttBuilder(IModelWrapper *wrapper);
    ~GanttBuilder();

    virtual GanttInfoItem *createInfo(const QModelIndex &index);
    virtual GanttGraphicsObject *createGraphicsObject(GanttInfoItem *info);
    virtual QAbstractItemModel *model() const;

    virtual bool isEvent(GanttInfoItem *info) const;

private:
    IModelWrapper *_wrapper;
    static RegisteredGraphicsObjectType readTag(const QString &tag);
};

#endif // GANTTBUILDER_H
