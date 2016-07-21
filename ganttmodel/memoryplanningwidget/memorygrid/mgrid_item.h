#ifndef MGRID_ITEM_H
#define MGRID_ITEM_H

#include "kamemorypart.h"
#include "memory_globalvalues.h"

#include <QGraphicsLayoutItem>
#include <QGraphicsItem>
#include <QColor>

class MGridUnit;
class MGridScene;

class MGridtem : public QGraphicsLayoutItem, public QGraphicsItem
{
public:
    MGridtem(long index,qreal edgeLength = DEFAULT_EDGELENGTH,
               qreal borderWidth = DEFAULT_BORDERWIDTH,QGraphicsItem *parent = 0);
    virtual ~MGridtem();

    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &geom);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

    // Inherited from QGraphicsItem
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
        QWidget *widget = 0);


    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);


    QString toolTip() const;
    void setToolTip(const QString &toolTip);

    void enableToolTip();
    void disableToolTip();



    QVariant itemChange(GraphicsItemChange change, const QVariant &value);


    void setParentUnitSelected(bool selected);

    Memory::MemoryState state() const;
    QColor color() const;

    long index() const;
    void setIndex(long index);

    qreal edgeLength() const;
    void setEdgeLength(qreal edgeLength);

    qreal borderWidth() const;
    void setBorderWidth(const qreal &borderWidth);

    void disableSizeModify();


    qreal sizeModify() const;
    void setSizeModify(qreal sizeModify);

    bool isHighlighted() const;

    MGridUnit *parentUnit() const;
    void setParentUnit(MGridUnit *parentUnit);


public slots:

private:

    void drawHighlighted(QPainter *painter, const QStyleOptionGraphicsItem *option,
                         QWidget *widget = 0);
    void drawBlurred(QPainter *painter, const QStyleOptionGraphicsItem *option,
                         QWidget *widget = 0);


private:

    MGridScene* m_scene;

    qreal m_edgeLength;
    qreal m_borderWidth;

    long m_index;

    MGridUnit* m_parentUnit;

    qreal m_sizeModify;
    bool m_sizeModifying;

};

#endif // MGRID_ITEM_H
