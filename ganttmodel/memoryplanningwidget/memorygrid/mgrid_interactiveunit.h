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
    long finish() const;
    long size() const;

    void disable();

    void setRange(long start, long finish);

    long memorySize() const;

    void rebuildShape();

private:
    void setStart(long start);
    void setFinish(long finish);
    void setSize(long newSize);

    QPainterPath shapeBorder() const;
    void setShapeBorder(const QPainterPath &shapeBorder);

    qreal extraSize() const;

private:

    QPainterPath m_shapeBorder;
    QPen m_borderPen;
    MGridScene *m_scene;

    QVector<MGridItem*>* m_items;
    long m_start, m_finish;

    bool m_enabled;

};

#endif // MGRID_INTERACTIVEUNIT_H

