#ifndef MGRID_INTERACTIVEUNIT_H
#define MGRID_INTERACTIVEUNIT_H

#include "kamemorypart.h"
#include "mgrid_scene.h"

#include <QGraphicsItem>
#include <QVariant>
#include <QPen>

class MGridItem;

/*!
 * \brief The MGridInteractiveUnit class
 *
 * Отвечает за отображение выбираемого интервала памяти.
 *
 *
 */

class MGridInteractiveUnit : public QGraphicsItem
{
public:
    MGridInteractiveUnit(MGridScene* scene,QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
        QWidget *widget = 0);

    bool inRange(long index) const;


    long start() const;

    void disable();

    void setRange(long start, long length);
    void setStart(long start);

    long memorySize() const;

    void rebuildShape();

    long length() const;
    void setLength(long length);

private:
    void setSize(long newSize);

    QPainterPath shapeBorder() const;
    void setShapeBorder(const QPainterPath &shapeBorder);

    qreal extraSize() const;

private:

    QPainterPath m_shapeBorder;
    QPen m_borderPen;
    MGridScene *m_scene;

    QVector<MGridItem*>* m_items;
    long m_start, m_length;

    bool m_enabled;


};

#endif // MGRID_INTERACTIVEUNIT_H

