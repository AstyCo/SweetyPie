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

class GANTTMODELSHARED_EXPORT KaMemoryPart
{
private:
    MemoryState _state;
    int _begin;
    int _end;

public:
    KaMemoryPart();
    ~KaMemoryPart();

//    static QMultiMap<int, KaMemoryPart> selectAll();
    static QList<KaMemoryPart> selectDeviceParts(int idKa);

    void setData(const QList<MemoryState>& list);


    MemoryState state() const;
    void setState(const MemoryState &state);


    bool operator ==(const KaMemoryPart &part)
    {
        return begin() == part.begin() &&
                end() == part.end();
    }

    bool operator !=(const KaMemoryPart &part)
    {
        return !(*this==part);
    }

    int size() const;

    int begin() const;
    void setBegin(int begin);
    int end() const;
    void setEnd(int end);
};


#endif // MEMORYITEMPRESENTATION_HPP

