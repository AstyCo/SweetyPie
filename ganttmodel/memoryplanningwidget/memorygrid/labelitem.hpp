#ifndef LABELITEM_HPP
#define LABELITEM_HPP

#include "globalvalues.hpp"

#include <QGraphicsLayoutItem>
#include <QGraphicsItem>
#include <QColor>

#include <QString>


class LabelItem : public QGraphicsLayoutItem, public QGraphicsItem
{
public:
    LabelItem(  const QString& label = DEFAULT_LABEL,
                QGraphicsItem *parent = 0,
                qreal edgeLength = DEFAULT_EDGELENGTH,
                qreal borderWidth = DEFAULT_BORDERWIDTH,
                const QColor& color = DEFAULT_LABELCOLOR);
    virtual ~LabelItem();

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

#endif // LABELITEM_HPP

