#ifndef GANTTWIDGET_H
#define GANTTWIDGET_H

#include "ganttproject_global.h"
#include "ganttinfoleaf.h"

#include <QModelIndex>

#include <QList>

#include <QWidget>


namespace Ui {
class GanttWidget;
}

class GanttScene;
class GanttTreeModel;
class GanttHeader;

class GanttInfoNode;
class GanttInfoItem;
class GanttTreeView;

class GANTTMODELSHARED_EXPORT GanttWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GanttWidget(QWidget *parent = 0);
    ~GanttWidget();

    void showPlayer(bool show);
    bool player() const;

    void addItems(GanttInfoItem* item);
    void addItems(const QList<GanttInfoItem*>& items);

    UtcDateTime slidersDt() const;

    UtcDateTime minDt() const;
    const UtcDateTime &outerMinDt() const;

    UtcDateTime maxDt() const;
    const UtcDateTime &outerMaxt() const;

    GanttInfoNode *nodeByName(const QString&title) const;

    static void callForEachItem(GanttInfoItem* item,GanttTreeView* view,GanttScene* scene,
                         void (*func)(GanttInfoItem*,GanttTreeView*,GanttScene*));

    void clear();

    GanttInfoItem *itemAtPos(const QPoint& widgetPoint) const;

signals:
    void currentDtChanged(const UtcDateTime& dt);

    void graphicsItemHoverEnter(const GanttInfoItem*);
    void graphicsItemHoverLeave(const GanttInfoItem*);
    void currentItemChanged(const GanttInfoItem*);

private slots:
//    void on_comboBox_mode_currentIndexChanged(int inde);

    void expanded(const QModelIndex& index);
    void collapsed(const QModelIndex& index);

    void onSliderMoved();

    void onGanttViewCustomContextMenuRequested(const QPoint&);

public slots:
//    void repaintDtHeader();
    void updateRange(const UtcDateTime& min, const UtcDateTime& max);
    void updateSliderLimits();
    // ZOOM +
    void newLimits();
    void newLimits(const UtcDateTime& min, const UtcDateTime& max);
    void prevLimits();
    // ZOOM -

    void setCurrentItem(const GanttInfoItem* item);
    void onTreeViewEntered(const QModelIndex& index);

private slots:
    void pushLimits();


private:
    void updatePos(GanttInfoNode* from);
    void updatePosHelper(GanttInfoItem* item);

    static void connectSignalsToNewItems(GanttInfoItem* items,GanttTreeView* view,GanttScene* scene);

private:
    Ui::GanttWidget *ui;

    GanttScene *m_scene;
    GanttTreeModel *m_model;

    qreal m_curSceneMax;
    bool m_playerShown;

    UtcDateTime m_minDt, m_maxDt;

    QVector<std::pair<UtcDateTime,UtcDateTime> > m_stackLimits;

    friend class GanttScene;
    friend class MyGanttView;
    friend class GanttHeader;
};

#endif // GANTTWIDGET_H
