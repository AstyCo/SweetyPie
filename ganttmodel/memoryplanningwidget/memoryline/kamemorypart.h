#ifndef KAMEMORYPART_H
#define KAMEMORYPART_H

#include "kamemoryabstractpart.h"
//#include "QSqlRecord"
#include "memoryplanningwidget/memoryitempresentation.hpp"

#include <ganttmodel_global.h>

using namespace Memory;

class GANTTMODELSHARED_EXPORT KaMemoryPart
{
private:
    MemoryState _state;
    int _begin;
    int _end;

public:
    KaMemoryPart();
    ~KaMemoryPart();

    static QMultiMap<int, KaMemoryPart> selectAll();
    static QList<KaMemoryPart> selectDeviceParts(int idKa);

    void setData(const QList<MemoryState>& list);


    MemoryState state() const;
    void setState(const MemoryState &state);


    bool operator ==(const KaMemoryPart &part)
    {
        return begin() == part.begin() &&
                end() == part.end();
    }

    bool operator !=(const KaMemoryPart &part)
    {
        return !(*this==part);
    }

    int size() const;

    int begin() const;
    void setBegin(int begin);
    int end() const;
    void setEnd(int end);
};

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

#endif // KAMEMORYPART_H
