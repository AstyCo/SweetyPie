#ifndef MGRID_UNIT_H
#define MGRID_UNIT_H

#include "kamemorypart.h"

#include <QGraphicsItem>
#include <QVariant>
#include <QPen>

class MGridItem;
class MGridScene;

class MGridUnit : public QGraphicsItem
{
public:
    MGridUnit(QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
        QWidget *widget = 0);

    int id() const;
    void setId(int id);

    Memory::MemoryState state() const;
    void setState(const Memory::MemoryState &state);

    long start() const;
    void setStart(long start);

    long finish() const;
    void setFinish(long finish);

    long size() const;
    void setSize(long newSize);

    qreal spacing() const;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    bool showBorder() const;
    void setShowBorder(bool showBorder);

    QColor color() const;

    void addItems(long start, long finish);
    long removeItems(long from, long count); ///< returns count of actually removed items
    void updateParenthesis();

    qreal extraSize() const;


private:
    void rebuildShape();

    QPainterPath shapeBorder() const;
    void setShapeBorder(const QPainterPath &shapeBorder);

    void setItems();
    friend class MGridScene;

private:
    bool m_unitSelected;

    int m_unitId;
    Memory::MemoryState m_state;
    MGridScene* m_scene;
    QList<MGridItem*>* m_items;

    QPainterPath m_shapeBorder;
    QPen m_borderPen;

    long m_start, m_finish;
    bool isEmpty;

};

#endif // MGRID_UNIT_H
