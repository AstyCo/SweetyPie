#ifndef KAMEMORY_H
#define KAMEMORY_H

#include <QList>
#include <ganttproject_global.h>

#include "kamemorypart.h"


class GANTTMODELSHARED_EXPORT KaMemory
{
public:
    KaMemory();

    ~KaMemory();

    long memorySize() const;
    void setMemorySize(long memorySize);

    QList<KaMemoryPart> &memoryParts();
    const QList<KaMemoryPart> &memoryParts() const;


    void init(const QList<KaMemoryPart>& list, long memorySize);



private:
    long _memorySize;
    QList<KaMemoryPart> _memoryParts;

};

#endif // KAMEMORY_H

