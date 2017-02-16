#ifndef IMODELWRAPPER_H
#define IMODELWRAPPER_H

#include "ganttproject_global.h"
#include "utcdatetime.h"

#include <QAbstractItemModel>
#include <QString>

class GANTTMODELSHARED_EXPORT IModelWrapper
{
public:
    IModelWrapper();
    virtual ~IModelWrapper();

    // --- Interface

    virtual QAbstractItemModel *model() const = 0;
    virtual QString     tag(const QModelIndex &index) const = 0;  /// < What is?
    virtual QString     title(const QModelIndex &iGanttIndex) const = 0;
    virtual UtcDateTime start(const QModelIndex &iGanttIndex) const = 0;
    virtual TimeSpan    length(const QModelIndex &iGanttIndex) const = 0;
    virtual QColor      color(const QModelIndex &iGanttIndex) const = 0;
    virtual bool        isEvent(const QModelIndex &/*index*/) const{
        return true;
    }

    // ---

public:
    bool isDot(QModelIndex &index){
        return length(index).totalMicroseconds() == 0;
    }
};

#endif // IMODELWRAPPER_H
