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

class MGridStatus;

class MGridWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit MGridWidget(QGraphicsWidget *parent = 0);
    ~MGridWidget();

    int itemPerRow() const;
    void setItemPerRow(int itemPerRow);
    void setupMatrix(QList<MGridtem*> mem_items);

    bool labels() const;
    void setLabels(bool labels);

    qreal spacing() const;
    void setSpacing(const qreal &spacing);

    void setStatusLabel(const QString& text);
    QString statusLabel() const;
    void setItemInfo(const QString& text);
    QString itemInfo() const;
    void setUnitInfo(const QString& text);
    QString unitInfo() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void memoryStatusUpdate(const QRectF& rect = QRectF());

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void removeLabels();

    qreal margins() const;
    void setMargins(const qreal &margins);

public slots:

    void transformChanged(const QTransform& transform);


private:
    int toColumn(int index) const;
    int toRow(int index) const;

    int fromPos(int row,int column) const;
    int toMemoryAdress(int row,int column) const;
    int toMemoryAdress(int pos) const;

private:

    QGraphicsLayout *layout();

    int m_itemPerRow;
    bool m_labels;
    QList<MGridLabelItem*> m_labelItems;
    qreal m_spacing;
    qreal m_margins;
    MGridStatus* m_memoryStatus;

    friend class MGridScene;
};

#endif // MEMORYWIDGET_H
