#ifndef MEMORYITEMPRESENTATION_HPP
#define MEMORYITEMPRESENTATION_HPP

#include <ganttmodel_global.h>

#include <QColor>


namespace Memory
{
    enum MemoryState
    {
        Empty,          ///< Пустая память - белый
        Free,           ///< Свободно - серый
        Recorded,       ///< Записано - зелёный
        PendingWrite,   ///< Ожидание съёмки - оранжевый
        PendingRead,    ///< Ожидание сброса - жёлтый
        ErrorWrite,     ///< Ошибка записи - сиреневый
        ErrorRead,      ///< Ошибка сброса - красный
        MemoryState_count
    };
}
using namespace Memory;

QString fixedNumPresentation(long num,int base,long max);

QString MemoryState_to_Qstring(MemoryState state);
QColor MemoryState_to_QColor(MemoryState state, bool isActive = true);

QString operator+(const QString& qstr, MemoryState status);


struct GANTTMODELSHARED_EXPORT MemoryItemPresentation
{
    MemoryItemPresentation(MemoryState status = Empty, int unitId = 0);

    MemoryState m_state;

    // extra info here
    int m_unitId;

    //

    long m_start,
         m_finish;

    QColor color() const;
};

#endif // MEMORYITEMPRESENTATION_HPP

