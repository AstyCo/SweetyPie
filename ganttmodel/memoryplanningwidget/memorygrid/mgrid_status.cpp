#include "mgrid_status.h"

#include "mgrid_scene.h"

#include <QPainter>

#include <QFontMetrics>
#include <QStyleOptionGraphicsItem>

#include <QDebug>

extern MGridScene* mem_scene;

MGridStatus::MGridStatus(QGraphicsItem* parent)
    : QGraphicsItem(parent), QGraphicsLayoutItem()
{
    setGraphicsItem(this);
    m_lastLod = 0;

    setHeight(DEFAULT_STATUS_HEIGHT);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);
}

MGridStatus::~MGridStatus()
{
}

void MGridStatus::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}

QSizeF MGridStatus::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    MGridScene* p_scene =dynamic_cast<MGridScene*>(scene());

    if(p_scene)
    {
        switch (which) {
        case Qt::MinimumSize:
        case Qt::PreferredSize:
        case Qt::MaximumSize:
            return boundingRect().size();
        default:
            break;
        }
    }
    return constraint;
}

QRectF MGridStatus::boundingRect() const
{
    return QRectF( 0, 0, parentItem()->boundingRect().width(), height());
}

void MGridStatus::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

//    qDebug() << "MemoryStatus::paint";
//    qDebug() << boundingRect();


    QFontMetrics fm(m_font);
    m_statusLabelRect = fm.tightBoundingRect(statusLabel());
    m_statusLabelRect.setHeight(boundingRect().height());
//    qDebug() << m_statusLabelRect;

    qreal tmpVal = (boundingRect().width()-m_statusLabelRect.width())/2;
    m_itemInfoRect = QRect(m_statusLabelRect.width()+5,0,tmpVal,boundingRect().height());
    m_unitInfoRect = QRect(m_statusLabelRect.width()+5+tmpVal,0,tmpVal,boundingRect().height());

//    qDebug() << m_itemInfoRect;
//    qDebug() << m_unitInfoRect;


    QPen textPen(Qt::black);
    painter->fillRect(boundingRect(),QBrush(Qt::white));
    painter->setPen(textPen);
    painter->setFont(m_font);
//    qDebug()<<geometry();
    painter->drawText(QRectF(QPoint(5,0),m_statusLabelRect.size()),Qt::AlignLeft|Qt::AlignVCenter,statusLabel());

    painter->drawText(m_itemInfoRect,Qt::AlignCenter,itemInfo());
    painter->drawText(m_unitInfoRect,Qt::AlignCenter,unitInfo());

}
QString MGridStatus::statusLabel() const
{
    return m_statusLabel;
}

void MGridStatus::setStatusLabel(const QString &statusLabel)
{
    m_statusLabel = statusLabel;
}
QString MGridStatus::itemInfo() const
{
    return m_itemInfo;
}

void MGridStatus::setItemInfo(const QString &itemInfo)
{
    m_itemInfo = itemInfo;
}
QString MGridStatus::unitInfo() const
{
    return m_unitInfo;
}

void MGridStatus::setUnitInfo(const QString &unitInfo)
{
    m_unitInfo = unitInfo;
}


void MGridStatus::transformChanged(const QTransform &transform)
{
    qreal vScaling = transform.m22();

    setHeight(DEFAULT_STATUS_HEIGHT/vScaling);

}
qreal MGridStatus::height() const
{
    return m_height;
}

void MGridStatus::setHeight(const qreal &height)
{
    m_height = height;
}
QFont MGridStatus::font() const
{
    return m_font;
}

void MGridStatus::setFont(const QFont &font)
{
    m_font = font;
}






