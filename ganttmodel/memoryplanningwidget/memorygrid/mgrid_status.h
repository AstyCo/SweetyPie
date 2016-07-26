/*! \file
 * MGrid_Status - Obsolete member of MGridWidget
 *
*/

#ifndef MGRID_STATUS_H
#define MGRID_STATUS_H

#include <QGraphicsItem>
#include <QGraphicsLayoutItem>

#include <QString>
#include <QFont>




class MGridStatus : public QGraphicsItem, public QGraphicsLayoutItem
{
public:
    MGridStatus(QGraphicsItem* parent = 0);
    virtual ~MGridStatus();

    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &geom);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

    // Inherited from QGraphicsItem
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
        QWidget *widget = 0);

    QString statusLabel() const;
    void setStatusLabel(const QString &statusLabel);

    QString itemInfo() const;
    void setItemInfo(const QString &itemInfo);

    QString unitInfo() const;
    void setUnitInfo(const QString &unitInfo);

//    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void transformChanged(const QTransform& transform);

    qreal height() const;
    void setHeight(const qreal &height);

    QFont font() const;
    void setFont(const QFont &font);

private:
    QString m_statusLabel;
    QString m_itemInfo;
    QString m_unitInfo;

    QRect   m_statusLabelRect,
            m_itemInfoRect,
            m_unitInfoRect;

    qreal m_lastLod;

    qreal m_height;
    QFont m_font;

};

#endif // MGRID_STATUS_H
