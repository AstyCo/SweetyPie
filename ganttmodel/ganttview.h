#ifndef GANTTVIEW_H
#define GANTTVIEW_H

#include <QSplitter>
#include <QToolBar>
#include "gantttreeview.h"
#include "ganttgraphicsview.h"
#include "ganttgraphicsscene.h"
#include "QVBoxLayout"
#include "ganttmodel_global.h"
#include "ganttplayer.h"

class GANTTMODELSHARED_EXPORT GanttView : public QWidget//QSplitter
{
    Q_OBJECT
public:
    explicit GanttView(QWidget *parent = 0);
    ~GanttView();

    //void setModel(GanttModel * model);


    GanttGraphicsScene *graphicsscene() const;
    void setGraphicsscene(GanttGraphicsScene *graphicsscene);
    void setEditable(bool flag);

    GanttModel *model() const;
    void setModel(GanttModel *model);

    GanttTreeView *treeview() const;
    void setTreeview(GanttTreeView *treeview);

private:
    //QGraphicsScene * createScene(QAbstractItemModel * model); // тут будет происходить отрисовка сцены на основе данных модели
                                                            // хотя решил передавать указатель на модель в сам конструктор сцены

    GanttTreeView      * m_treeview;
    GanttGraphicsView  * m_graphicsview;
    GanttGraphicsScene * m_graphicsscene;
    GanttModel         * m_model;
    QSplitter          * m_splitter;
    GanttPlayer * m_player;


    QToolBar * m_editToolBar;

    //====timelog2==========
    void createModelAndView();
    void createActions();
    void createMenusAndToolBar();
    void createConnections();
    void setCurrentIndex(const QModelIndex &index);

    QAction *editAddAction;
    QAction *editDeleteAction;
    QAction *editCutAction;
    QAction *editPasteAction;
    QAction *editMoveUpAction;
    QAction *editMoveDownAction;
    QAction *editPromoteAction;
    QAction *editDemoteAction;
    //==================

signals:
    //void mySignal();
    void treeSignal(GanttModel*);


public slots:

private slots:
    void holdHeaderOnTop();

    //======timelog2=========
    void editAdd();
    void editDelete();
    void editCut();
    void editPaste();
    void editMoveUp();
    void editMoveDown();
    void editPromote();
    void editDemote();
    void updateUi();
    //======================

    void updateScene();

};

#endif // GANTTVIEW_H
