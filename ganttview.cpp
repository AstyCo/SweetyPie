#include "ganttview.h"
#include <QDebug>
#include "QScrollBar"

GanttView::GanttView(QWidget *parent) :
    QSplitter(parent)
{
    m_treeview = new GanttTreeView();
    m_graphicsview = new GanttGraphicsView();
    addWidget(m_treeview);
    addWidget(m_graphicsview);

    //m_graphicsview->horizontalScrollBar()->setMinimum(5);
    //connect(m_graphicsview->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(holdHeaderOnTop()));
    //holdHeaderOnTop();

}

//====================================================
void GanttView::setModel(GanttModel *model)
{
    m_treeview->setModel(model);
    m_graphicsscene = new GanttGraphicsScene(model);
    m_graphicsview->setScene(m_graphicsscene);
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

