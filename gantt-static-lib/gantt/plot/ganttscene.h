#ifndef GANTTSCENE_H
#define GANTTSCENE_H

#include "gantt/private_extensions/mousepresshelper.h"
#include "scene_objects/ganttcurrentdtslider.h"
#include "extensions-gui/interfaces/abstractganttfactory.h"
#include "ganttdtline.h"

#include "hfitscene.h"
#include <QMap>
#include <QPointer>

//class GanttGraphicsView;
class GanttIntervalGraphicsObject;
class GanttCalcGraphicsObject;
class GanttGraphicsObject;
class GanttDtCrossObject;
class GanttHoverGraphicsObject;
class GanttInfoTree;

//class QModelIndex;

#include "gantt/info/ganttinfotree.h"

class GanttScene : public HFitScene
{
    Q_OBJECT

    void init();
public:
    GanttScene(GanttGraphicsView *view, GanttDtLine *dtline, QObject * parent = 0);
    int dtToPos(const UtcDateTime &dt) const;
    UtcDateTime posToDt(int pos) const;
    void setTreeInfo(GanttInfoTree *treeInfo);
    void setFactory(AbstractGanttFactory *factory);

    MousePressHelper *mousePressH();
    void setCursor(Qt::CursorShape cursor);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
public slots:
    void onTreeInfoReset();
    void onItemAdded(GanttInfoItem *item);
    void onExpanded(GanttInfoNode *which);
    void onCollapsed(GanttInfoNode *which);

    void drawBackground(QPainter *painter, const QRectF &rect); ///< Задний план содержит сетку

    GanttGraphicsObject* itemForInfo(const GanttInfoItem *key) const;

    void updateSliderHeight();
    UtcDateTime slidersDt() const;
    void setCurrentDt(const UtcDateTime &dt);
    const QList<GanttGraphicsObject *> &dtItems() const;

    void setDrawCurrentDtSlider(bool enable);

    QGraphicsItem *currentItem() const;


    bool isVisible(const QGraphicsItem* which) const;
    bool needVScroll(const QGraphicsItem *which) const;
    QRectF elementsBoundingRect();

    void clear();
    bool sceneHaveItems() const;


signals:
    void currentDtChanged(const UtcDateTime& dt);

    void graphicsItemHoverEnter(const GanttInfoItem*);
    void graphicsItemHoverLeave(const GanttInfoItem*);
    void currentItemChanged(const GanttInfoItem*);

public slots:
    void onViewResized(const QSize& newSize);
    void updateSceneRect();
    void updateSceneItems();
    void updateIntersections();         ///< updates all intersections

    void makeStep(int step);
    void moveSliderToNextEventStart();
    void moveSliderToPrevEventFinish();
    void moveSliderToViewStart();
    void moveSliderToViewFinish();
    void moveSliderToStart();
    void setCurrentItemByInfo(GanttInfoItem *info);
    void setCurrentItem(GanttGraphicsObject *currentItem);
    void addInfoItem(GanttInfoItem *parent);
    void addInfoItem(GanttInfoNode *parent, int from, int to);
    void onLimitsChanged(const UtcDateTime &first, const TimeSpan&ts);

    GanttGraphicsObject *objectForPos(const QPointF& pos);


private:
    void connectNewInfo(GanttInfoItem *info);
    void connectDtLine();

    void updateSlider();
    void updateItems();
    void createPersistentItems();
    void addPersistentItems();
    void removePersistentItems();
    void scrollViewToCurrentItem();

    void drawBackgroundExpandedItems(QPainter *painter, const QRectF &rect);
    void drawBackgroundLines(QPainter *painter, const QRectF &rect);
    void drawBackgroundExpandedNode(QPainter *painter, GanttInfoNode *node, int nest,
                                    int bgLeft, int bgWidth, bool isLast);
    void drawBackgroundExpandedNodeRect(QPainter *painter, int nest,
                                        int bgLeft, int bgWidth,
                                        int top, int bottom);

private slots:
    void invalidateBackground();
    void updateIntersectionR(GanttInfoItem *item);   ///< updates intersection recursively
    void onDoubleClick(const QPointF &pos); ///< screen pos
    void onClick(const QPointF &pos);   ///< screen pos
    void onVisItemDestroyed();
    void onInfoAboutToBeDeleted();
    void onGraphicsItemPress();
    void onGraphicsItemHoverEnter();
    void onGraphicsItemHoverLeave();

private:
    AbstractGanttFactory *_factory;

    GanttInfoTree *_treeInfo;
    GanttDtLine *_dtline;

    QList<GanttGraphicsObject*> _items;
    GanttCurrentDtSlider *_playerCurrent;
    GanttDtCrossObject *_crossObject;
    GanttHoverGraphicsObject *_hoverObject;
    QPointer<GanttGraphicsObject> _currentItem;

    MousePressHelper _mousePressH;
    QMap<const GanttInfoItem*, GanttGraphicsObject*> _itemForInfo;  // caches
    Qt::CursorShape _savedCursor;
};





#endif // GANTTSCENE_H
