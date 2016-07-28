#ifndef MGRID_ITEM_H
#define MGRID_ITEM_H

#include "kamemorypart.h"
#include "memory_globalvalues.h"

#include <QGraphicsLayoutItem>
#include <QGraphicsItem>
#include <QColor>

class MGridUnit;
class MGridScene;

/*!
 * \brief The MGridItem class
 *
 * Отвечает за отображение блоков памяти.
 *
 */

class MGridItem : public QGraphicsItem
{
public:
    MGridItem(long index,QGraphicsScene* scene,qreal edgeLength = DEFAULT_EDGELENGTH,
               qreal borderWidth = DEFAULT_BORDERWIDTH,QGraphicsItem *parent = 0);
    virtual ~MGridItem();

    QRectF geometry() const;
    qreal left() const;
    qreal right() const;
    qreal top() const;
    qreal bottom() const;
    void setScene(MGridScene* scene);

    // Inherited from QGraphicsItem
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
        QWidget *widget = 0);


    QString toolTip() const;
    void setToolTip(const QString &toolTip);

    void enableToolTip();
    void disableToolTip();


    void setUnitSelected(bool selected);

    Memory::MemoryState state() const;
    QColor color() const;

    long index() const;
    void setIndex(long index);

    qreal edgeLength() const;
    void setEdgeLength(qreal edgeLength);

    qreal borderWidth() const;
    void setBorderWidth(const qreal &borderWidth);

    bool isHighlighted() const;

    MGridUnit *unit() const;
    void setUnit(MGridUnit *unit);


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

    MGridUnit* m_unit;

};

#endif // MGRID_ITEM_H
