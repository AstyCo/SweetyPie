#ifndef GANTTVIEW_H
#define GANTTVIEW_H

#include <QSplitter>
#include <QToolBar>
#include <QTreeView>
#include "ganttgraphicsview.h"
#include "ganttgraphicsscene.h"
#include "QVBoxLayout"
#include "ganttproject_global.h"
#include "ganttplayer.h"

//enum Options
//{
//    All,
//    OnlyPlayer,
//    OnlyTree,
//    OnlyGraphics,
//    WithoutToolbar,
//    WithoutPlayer
//};

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

    QTreeView *treeview() const;
    void setTreeview(QTreeView *treeview);

    void ShowOnlyPlayer(bool flag);

    GanttPlayerControl *player() const;
    void setPlayer(GanttPlayerControl *player);

private:
    //QGraphicsScene * createScene(QAbstractItemModel * model); // тут будет происходить отрисовка сцены на основе данных модели
                                                            // хотя решил передавать указатель на модель в сам конструктор сцены

    QTreeView      * m_treeview;
    GanttGraphicsView  * m_graphicsview;
    GanttGraphicsScene * m_graphicsscene;
    GanttModel         * m_model;
    QSplitter          * m_splitter;
    GanttPlayerControl        * m_player;


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
