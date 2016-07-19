#ifndef KAMEMORYGRAPHICSPART_H
#define KAMEMORYGRAPHICSPART_H

#include "kamemoryabstractpart.h"
//#include "QSqlRecord"
#include "kamemorypart.h"

#include <ganttmodel_global.h>

using namespace Memory;

class KaMemoryGraphicsPart : public KaMemoryAbstractPart
{
    Q_OBJECT
private:
    MemoryState _status;
    QColor statusColor() const;

public:
    KaMemoryGraphicsPart(QGraphicsItem *parent =0);
    ~KaMemoryGraphicsPart();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;  ///< used for update-on-fly
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void setKaMemoryPart(const KaMemoryPart &part);

    MemoryState status() const;
    void setStatus(const MemoryState &status);
private:
    // VISUAL ELEMENTS
    void updateVisualElements();

    QRectF m_rect,
            m_beginText,
            m_endText;
    QLineF m_leftLine,
            m_rightLine;

    QRectF m_boundingRect;
};

#endif // KAMEMORYGRAPHICSPART_H
