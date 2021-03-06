/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию GanttGraphicsView class.
 * \~englist
 * \brief File contains realization of GanttGraphicsView class.
 */
#include "ganttgraphicsview.h"
#include "ganttscene.h"
#include "gantt/info/ganttinfoitem.h"

#include "gantt/private_extensions/gantt-lib_global_values.h"

#include <QApplication>
#include <QResizeEvent>
#include <QScrollBar>

#include <QDebug>

GanttGraphicsView::GanttGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    initialize();
}


GanttGraphicsView::GanttGraphicsView(HFitScene *scene, QWidget * parent) :
    QGraphicsView(scene,parent)
{
    initialize();

//    connect(scene,SIGNAL(sceneRectAboutToBeChanged(QRectF)), this, SLOT(updateMaximumHeight()));
}

void GanttGraphicsView::setScene(HFitScene *scene)
{
    QGraphicsView::setScene(scene);

//    connect(scene,SIGNAL(sceneRectAboutToBeChanged(QRectF)), this, SLOT(updateMaximumHeight()));
}


void GanttGraphicsView::resizeEvent(QResizeEvent *event)
{
//    qDebug() << "GanttGraphicsView::resizeEvent " << event->size() << event->oldSize() << scene()->height();
    if(scene() && scene()->height() < event->size().height()){


        QResizeEvent rszEvent(QSize(event->size().width(), scene()->height()),
                                                      event->oldSize());
        resizeEvent(&rszEvent);
        event->accept();
        return;
    }
    QGraphicsView::resizeEvent(event);

    emit viewResized(event->size());
}

//void GanttGraphicsView::scrollContentsBy(int dx, int dy)
//{
//    QGraphicsView::scrollContentsBy(dx,dy);

//    if(!m_scene)
//        return;

//    if(dx)
//        m_scene->invalidate(QRectF(),QGraphicsScene::BackgroundLayer);

//    int vs = verticalScrollBar()->value();

//    m_scene->updateHeaderPos(vs);

//    if(!m_treeView)
//        return;

//    m_treeView->verticalScrollBar()->setValue(vs);

//    m_treeView->update();


//}

void GanttGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    int dy = 0;

    if( !_lastPos.isNull()
        && _mousePressH->isSlide()
        && !_mousePressH->isHorSlide())
    {
        int val = verticalScrollBar()->value(),
            min = verticalScrollBar()->minimum(),
            max = verticalScrollBar()->maximum();

        dy  = event->pos().y() - _lastPos.y();
        if(val - dy > max)
            dy = val - max;
        if(val - dy < min)
            dy = val - min;

        verticalScrollBar()->setValue(val - dy);
        if(scene())
            scene()->update();
        _lastPos = QPoint(event->pos().x(), _lastPos.y() + dy);
    }
    else
        _lastPos = event->pos();

    QGraphicsView::mouseMoveEvent(event);
}

void GanttGraphicsView::leaveEvent(QEvent *e)
{
    _lastPos = QPoint();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsView::leaveEvent(e);
}

void GanttGraphicsView::updateMaximumHeight()
{
    if(scene() && maximumHeight() != scene()->height()){
        setMaximumHeight(scene()->height());
        emit maximumHeightChanged();
    }
}




void GanttGraphicsView::initialize()
{
    m_hSliderHeight = 0;

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMinimumWidth(GANTTGRAPHICSVIEW_MIN_WIDTH);
    setFrameStyle(0);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setHSliderHeight(15);

    setContextMenuPolicy(Qt::CustomContextMenu);
    setMouseTracking(true);
}

void GanttGraphicsView::setHSliderHeight(int hSliderHeight)
{
    if(m_hSliderHeight == hSliderHeight)
        return;

    m_hSliderHeight = hSliderHeight;
    horizontalScrollBar()->setStyleSheet(
                QString("QScrollBar {height:%1px;}").arg(m_hSliderHeight));
}

void GanttGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MiddleButton){
        _lastCursor = cursor();
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

void GanttGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MiddleButton){
        setCursor(_lastCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}


//void GanttGraphicsView::wheelEvent(QWheelEvent *event)
//{
//    if(event->buttons() & Qt::MiddleButton){
//        setCursor();
//    }
//}



