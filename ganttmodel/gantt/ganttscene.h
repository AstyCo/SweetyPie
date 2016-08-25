#ifndef GANTTSCENE_H
#define GANTTSCENE_H


#include "ganttheader.h"
#include "ganttcurrenttimeslider.h"

#include <QGraphicsScene>
#include <QMap>

class GanttGraphicsView;
class GanttGraphicsItem;

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

    GanttGraphicsItem* itemByInfo(const GanttInfoLeaf* key) const;

    void updateHeaderPos(int dy);
    void updateSliderRect();

    void onViewAdded(QGraphicsView* view);

    void setHeaderMode(GanttHeader::GanttHeaderMode mode);
    GanttHeader::GanttHeaderMode headerMode() const;

    void changeExpanding(const QModelIndex& index);

    UtcDateTime slidersDt() const;

    GanttSlider *slider() const;

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

signals:
    void limitsChanged(const UtcDateTime &start, const UtcDateTime &finish);

    void currentDtChanged(const UtcDateTime& dt);
    void viewResized();

public slots:
    void onViewResize(const QSize& newSize);

    void makeStep(long long step);
    void moveSliderToNextEventStart();
    void moveSliderToPrevEventFinish();
    void moveSliderToViewStart();
    void moveSliderToViewFinish();
    void moveSliderToStart();



protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:

    void updateSlider();
    void addItemsHelper(GanttInfoItem* item);
    void updateItems();

private slots:
    void onInfoChanged();
    void onLeafStartChanged();
    void onLeafFinishChanged();

private:

    QList<GanttGraphicsItem*> m_items;
    QMap<const GanttInfoLeaf*, GanttGraphicsItem*> m_itemByInfo;
    QMap<UtcDateTime,const GanttInfoLeaf*> m_infoByStart,
                                            m_infoByFinish;
    GanttHeader *m_header;
    GanttSlider *m_slider;

    friend class GanttWidget;
};

#endif // GANTTSCENE_H
