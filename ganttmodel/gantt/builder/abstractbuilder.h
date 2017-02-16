#ifndef ABSTRACTBUILDER_H
#define ABSTRACTBUILDER_H

#include "ganttproject_global.h"

#include <QAbstractItemModel>

class GanttInfoItem;
class GanttGraphicsObject;


class GANTTMODELSHARED_EXPORT AbstractBuilder
{
public:
    AbstractBuilder();

    virtual GanttInfoItem *createInfo(const QModelIndex &index) = 0;
    virtual GanttGraphicsObject *createGraphicsObject(GanttInfoItem *info) = 0;
    virtual QAbstractItemModel *model() const = 0;

    virtual bool isEvent(GanttInfoItem *info) const{
        return false;
    }
};

#endif // ABSTRACTBUILDER_H
