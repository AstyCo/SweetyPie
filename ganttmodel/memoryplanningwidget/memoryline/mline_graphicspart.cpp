#include "mline_graphicspart.h"

#include <QBrush>
#include <QPainter>
#include <QDebug>
#include <qstyleoption.h>
//#include "dbprocess.h"
#include "mline_scene.h"

//setToolTip(QString::number(size()) + QObject::trUtf8(" КБ"));


void MLineGraphicsPart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(isSelected())
    {
        painter->setBrush(QBrush(statusColor().lighter()));
    }
    else
        painter->setBrush(QBrush(statusColor()));

//    qDebug()<<begin()<<"-"<<end()<<" "<<width();
    setX(xByVal(begin()));

    updateVisualElements();

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

QRectF MLineGraphicsPart::boundingRect() const
{
    return m_boundingRect;
}

QVariant MLineGraphicsPart::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemSceneHasChanged)
    {
        updateVisualElements();
    }
    return QGraphicsItem::itemChange(change,value);
}







void MLineGraphicsPart::setKaMemoryPart(const KaMemoryPart &part)
{
    _status = part.state();
    setBegin(part.start());
    setEnd(part.finish());

    updateVisualElements();
}



MemoryState MLineGraphicsPart::status() const
{
    return _status;
}

void MLineGraphicsPart::setStatus(const MemoryState &status)
{
    _status = status;
}

void MLineGraphicsPart::updateVisualElements()
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
QColor MLineGraphicsPart::statusColor() const
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



MLineGraphicsPart::MLineGraphicsPart(QGraphicsItem *parent):
    MLineAbstractPart(parent)
{
    _status=Empty;
    setFlags(ItemIsSelectable);


}

MLineGraphicsPart::~MLineGraphicsPart()
{

}
