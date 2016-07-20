#include "kamemorypart.h"

#include <QDebug>

using namespace Memory;



int KaMemoryPart::id() const
{
    return m_id;
}

void KaMemoryPart::setId(int id)
{
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
    m_start = start;
}

long KaMemoryPart::finish() const
{
    return m_finish;
}

void KaMemoryPart::setFinish(long finish)
{
    m_finish = finish;
}
KaMemoryPart::KaMemoryPart()
{
    m_state=Empty;
    m_start =0;
    m_finish = 0;
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

QList<KaMemoryPart> KaMemoryPart::selectDeviceParts(int idKaDevice)
{
    QList<KaMemoryPart> rez;
//    DbProcess dbp1(dbptConnectAndSqlCommand,"ka mem parts list","select * from koci.kamemorypart where id_device="+QString::number(idKaDevice));
//    dbp1.startDbProcess();
//    if(dbp1.getSqlResultIsActive())
//    {
//        QSqlQuery q1 = dbp1.getSqlResultToQuery();
//        while(q1.next())
//        {
//            int tmpiddevice = q1.value(q1.record().indexOf("id_device")).toInt();

//            if(tmpiddevice==idKaDevice)
//            {
//                KaMemoryPart part;
//                part.setData(q1.record());
//                rez.append(part);
//            }
//        }

//    }
    return rez;
}

void KaMemoryPart::setData(const QList<MemoryState>& list)
{
//    setBegin(record.value("from").toInt());
//    setEnd(record.value("to").toInt());
//    setStatus((MemoryState)record.value("type").toInt());
}




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
    return qstr+MemoryState_to_Qstring(status);
}

QString MemoryState_to_Qstring(MemoryState status)
{
    switch(status)
    {
//    case Freed:
//        return "Empty";
//    case Written:
//        return "Busy";
//    case Read:
//        return "Read";
//    case Available:
//        return "Free";
    case Empty:
        return "Empty";
    case Free:
        return "Free";
    case Recorded:
        return "Recorded";
    case PendingWrite:
        return "PendingWrite";
    case PendingRead:
        return "PendingRead";
    case ErrorWrite:
        return "ErrorWrite";
    case ErrorRead:
        return "ErrorRead";
    default:
        return "MemoryState::Undefined";
    }
}


QColor MemoryState_to_QColor(MemoryState state, bool isActive /*= true*/)
{
    QColor result;
//    if(!isActive)
//    {
//        // "GRAY" COLORS
//        switch(state)
//        {
//        case Memory::Freed:
//            result = QColor::fromRgb(220,220,220);
//            break;
//        case Memory::Written:
//            result = QColor::fromRgb(255,180,177);
//            break;
//        case Memory::Read:
//            result = QColor::fromRgb(255,229,177);
//            break;
//        case Memory::Available:
//            result = QColor::fromRgb(177,255,191);
//            break;
//        default:
//            qDebug() << "MemoryState_to_QColor() default case";
//            return Qt::black;
//        }
//    }
//    else
//    {
        // "HIGHLIGHTED" COLORS
//        switch(state)
//        {
//        case Memory::Freed:
//            result = QColor::fromRgb(250,250,250);
//            break;
//        case Memory::Written:
//            result = QColor::fromRgb(255,101,101);
//            break;
//        case Memory::Read:
//            result = QColor::fromRgb(250,130,16);
//            break;
//        case Memory::Available:
//            result = QColor::fromRgb(90,210,90);
//            break;
//        default:
//            qDebug() << "MemoryState_to_QColor() default case";
//            return Qt::black;
//        }
        switch (state) {
        case Empty:
            result = QColor(Qt::white);
            break;
        case Free:
            result = QColor(Qt::gray);
            break;
        case Recorded:
            result = QColor(Qt::green);
            break;
        case PendingWrite:
            result = QColor(255, 165, 0);
            break;
        case PendingRead:
            result = QColor(Qt::yellow);
            break;
        case ErrorWrite:
            result = QColor(Qt::magenta);
            break;
        case ErrorRead:
            result = QColor(Qt::red);
            break;
        default:
            result = QColor(Qt::black);
            break;
        }
//    }
    if(!isActive)
        return result.lighter(150);
    return result;
}
