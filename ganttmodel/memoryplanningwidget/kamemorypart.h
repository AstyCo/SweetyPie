#ifndef KAMEMORYPART_HPP
#define KAMEMORYPART_HPP

#include <ganttproject_global.h>

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

QString MemoryState_to_QString(MemoryState state);
QColor MemoryState_to_QColor(MemoryState state, bool isActive = true);

QString operator+(const QString& qstr, MemoryState status);


//struct GANTTMODELSHARED_EXPORT MemoryItemPresentation
//{
//    MemoryItemPresentation(MemoryState status = Empty, int unitId = 0);

//    MemoryState m_state;

//    // extra info here
//    int m_unitId;

//    //

//    long m_start,
//         m_finish;

//    QColor color() const;
//};

class GANTTMODELSHARED_EXPORT KaMemoryPart
{
private:
    MemoryState m_state;
    long m_start;
    long m_finish;
    // extra
    int m_id;
    //
    bool m_initialized;
public:
    KaMemoryPart();
    KaMemoryPart(long start,long finish, MemoryState state, int id);
    ~KaMemoryPart();

// //    static QMultiMap<int, KaMemoryPart> selectAll();
//    static QList<KaMemoryPart> selectDeviceParts(int idKa);

//    void setData(const QList<MemoryState>& list);


    MemoryState state() const;
    void setState(const MemoryState &state);


    bool operator ==(const KaMemoryPart &part);

    bool operator !=(const KaMemoryPart &part);

    long size() const;

    long start() const;
    void setStart(long start);
    long finish() const;
    void setFinish(long finish);
    long length() const;
    int id() const;
    void setId(int id);
};


#endif // KAMEMORYPART_HPP

