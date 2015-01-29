#ifndef GANTTVIEW_H
#define GANTTVIEW_H

#include <QSplitter>
#include "gantttreeview.h"
#include "ganttgraphicsview.h"
#include "ganttgraphicsscene.h"

class GanttView : public QSplitter
{
    Q_OBJECT
public:
    explicit GanttView(QWidget *parent = 0);

    void setModel(GanttModel * model);


    GanttGraphicsScene *graphicsscene() const;
    void setGraphicsscene(GanttGraphicsScene *graphicsscene);

private:
    //QGraphicsScene * createScene(QAbstractItemModel * model); // тут будет происходить отрисовка сцены на основе данных модели
                                                            // хотя решил передавать указатель на модель в сам конструктор сцены

    GanttTreeView * m_treeview;
    GanttGraphicsView * m_graphicsview;
    GanttGraphicsScene * m_graphicsscene;

signals:

public slots:

private slots:
    void holdHeaderOnTop();

};

#endif // GANTTVIEW_H
