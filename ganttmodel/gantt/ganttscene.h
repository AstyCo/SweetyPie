#ifndef GANTTSCENE_H
#define GANTTSCENE_H


#include "ganttheader.h"
#include "ganttcurrentdtslider.h"

#include <QGraphicsScene>
#include <QMap>

class GanttGraphicsView;
class GanttGraphicsObject;
class GanttCalcGraphicsObject;

class QModelIndex;

class GanttScene : public QGraphicsScene
{
    Q_OBJECT

public:
    GanttScene(QObject * parent = 0);


    void updateWidth(int w);
    void updateHeight(int h);

    void drawBackground(QPainter *painter, const QRectF &rect); ///< Задний план содержит сетку

    void setMode(GanttHeader::GanttPrecisionMode newMode);

    void addItems(GanttInfoItem* items);
    void addItems(const QList<GanttInfoItem*>& items);

    QGraphicsObject* itemByInfo(const GanttInfoItem *key) const;

    void updateHeaderPos(int dy);
    void updateSliderRect();

    void onViewAdded(QGraphicsView* view);

    void setHeaderMode(GanttHeader::GanttHeaderMode mode);
    GanttHeader::GanttHeaderMode headerMode() const;

    void changeExpanding(const QModelIndex& index);

    UtcDateTime slidersDt() const;

    GanttCurrentDtSlider *slider() const;

    void setRange(UtcDateTime min, UtcDateTime max);


    UtcDateTime minDt() const;
    UtcDateTime maxDt() const;
    UtcDateTime startDt() const;
    UtcDateTime finishDt() const;

    UtcDateTime xToDt(qreal x) const;
    qreal dtToX(const UtcDateTime& dt) const;

    UtcDateTime startByDt(const UtcDateTime& dt,GanttHeader::GanttPrecisionMode mode) const;
    UtcDateTime startByDt(const UtcDateTime& dt) const;

    UtcDateTime finishByDt(const UtcDateTime& dt,GanttHeader::GanttPrecisionMode mode) const;
    UtcDateTime finishByDt(const UtcDateTime& dt) const;

    UtcDateTime nextStart(const UtcDateTime& dt,GanttHeader::GanttPrecisionMode mode) const;
    UtcDateTime nextStart(const UtcDateTime& dt) const;

    UtcDateTime prevFinish(const UtcDateTime& dt,GanttHeader::GanttPrecisionMode mode) const;
    UtcDateTime prevFinish(const UtcDateTime& dt) const;

    GanttHeader::GanttPrecisionMode headerPrecisionMode() const;
    GanttHeader::GanttPrecisionMode calculateTimeMode(const UtcDateTime &min, const UtcDateTime &max) const;

    void emitLimitsChanged(const UtcDateTime &start, const UtcDateTime &finish);

    long long minTimeUnit() const;

    const GanttInfoLeaf *nextEvent(const UtcDateTime &curDt) const;
    const GanttInfoLeaf *prevEvent(const UtcDateTime &curDt) const;
    void removeByInfo(const GanttInfoItem* item);
    void removeByInfoLeaf(const GanttInfoLeaf* leaf);
    void setEmpty(bool empty);

    const QList<GanttGraphicsObject *>& dtItems() const;
    void removeItem(QGraphicsItem *item);

    void setDrawCurrentDtSlider(bool enable);

    QGraphicsItem *currentItem() const;

    void setSceneRect(const QRectF &rect);
    void setSceneRect(qreal x,qreal y, qreal width, qreal height);

signals:
    void limitsChanged(const UtcDateTime &start, const UtcDateTime &finish);

    void currentDtChanged(const UtcDateTime& dt);
    void viewResized();

    void graphicsItemHoverEnter(const GanttInfoItem*);
    void graphicsItemHoverLeave(const GanttInfoItem*);

    void currentItemChanged(QGraphicsItem*);



public slots:
    void onViewResize(const QSize& newSize);
    void updateSceneRect();

    void makeStep(long long step);
    void moveSliderToNextEventStart();
    void moveSliderToPrevEventFinish();
    void moveSliderToViewStart();
    void moveSliderToViewFinish();
    void moveSliderToStart();
    void setCurrentItem(QGraphicsItem *currentItem);



protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:

    void updateSlider();
    void addItemsHelper(GanttInfoItem* item);
    void updateItems();

private slots:
    void onGraphicsItemHoverEnter();
    void onGraphicsItemHoverLeave();

    void onInfoDelete();
    void onInfoLeafDelete();
    void onInfoChanged();
    void onLeafStartChanged();
    void onLeafFinishChanged();

private:

    QList<GanttGraphicsObject*> m_items;
    QList<GanttCalcGraphicsObject*> m_calcItems;
    QMap<const /*GanttInfoLeaf*/GanttInfoItem*, /*GanttGraphicsObject*/QGraphicsObject*> m_itemByInfo;
    QMap<UtcDateTime,const GanttInfoLeaf*> m_infoByStart,
                                            m_infoByFinish;
    GanttHeader *m_header;
    GanttCurrentDtSlider *m_slider;
    QGraphicsItem *m_currentItem;

    friend class GanttWidget;
};



#endif // GANTTSCENE_H
