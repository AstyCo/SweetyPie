#include "gantttreeview.h"
#include "ganttheaderview.h"

#include "ganttglobalvalues.h"
#include "ganttgraphicsview.h"
#include "gantttreedelegate.h"


#include <QScrollBar>
#include <QMouseEvent>

#include <QDebug>

GanttTreeView::GanttTreeView(QWidget * parent )
{
    m_graphicsView = NULL;
    m_hSliderHeight = 15;

    m_header = new GanttHeaderView(Qt::Horizontal,this);
    setHeader(m_header);

    setMinimumWidth(GANTTTREEVIEW_MIN_WIDTH);
    horizontalScrollBar()->setStyleSheet(
                QString("QScrollBar {height:%1px;}").arg(m_hSliderHeight));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);

    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::NoSelection);
    setFrameStyle(0);



    setItemDelegateForColumn(1, new GanttTreeDelegate(this));
    setItemDelegateForColumn(2, new GanttTreeDelegate(this));

}

void GanttTreeView::scrollContentsBy(int dx, int dy)
{


    if(dx)
        repaintHeader();

    if(!m_graphicsView)
        return;

    int oldVal = verticalScrollBar()->value();
    int vs = verticalScrollBar()->value();
    m_graphicsView->verticalScrollBar()->setValue(vs);
    if(m_graphicsView->verticalScrollBar()->value() != vs)
        verticalScrollBar()->setValue(m_graphicsView->verticalScrollBar()->value());

    QList<QRectF> rects;
    rects.append(m_graphicsView->viewport()->rect());
    m_graphicsView->updateScene(rects);


    QTreeView::scrollContentsBy(dx,dy);
}

void GanttTreeView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();

    if( pos.y() > rect().bottom() - header()->height() - m_hSliderHeight)
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    else
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    QTreeView::mouseMoveEvent(event);
}

void GanttTreeView::leaveEvent(QEvent *e)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QTreeView::leaveEvent(e);
}


void GanttTreeView::setGraphicsView(GanttGraphicsView *graphicsView)
{

    m_graphicsView = graphicsView;
}


void GanttTreeView::repaintHeader()
{
    header()->reset();
}
