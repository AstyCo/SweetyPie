#include "kamemory.h"

#include <QDebug>

KaMemory::KaMemory()
{
    _memorySize = 0;
}


KaMemory::~KaMemory()
{

}

long KaMemory::memorySize() const
{
    return _memorySize;
}

void KaMemory::setMemorySize(long memorySize)
{
    _memorySize = memorySize;
}

QList<KaMemoryPart> &KaMemory::memoryParts()
{
    return _memoryParts;
}

const QList<KaMemoryPart> &KaMemory::memoryParts() const
{
    return _memoryParts;
}

void KaMemory::init(const QList<KaMemoryPart> &list, long memorySize)
{
    _memoryParts.clear();

    setMemorySize(memorySize);
    _memoryParts.append(list);
}

