#include "kamemorygraphicspart.h"

#include <QBrush>
#include <QPainter>
#include <QDebug>
#include <qstyleoption.h>
//#include "dbprocess.h"
#include "kamemoryscene.h"

//setToolTip(QString::number(size()) + QObject::trUtf8(" КБ"));


void KaMemoryGraphicsPart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(isSelected())
    {
        painter->setBrush(QBrush(statusColor().lighter()));
    }
    else
        painter->setBrush(QBrush(statusColor()));

    qDebug()<<begin()<<"-"<<end()<<" "<<width();
    setX(xByVal(begin()));

    updateVisualElements();

    qDebug() <<m_rect;

    painter->drawRect(m_rect);

    painter->drawLine(m_leftLine);
    if(m_rect.width()>20)
        painter->drawText(m_beginText, Qt::AlignCenter, QString::number(begin()));

    painter->drawLine(m_rightLine);
    if(m_rect.width()>20)
        painter->drawText(m_endText, Qt::AlignCenter, QString::number(end()));

    if(m_rect.width()>20)
        painter->drawText(m_rect, Qt::AlignCenter, QString::number(size())/* + QObject::trUtf8(" КБ") */);

}

QRectF KaMemoryGraphicsPart::boundingRect() const
{
    return m_boundingRect;
}

QVariant KaMemoryGraphicsPart::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemSceneHasChanged)
    {
        updateVisualElements();
    }
    return QGraphicsItem::itemChange(change,value);
}







void KaMemoryGraphicsPart::setKaMemoryPart(const KaMemoryPart &part)
{
    _status = part.state();
    setBegin(part.begin());
    setEnd(part.end());

    updateVisualElements();
}



MemoryState KaMemoryGraphicsPart::status() const
{
    return _status;
}

void KaMemoryGraphicsPart::setStatus(const MemoryState &status)
{
    _status = status;
}

void KaMemoryGraphicsPart::updateVisualElements()
{
    m_rect = QRectF(0,0, width(), 20);

    m_leftLine = QLineF(m_rect.left(), m_rect.bottom()+10, m_rect.left(), m_rect.bottom()+20);

    m_rightLine = QLineF(m_rect.left(), m_rect.bottom()+10, m_rect.left(), m_rect.bottom()+20);

    m_beginText = QRectF(m_rect.left()-10,m_rect.bottom()+20,25,15);

    m_endText = QRectF(m_rect.right()-10,m_rect.bottom()+20,25,15);

    m_boundingRect|=m_rect;
    m_boundingRect|=QRectF(m_leftLine.p1(),m_leftLine.p2());
    m_boundingRect|=QRectF(m_rightLine.p1(),m_rightLine.p2());
    m_boundingRect|=m_beginText;
    m_boundingRect|=m_endText;

}
QColor KaMemoryGraphicsPart::statusColor() const
{
    switch (_status) {
    case Empty:
        return QColor(Qt::white);
        break;
    case Free:
        return QColor(Qt::gray);
        break;
    case Recorded:
        return QColor(Qt::green);
        break;
    case PendingWrite:
        return QColor(255, 165, 0);
        break;
    case PendingRead:
        return QColor(Qt::yellow);
        break;
    case ErrorWrite:
        return QColor(Qt::magenta);
        break;
    case ErrorRead:
        return QColor(Qt::red);
        break;
    default:
        return QColor(Qt::black);
        break;
    }
}



int KaMemoryPart::begin() const
{
    return _begin;
}

void KaMemoryPart::setBegin(int begin)
{
    _begin = begin;
}

int KaMemoryPart::end() const
{
    return _end;
}

void KaMemoryPart::setEnd(int end)
{
    _end = end;
}
KaMemoryPart::KaMemoryPart()
{
    _state=Empty;
    _begin =0;
    _end = 0;
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




KaMemoryGraphicsPart::KaMemoryGraphicsPart(QGraphicsItem *parent):
    KaMemoryAbstractPart(parent)
{
    _status=Empty;
    setFlags(ItemIsSelectable);


}

KaMemoryGraphicsPart::~KaMemoryGraphicsPart()
{

}
