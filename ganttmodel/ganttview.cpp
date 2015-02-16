#include "ganttview.h"
#include <QDebug>
#include "QScrollBar"


//#include "alt_key.hpp"
//#include "aqp.hpp"
//#include "global.hpp"
//#include "mainwindow.hpp"

//#include "richtextdelegate.hpp"

#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QHash>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#if QT_VERSION >= 0x040600
#include <QScopedPointer>
#else
#include <QSharedPointer>
#endif
#include <QStatusBar>
#include <QStandardItem>
#include <QTimer>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>


namespace {

//const int StatusTimeout = AQP::MSecPerSecond * 10;
const QString FilenameSetting("Filename");
const QString GeometrySetting("Geometry");
const QString CurrentTaskPathSetting("CurrentTaskPath");
const int FirstFrame = 0;
const int LastFrame = 4;


QAction *createAction(const QString &icon, const QString &text,
        QObject *parent,
        const QKeySequence &shortcut=QKeySequence())
{
    QAction *action = new QAction(QIcon(icon), text, parent);
    if (!shortcut.isEmpty())
        action->setShortcut(shortcut);
    return action;
}

} // anonymous namespace



GanttView::GanttView(QWidget *parent) :
    QWidget(parent)
{

    Q_INIT_RESOURCE(images);

    m_splitter = new QSplitter(this);

    m_treeview = new GanttTreeView();
    m_graphicsview = new GanttGraphicsView();

    //QTime midnight(11,8,43);
    //qsrand(midnight.secsTo(QTime::currentTime()));



    m_splitter->addWidget(m_treeview);
    m_splitter->addWidget(m_graphicsview);

    m_splitter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    //m_graphicsview->horizontalScrollBar()->setMinimum(5);
    connect(m_graphicsview->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(holdHeaderOnTop()));
    //holdHeaderOnTop();

    //======timelog2==========
    //createModelAndView();
    createActions();
    createMenusAndToolBar();
    createConnections();

    //AQP::accelerateMenu(menuBar());

    //====================

    m_treeview->setDragDropMode(QAbstractItemView::InternalMove);

    m_treeview->setAllColumnsShowFocus(true);
    //m_treeview->setItemDelegateForColumn(1, new QDateTimeEdit);

}

//====================================================
void GanttView::setModel(GanttModel *model)
{
    m_treeview->setModel(model);
    m_model = model;
    m_graphicsscene = new GanttGraphicsScene(model);
    m_graphicsview->setScene(m_graphicsscene);
    //connect(this, SIGNAL(treeSignal(QModelIndex)),m_graphicsscene,SLOT(deleteGraphicsItem(QModelIndex)));
    //connect(this, SIGNAL(treeSignal(GanttModel*)),m_graphicsscene,SLOT(updateItems(GanttModel*)));
}

GanttGraphicsScene *GanttView::graphicsscene() const
{
    return m_graphicsscene;
}

void GanttView::setGraphicsscene(GanttGraphicsScene *graphicsscene)
{
    m_graphicsscene = graphicsscene;
}


void GanttView::holdHeaderOnTop()
{
    //int maxY = _list.list().count()*60+60;
    if(m_graphicsview->verticalScrollBar()->isVisible())
        m_graphicsscene->m_header->setPos(0,m_graphicsview->verticalScrollBar()->value()-
                          m_graphicsview->verticalScrollBar()->minimum());
    else
        m_graphicsscene->m_header->setPos(0, 0);

    m_graphicsscene->update();
}

//====================================================
/*void GanttView::createModelAndView()
{
    m_treeview = new QTreeView;

    m_model = new GanttModel(this);
    m_treeview->setDragDropMode(QAbstractItemView::InternalMove);

    m_treeview->setAllColumnsShowFocus(true);
    m_treeview->setItemDelegateForColumn(0, new RichTextDelegate);
    m_treeview->setModel(m_model);
    setCentralWidget(m_treeview);
}*/

void GanttView::createActions()
{
   /* fileNewAction = createAction(":/filenew.png", tr("New"),
            this, QKeySequence::New);
    fileOpenAction = createAction(":/fileopen.png", tr("Open..."),
            this, QKeySequence::Open);
    fileSaveAction = createAction(":/filesave.png", tr("Save"),
            this, QKeySequence::Save);
    fileSaveAsAction = createAction(":/filesave.png",
            tr("Save As..."), this
#if QT_VERSION >= 0x040500
            , QKeySequence::SaveAs
#endif
            );
    fileQuitAction = createAction(":/filequit.png", tr("Quit"), this);
#if QT_VERSION >= 0x040600
    fileQuitAction->setShortcuts(QKeySequence::Quit);
#else
    fileQuitAction->setShortcut(QKeySequence("Ctrl+Q"));
#endif*/
    editAddAction = createAction(":/images/editadd.png", tr("Add..."),
            this, QKeySequence(tr("Ctrl+A")));
    editDeleteAction = createAction(":/images/editdelete.png",
            tr("Delete..."), this, QKeySequence::Delete);

    editCutAction = createAction(":/images/editcut.png", tr("Cut"),
            this, QKeySequence::Cut);
    editPasteAction = createAction(":/images/editpaste.png", tr("Paste"),
            this, QKeySequence::Paste);
    editMoveUpAction = createAction(":/images/editup.png", tr("Move Up"),
            this, QKeySequence(tr("Ctrl+Up")));
    editMoveDownAction = createAction(":/images/editdown.png",
            tr("Move Down"), this, QKeySequence(tr("Ctrl+Down")));
    editPromoteAction = createAction(":/images/editpromote.png",
            tr("Promote"), this, QKeySequence(tr("Ctrl+Left")));
    editDemoteAction = createAction(":/images/editdemote.png",
            tr("Demote"), this, QKeySequence(tr("Ctrl+Right")));
}

void GanttView::createMenusAndToolBar()
{
    //setUnifiedTitleAndToolBarOnMac(true);

    //setContentsMargins(0,0,0,40);
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setSpacing(0);
    vlayout->setMargin(0);


//    QMenu *fileMenu = menuBar()->addMenu(tr("File"));
//    QToolBar *fileToolBar = addToolBar(tr("File"));

//    foreach (QAction *action, QList<QAction*>() << fileNewAction
//            << fileOpenAction << fileSaveAction << fileSaveAsAction) {
//        fileMenu->addAction(action);
//        if (action != fileSaveAsAction)
//            fileToolBar->addAction(action);
//        if (action == fileSaveAction || action == fileSaveAsAction)
//            action->setEnabled(false);
//    }
//    fileMenu->addSeparator();
//    fileMenu->addAction(fileQuitAction);

    QAction *emptyAction = 0;
    //QMenu *editMenu = menuBar()->addMenu(tr("Edit"));
    QToolBar *editToolBar = new QToolBar(tr("Edit"));//addToolBar(tr("Edit"));


    foreach (QAction *action, QList<QAction*>() << editAddAction
            << editDeleteAction << emptyAction
            << editCutAction << editPasteAction << emptyAction
            << editMoveUpAction << editMoveDownAction
            << editPromoteAction << editDemoteAction)

    {
        if (action == emptyAction) {
            //editMenu->addSeparator();
            editToolBar->addSeparator();
            continue;
        }
//        if (action != editHideOrShowDoneTasksAction)
            editToolBar->addAction(action);
//        else
//            editMenu->addSeparator();
//        editMenu->addAction(action);
    }

    vlayout->addWidget(m_splitter, 0);
    vlayout->addWidget(editToolBar, 1);
    setLayout(vlayout);


}

void GanttView::createConnections()
{
//    connect(m_treeview->selectionModel(),
//            SIGNAL(currentChanged(const QModelIndex&,
//                                  const QModelIndex&)),
//            this, SLOT(updateUi()));

//    connect(m_model,
//        SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
//        this, SLOT(setDirty()));
//    connect(m_model, SIGNAL(stopTiming()), this, SLOT(stopTiming()));

//    connect(m_model, SIGNAL(rowsRemoved(const QModelIndex&,int,int)),
//            this, SLOT(setDirty()));
//    connect(m_model, SIGNAL(modelReset()), this, SLOT(setDirty()));

    QHash<QAction*, QString> slotForAction;

    slotForAction[editAddAction] = SLOT(editAdd());
    slotForAction[editDeleteAction] = SLOT(editDelete());

    slotForAction[editCutAction] = SLOT(editCut());
    slotForAction[editPasteAction] = SLOT(editPaste());
    slotForAction[editMoveUpAction] = SLOT(editMoveUp());
    slotForAction[editMoveDownAction] = SLOT(editMoveDown());
    slotForAction[editPromoteAction] = SLOT(editPromote());
    slotForAction[editDemoteAction] = SLOT(editDemote());

    QHashIterator<QAction*, QString> i(slotForAction);
    while (i.hasNext()) {
        i.next();
        connect(i.key(), SIGNAL(triggered()),
                this, qPrintable(i.value()));
    }

//    connect(editStartOrStopAction, SIGNAL(triggered(bool)),
//            this, SLOT(editStartOrStop(bool)));
//    connect(editHideOrShowDoneTasksAction, SIGNAL(triggered(bool)),
//            this, SLOT(editHideOrShowDoneTasks(bool)));
//    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
//    connect(&iconTimeLine, SIGNAL(frameChanged(int)),
//            this, SLOT(updateIcon(int)));

    m_treeview->verticalScrollBar()->setStyleSheet("QScrollBar {width:0px;}");
    //m_treeview->verticalScrollBar()->hide();//setVisible(false);
    connect(m_graphicsview->verticalScrollBar(),SIGNAL(valueChanged(int)),m_treeview->verticalScrollBar(),SLOT(setValue(int)));


}

void GanttView::updateUi()
{
//    fileSaveAction->setEnabled(isWindowModified());
//    int rows = m_model->rowCount();
//    fileSaveAsAction->setEnabled(isWindowModified() || rows);
//    editHideOrShowDoneTasksAction->setEnabled(rows);
    bool enable = m_treeview->currentIndex().isValid();

    foreach (QAction *action, QList<QAction*>() << editDeleteAction
            << editMoveUpAction << editMoveDownAction << editCutAction
            << editPromoteAction << editDemoteAction)

        action->setEnabled(enable);

   // editStartOrStopAction->setEnabled(rows);
    editPasteAction->setEnabled(m_model->hasCutItem());

}

void GanttView::updateScene()
{
    m_graphicsscene = new GanttGraphicsScene(m_model);
    m_graphicsview->setScene(m_graphicsscene);
}

void GanttView::setCurrentIndex(const QModelIndex &index)
{
    if (index.isValid()) {
        m_treeview->scrollTo(index);
        m_treeview->setCurrentIndex(index);
    }
}

void GanttView::editAdd()
{
    QModelIndex index = m_treeview->currentIndex();
    if (m_model->insertRow(0, index)) {
        index = m_model->index(0, 0, index);
        setCurrentIndex(index);
        m_treeview->edit(index);
        //setDirty();
        updateUi();
    }

    m_graphicsscene->editAdd(index);

   //emit treeSignal(m_model);
}

void GanttView::editDelete()
{
    QModelIndex index = m_treeview->currentIndex();
    if (!index.isValid())
        return;

    QString name = m_model->data(index).toString();
    int rows = m_model->rowCount(index);
//    if (m_model->isTimedItem(index))
//        stopTiming();

    QString message;
    if (rows == 0)
        message = tr("<p>Delete '%1'").arg(name);
    else if (rows == 1)
        message = tr("<p>Delete '%1' and its child (and "
                     "grandchildren etc.)").arg(name);
    else if (rows > 1)
        message = tr("<p>Delete '%1' and its %2 children (and "
                     "grandchildren etc.)").arg(name).arg(rows);
//    if (!AQP::okToDelete(this, tr("Delete"), message))
//        return;
    m_model->removeRow(index.row(), index.parent());
    //setDirty();
    updateUi();



    m_graphicsscene->editDelete(index);
    //emit treeSignal(m_model);
}

void GanttView::editCut()
{
    QModelIndex index = m_treeview->currentIndex();
//    if (m_model->isTimedItem(index))
//        stopTiming();
    setCurrentIndex(m_model->cut(index));
    editPasteAction->setEnabled(m_model->hasCutItem());
}


void GanttView::editPaste()
{
    setCurrentIndex(m_model->paste(m_treeview->currentIndex()));
//    editHideOrShowDoneTasks(
//            editHideOrShowDoneTasksAction->isChecked());
}


void GanttView::editMoveUp()
{
    QModelIndex index = m_model->moveUp(m_treeview->currentIndex());
    m_treeview->setCurrentIndex(index);
//    editHideOrShowDoneTasks(
//            editHideOrShowDoneTasksAction->isChecked());

    m_graphicsscene->editMoveUp(index);
}


void GanttView::editMoveDown()
{
    m_treeview->setCurrentIndex(
            m_model->moveDown(m_treeview->currentIndex()));
//    editHideOrShowDoneTasks(
//            editHideOrShowDoneTasksAction->isChecked());
}


void GanttView::editPromote()
{
    QModelIndex index = m_treeview->currentIndex();
//    if (m_model->isTimedItem(index))
//        stopTiming();
    setCurrentIndex(m_model->promote(index));
//    editHideOrShowDoneTasks(
//            editHideOrShowDoneTasksAction->isChecked());
}


void GanttView::editDemote()
{
    QModelIndex index = m_treeview->currentIndex();
//    if (m_model->isTimedItem(index))
//        stopTiming();
    m_treeview->setCurrentIndex(m_model->demote(index));
//    editHideOrShowDoneTasks(
//            editHideOrShowDoneTasksAction->isChecked());
}

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

//====================================================

