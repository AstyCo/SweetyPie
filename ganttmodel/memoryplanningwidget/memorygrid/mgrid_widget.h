#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

//#include "memoryitempresentation.hpp"
#include "mgrid_item.h"

#include <QGraphicsWidget>

#include <QPoint>
#include <QColor>
#include <QList>

#include <QStatusBar>
#include <QLabel>

class MGridLabelItem;

class MGridWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit MGridWidget(QGraphicsWidget *parent = 0);
    ~MGridWidget();

    int itemPerRow() const;
    void setItemPerRow(int itemPerRow);
    void setupMatrix(QVector<MGridItem*> mem_items);

    bool labels() const;
    void setLabels(bool labels);

    qreal spacing() const;
    void setSpacing(const qreal &spacing);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void removeLabels();

    qreal margins() const;
    void setMargins(const qreal &margins);

protected:
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    int toColumn(int index) const;
    int toRow(int index) const;

    int fromPos(int row,int column) const;
    int toMemoryAdress(int row,int column) const;
    int toMemoryAdress(int pos) const;

private:

    int m_itemPerRow;
    bool m_labels;
    QList<MGridLabelItem*> m_labelItems;
    qreal m_spacing;
    qreal m_margins;

    friend class MGridScene;
};

#endif // MEMORYWIDGET_H
