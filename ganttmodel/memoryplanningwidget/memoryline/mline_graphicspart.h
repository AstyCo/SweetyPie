#ifndef MLINE_GRAPHICSPART_H
#define MLINE_GRAPHICSPART_H

#include "mline_abstractpart.h"
//#include "QSqlRecord"
#include "memorypart.h"

#include <ganttproject_global.h>


class MLineGraphicsPart : public MLineAbstractPart
{
    Q_OBJECT
private:
    MemoryPart::MemoryState _status;
    QColor statusColor() const;

public:
    MLineGraphicsPart(QGraphicsItem *parent =0);
    ~MLineGraphicsPart();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;  ///< used for update-on-fly
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void setKaMemoryPart(const MemoryPart &part);

    MemoryPart::MemoryState status() const;
    void setStatus(const MemoryPart::MemoryState &status);
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

#endif // MLINE_GRAPHICSPART_H
