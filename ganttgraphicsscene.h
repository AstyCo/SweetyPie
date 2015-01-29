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

    //void setGrid(bool flag);
    //void setHeaderItem(QGraphicsItemGroup * header);

private:
    GanttModel * m_model;
    QModelIndex * m_index;
    void setItems(GanttModel * model);

    QGraphicsLinearLayout * m_layout;
    GanttGraphicsItem * m_item;

    QDateTime m_begin;
    QDateTime m_end;


};

#endif // GANTTGRAPHICSSCENE_H
