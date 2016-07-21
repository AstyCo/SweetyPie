#ifndef MGRID_LABELITEM_H
#define MGRID_LABELITEM_H

#include "memory_globalvalues.h"

#include <QGraphicsLayoutItem>
#include <QGraphicsItem>
#include <QColor>

#include <QString>


class MGridLabelItem : public QGraphicsLayoutItem, public QGraphicsItem
{
public:
    MGridLabelItem(  const QString& label = DEFAULT_LABEL,
                QGraphicsItem *parent = 0,
                qreal edgeLength = DEFAULT_EDGELENGTH,
                qreal borderWidth = DEFAULT_BORDERWIDTH,
                const QColor& color = DEFAULT_LABELCOLOR);
    virtual ~MGridLabelItem();

    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &geom);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

    // Inherited from QGraphicsItem
    QRectF boundingRect() const;
//    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
        QWidget *widget = 0);


    QString getLabel() const;
    void setLabel(const QString &value);

    QColor getColor() const;
    void setColor(const QColor &color);

    int getEdgeLength() const;
    void setEdgeLength(int edgeLength);

    qreal getBorderWidth() const;
    void setBorderWidth(const qreal &borderWidth);

public slots:

private:

    int m_edgeLength;
    qreal m_borderWidth;
    QString m_label;
    QColor m_color;

};

#endif // MGRID_ITEM_H

