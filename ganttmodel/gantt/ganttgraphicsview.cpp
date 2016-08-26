#include "ganttgraphicsview.h"
#include "ganttwidget.h"
#include "ganttscene.h"
#include "gantttreeview.h"
#include "ganttinfonode.h"

#include <QResizeEvent>
#include <QScrollBar>

#include <QDebug>

GanttGraphicsView::GanttGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    initialize();
}


GanttGraphicsView::GanttGraphicsView(QGraphicsScene * scene, QWidget * parent) :
    QGraphicsView(scene,parent)
{
    initialize();
}


void GanttGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    if(!m_scene)
        return;

    emit viewResized(event->size());
}

void GanttGraphicsView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx,dy);

    if(!m_scene)
        return;

    if(dx)
        m_scene->invalidate(QRectF(),QGraphicsScene::BackgroundLayer);

    int vs = verticalScrollBar()->value();

    m_scene->updateHeaderPos(vs);

    if(!m_treeView)
        return;

    m_treeView->verticalScrollBar()->setValue(vs);

    m_treeView->update();


}

void GanttGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_scene)
        return;
    if(m_scene->headerMode() == GanttHeader::GanttDiagramMode)
    {
        QPoint pos = event->pos();

        if(rect().contains(pos) && pos.y() > rect().bottom() - m_hSliderHeight)
        {
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        }
        else
        {
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GanttGraphicsView::leaveEvent(QEvent *e)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsView::leaveEvent(e);
}



void GanttGraphicsView::initialize()
{
    m_scene = NULL;
    m_treeView = NULL;
    m_hSliderHeight = 0;

    setMinimumWidth(GANTTGRAPHICSVIEW_MIN_WIDTH);
    setFrameStyle(0);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setHSliderHeight(15);
}

void GanttGraphicsView::setHSliderHeight(int hSliderHeight)
{
    if(m_hSliderHeight == hSliderHeight)
        return;

    m_hSliderHeight = hSliderHeight;
    horizontalScrollBar()->setStyleSheet(
                QString("QScrollBar {height:%1px;}").arg(m_hSliderHeight));
}

void GanttGraphicsView::changeNodeExpanding()
{
    GanttInfoNode *p_node = qobject_cast<GanttInfoNode*>(sender());
    if(p_node)
    {
        changeExpanding(p_node->index());
    }
}


void GanttGraphicsView::setTreeView(GanttTreeView *treeView)
{
    m_treeView = treeView;
}

void GanttGraphicsView::changeExpanding(const QModelIndex &index)
{
    if(!m_treeView)
        return;

    if(m_treeView->model()->hasIndex(index.row(),index.column(),index.parent()))
        m_treeView->setExpanded(index, !(m_treeView->isExpanded(index)));
}

void GanttGraphicsView::setScene(GanttScene *scene)
{
    m_scene = scene;
    QGraphicsView::setScene(scene);

    scene->onViewAdded(this);
}




