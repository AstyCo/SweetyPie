#include "kamemorypart.h"

#include <QDebug>

using namespace Memory;



int KaMemoryPart::id() const
{
    return m_id;
}

void KaMemoryPart::setId(int id)
{
    m_initialized = true;
    m_id = id;
}
MemoryState KaMemoryPart::state() const
{
    return m_state;
}

void KaMemoryPart::setState(const MemoryState &statusPart)
{
    m_state = statusPart;
}

bool KaMemoryPart::operator ==(const KaMemoryPart &part)
{
    if(!m_initialized)
    {
        if(!part.m_initialized)
            return true;
        return false;
    }
    return start() == part.start() &&
            finish() == part.finish();
}

bool KaMemoryPart::operator !=(const KaMemoryPart &part)
{
    return !(*this==part);
}

long KaMemoryPart::size() const
{
    return m_finish-m_start;
}


long KaMemoryPart::start() const
{
    return m_start;
}

void KaMemoryPart::setStart(long start)
{
    m_initialized = true;
    m_start = start;
}

long KaMemoryPart::finish() const
{
    return m_finish;
}

void KaMemoryPart::setFinish(long finish)
{
    m_initialized = true;
    m_finish = finish;
}

long KaMemoryPart::length() const
{
    return m_finish - m_start + 1;
}
KaMemoryPart::KaMemoryPart()
{
    m_initialized = false;
    m_state=Empty;
    m_start =0;
    m_finish = 0;
}

KaMemoryPart::KaMemoryPart(long start, long finish, MemoryState state, int id)
{
    setStart(start);
    setFinish(finish);
    setState(state);
    setId(id);
    m_initialized = true;
}

KaMemoryPart::~KaMemoryPart()
{

}




//QMultiMap<int, KaMemoryPart> KaMemoryPart::selectAll()
//{
//    QMultiMap<int, KaMemoryPart> rez;
////    DbProcess dbp1(dbptConnectAndSqlCommand,"all ka mem parts list","select * from koci.kamemorypart");
////    dbp1.startDbProcess();
////    if(dbp1.getSqlResultIsActive())
////    {
////        QSqlQuery q1 = dbp1.getSqlResultToQuery();
////        while(q1.next())
////        {
////            int idka = q1.value(q1.record().indexOf("id_device")).toInt();

////            KaMemoryPart part ;
////            part.setData(q1.record());
////            rez.insert(idka,part);
////        }

////    }
//    return rez;
//}

//QList<KaMemoryPart> KaMemoryPart::selectDeviceParts(int idKaDevice)
//{
//    QList<KaMemoryPart> rez;
////    DbProcess dbp1(dbptConnectAndSqlCommand,"ka mem parts list","select * from koci.kamemorypart where id_device="+QString::number(idKaDevice));
////    dbp1.startDbProcess();
////    if(dbp1.getSqlResultIsActive())
////    {
////        QSqlQuery q1 = dbp1.getSqlResultToQuery();
////        while(q1.next())
////        {
////            int tmpiddevice = q1.value(q1.record().indexOf("id_device")).toInt();

////            if(tmpiddevice==idKaDevice)
////            {
////                KaMemoryPart part;
////                part.setData(q1.record());
////                rez.append(part);
////            }
////        }

////    }
//    return rez;
//}

//void KaMemoryPart::setData(const QList<MemoryState>& list)
//{
////    setBegin(record.value("from").toInt());
////    setEnd(record.value("to").toInt());
////    setStatus((MemoryState)record.value("type").toInt());
//}




static QChar calcChar(long num,int base)
{
    return QString::number(num%base,base).toUpper().at(0);
}

QString fixedNumPresentation(long num, int base, long max)
{
    if(max<num)
    {
        qDebug()<<"fixedNumPresentation num>max"+QString::number(num)+'>'+QString::number(max);
        return QString();
    }

    int len=0;
    QString res;
    while(num)
    {
        res.push_front(calcChar(num,base));
        num/=base;
        max/=base;
        len++;
        if((base==2)&&!(len%4))
            res.push_front(' ');
    }
    while(max)
    {
        res.push_front('0');
        len++;
        if((base==2)&&!(len%4)&&len)
            res.push_front(' ');
        max/=base;
    }

    while(len%4)
    {
        len++;
        res.push_front('0');
    }
    return res;
}


//MemoryItemPresentation::MemoryItemPresentation(MemoryState status, int unitId)
//{
//    m_state = status;
//    m_unitId = unitId;
//}


//QColor MemoryItemPresentation::color() const
//{
//    return MemoryState_to_QColor(m_state);
//}


QString operator+(const QString &qstr, MemoryState status)
{
    return qstr+MemoryState_to_QString(status);
}


QColor MemoryState_to_QColor(MemoryState state, bool isActive /*= true*/)
{
    QColor result;
//    if(isActive)
        switch (state) {
        case Empty:
            result = QColor(Qt::white);
            break;
        case Free:
            result = QColor::fromRgb(0xCCD2D4);
            break;
        case Recorded:
            result = QColor::fromRgb(0xE3E187);
            break;
        case PendingWrite:
            result = QColor::fromRgb(0x73E884);
            break;
        case PendingRead:
            result = QColor::fromRgb(0x99DDE3);
            break;
        case ErrorWrite:
            result = QColor::fromRgb(0xE39753);
            break;
        case ErrorRead:
            result = QColor::fromRgb(0x308C9E);
            break;
        default:
            result = QColor(Qt::black);
            break;
        }
//    else
//        switch (state) {
//        case Empty:
//            result = QColor(Qt::white);
//            break;
//        case Free:
//            result = QColor::fromRgb(0x9FA5A6);
//            break;
//        case Recorded:
//            result = QColor::fromRgb(0xFFF4A6);
//            break;
//        case PendingWrite:
//            result = QColor::fromRgb(0xAAE8B0);
//            break;
//        case PendingRead:
//            result = QColor::fromRgb(0xBFEAFF);
//            break;
//        case ErrorWrite:
//            result = QColor::fromRgb(0xB071E8);
//            break;
//        case ErrorRead:
//            result = QColor::fromRgb(0xC27C74);
//            break;
//        default:
//            result = QColor(Qt::black);
//            break;
//        }
    if(!isActive && state != Free)
        return result.lighter(130);
    return result;
}


QString MemoryState_to_QString(MemoryState status)
{
    switch(status)
    {
    case Empty:
        return QObject::tr("Пусто");
    case Free:
        return QObject::tr("Свободно");
    case Recorded:
        return QObject::tr("Записано");
    case PendingWrite:
        return QObject::tr("Ожидание записи");
    case PendingRead:
        return QObject::tr("Ожидание сброса");
    case ErrorWrite:
        return QObject::tr("Ошибка записи");
    case ErrorRead:
        return QObject::tr("Ошибка сброса");
    default:
        return "MemoryState::Undefined";
    }
}
