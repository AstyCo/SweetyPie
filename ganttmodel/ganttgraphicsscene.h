#ifndef GANTTGRAPHICSSCENE_H
#define GANTTGRAPHICSSCENE_H

#include <QGraphicsScene>
//#include <QAbstractItemModel>
#include "ganttmodel.h"

#include "ganttgraphicsheader.h"
#include "ganttgraphicsheaderitem.h"
#include "ganttgraphicsitem.h"

class GanttGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GanttGraphicsScene(GanttModel * model, /*QDateTime begin, QDateTime end,*/ QObject *parent = 0);
    ~GanttGraphicsScene();

    void drawBackground(QPainter *painter, const QRectF &_rect);
    void drawForeground(QPainter *painter, const QRectF &rect);

    GanttGraphicsHeader * m_header;

    GanttGraphicsHeaderItem * m_headerItem;
    //GanttGraphicsItem * createGraphicsItem(GanttItem * item);

    void setInterval(QDateTime begin, QDateTime end);

    void calcRowFromIndex();

    int setItems(QModelIndex parent, int rowIndex);

    QRectF m_backgroundRect;

    qreal cursor() const;
    void setCursor(const qreal &cursor);

signals:
    void cursorChanged(qreal);

public slots:

    void deleteGraphicsItem(QModelIndex index);
    void updateItems(GanttModel * model);

    void editAdd(QModelIndex index);
    void editMoveUp(QModelIndex index);

    void onScaleChanged(Scale zoom);




private slots:
    void onExpanded(QModelIndex index);
    void onCollapsed(QModelIndex index);
    void onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
    void onRowsInserted(const QModelIndex & parent, int start, int end );
    void onRowsRemoved(const QModelIndex & parent, int start, int end );
    void onRowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);
    void onMoveUp(QModelIndex index);

    void onTimer();

    void shrinkScene();

    void onCurrentValueChanged(qreal currentValue);

private:
    GanttModel * m_model;
    QModelIndex * m_index;


    QGraphicsWidget * test;
    QGraphicsLinearLayout * m_layout;
    QGraphicsLinearLayout * m_itemLayout;
    GanttGraphicsItem * m_item;

    QDateTime m_begin;
    QDateTime m_end;

    Scale m_scale;
    Zoom m_zoom;

    qreal m_cursor;

    QList<QPersistentModelIndex> m_proxyList;

    int m_beginDeleteRow;
    int m_endDeleteRow;

};

#endif // GANTTGRAPHICSSCENE_H
