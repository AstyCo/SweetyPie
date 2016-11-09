#ifndef MGRID_UNIT_H
#define MGRID_UNIT_H

#include "memorypart.h"

#include <QGraphicsItem>
#include <QVariant>
#include <QPen>

class MGridItem;
class MGridScene;

/*!
 * \brief The MGridUnit class

    Отвечает за отображение кусков памяти (интервалов).


*/

class MGridUnit : public QGraphicsItem
{
public:
    MGridUnit(QSharedPointer<KaMemoryPart> memoryPart,QGraphicsScene *scene,QGraphicsItem *parent = 0);
    void initialize();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
        QWidget *widget = 0);

    int id() const;
    void setId(int id);

    KaMemoryPart::KaMemoryState state() const;
    void setState(const KaMemoryPart::KaMemoryState &state);

    long start() const;
    void setStart(long start);

    long length() const;
    void setLength(long newSize);

    qreal spacing() const;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    bool showBorder() const;
    void setShowBorder(bool showBorder);

    QColor color();

    void addItems(long start, long length);
    long removeItems(long from, long count); ///< returns count of actually removed items
    void updateParenthesis();

    qreal extraSize() const;

    QSharedPointer<KaMemoryPart> kaMemoryPart() const;
    void setKaMemoryPart(const QSharedPointer<KaMemoryPart> &kaMemoryPart);

private:
    void rebuildShape();

    void setScene(MGridScene* scene);

    QPainterPath shapeBorder() const;
    void setShapeBorder(const QPainterPath &shapeBorder);

    friend class MGridScene;

private:
    QSharedPointer<KaMemoryPart> m_kaMemoryPart;
    bool m_unitSelected;

    MGridScene* m_scene;
    QVector<MGridItem*>* m_items;

    QPainterPath m_shapeBorder;
    QPen m_borderPen;
};

#endif // MGRID_UNIT_H
