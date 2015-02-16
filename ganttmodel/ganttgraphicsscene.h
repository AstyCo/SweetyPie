#ifndef GANTTGRAPHICSSCENE_H
#define GANTTGRAPHICSSCENE_H

#include <QGraphicsScene>
//#include <QAbstractItemModel>
#include "ganttmodel.h"

#include "ganttgraphicsheader.h"
#include "ganttgraphicsitem.h"

class GanttGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GanttGraphicsScene(GanttModel * model, /*QDateTime begin, QDateTime end,*/ QObject *parent = 0);
    ~GanttGraphicsScene();

    void drawBackground(QPainter *painter, const QRectF &_rect);

    GanttGraphicsHeader * m_header;
    //GanttGraphicsItem * createGraphicsItem(GanttItem * item);

    void setInterval(QDateTime begin, QDateTime end);

signals:

public slots:

    void deleteGraphicsItem(QModelIndex index);
    void updateItems(GanttModel * model);

    //void setGrid(bool flag);
    //void setHeaderItem(QGraphicsItemGroup * header);

    void editAdd(QModelIndex index);
    void editDelete(QModelIndex index);
    //void editCut();
    //void editPaste();
    void editMoveUp(QModelIndex index);
    //void editMoveDown();
    //void editPromote();
    //void editDemote();

private slots:



private:
    GanttModel * m_model;
    QModelIndex * m_index;
    void setItems(GanttModel * model);

    QGraphicsLinearLayout * m_layout;
    QGraphicsLinearLayout * m_itemLayout;
    GanttGraphicsItem * m_item;

    QDateTime m_begin;
    QDateTime m_end;

    Scale m_scale;


};

#endif // GANTTGRAPHICSSCENE_H
