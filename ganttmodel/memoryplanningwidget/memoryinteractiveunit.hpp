#ifndef MEMORYINTERACTIVEUNIT_HPP
#define MEMORYINTERACTIVEUNIT_HPP

#include "memoryitempresentation.hpp"
#include "memoryscene.hpp"

#include <QGraphicsItem>
#include <QVariant>
#include <QPen>

class MemoryItem;

class MemoryInteractiveUnit : public QGraphicsItem
{
public:
    MemoryInteractiveUnit(MemoryScene* scene,QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
        QWidget *widget = 0);

    bool inRange(long index) const;


    long start() const;
    long finish() const;
    long size() const;

    qreal spacing() const;

    void setRange(long start, long finish);

    void setShowBorders(bool value);

    long memorySize() const;


private:
    void setStart(long start);
    void setFinish(long finish);
    void setSize(long newSize);

    void rebuildShape();

    QPainterPath shapeBorder() const;
    void setShapeBorder(const QPainterPath &shapeBorder);

private:

    QPainterPath m_shapeBorder;
    QPen m_borderPen;

    QList<MemoryItem*>* m_items;
    long m_start, m_finish;

    bool m_showBorders;

    bool m_initialized;

};

#endif // MEMORYINTERACTIVEUNIT_HPP

