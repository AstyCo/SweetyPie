#ifndef KAMEMORY_H
#define KAMEMORY_H

#include <QList>
#include <ganttmodel_global.h>

#include "kamemorypart.h"


class KaMemory
{
public:
    KaMemory();

    ~KaMemory();

    long memorySize() const;
    void setMemorySize(long memorySize);

    QList<KaMemoryPart> &memoryParts();

    void init(const QList<KaMemoryPart>& list, long memorySize);



private:
    long _memorySize;
    QList<KaMemoryPart> _memoryParts;

};

#endif // KAMEMORY_H

