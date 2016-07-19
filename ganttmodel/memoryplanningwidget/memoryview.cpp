#include "memoryview.hpp"
#include "memoryscene.hpp"
#include "kamemoryscene.h"

#include <QDebug>
#include <QResizeEvent>
#include <QtCore>

#include <QScrollBar>

MemoryView::MemoryView(QWidget *parent) :
    QGraphicsView(parent)
{
}

MemoryView::MemoryView(QGraphicsScene * scene, QWidget * parent) :
    QGraphicsView(parent)
{
    setScene(scene);
}

void MemoryView::resizeEvent(QResizeEvent *event)
{
    MemoryScene* p_memoryScene = dynamic_cast<MemoryScene*>(scene());
    if(p_memoryScene)
    {
        p_memoryScene->viewResized(event->size());
        if(isTransformed())
            p_memoryScene->transformChanged(transform());

        qDebug()<< event->size();
        qDebug()<< sceneRect();

        verticalScrollBar()->setValue(0);

        return QGraphicsView::resizeEvent(event);
    }
    // ELSE

    KaMemoryScene* p_kaMemoryScene = dynamic_cast<KaMemoryScene*>(scene());
    if(p_kaMemoryScene)
    {
        p_kaMemoryScene->setSceneRect(0,0,event->size().width(),126);

//        update();
        repaint();
        return;
    }
    // ELSE
    return QGraphicsView::resizeEvent(event);
}

void MemoryView::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);

    MemoryScene* p_memoryScene = dynamic_cast<MemoryScene*>(scene);
    if(p_memoryScene)
    {
        setContentsMargins(0, 0, 0, 0);
        setRenderHint(QPainter::Antialiasing,true);

        p_memoryScene->setParent(this);
        return;
    }

    KaMemoryScene* p_kaMemoryScene = dynamic_cast<KaMemoryScene*>(scene);
    if(p_kaMemoryScene)
    {
        setMinimumSize(100,126);
        setMaximumSize(16777215,126);
        setAlignment(Qt::AlignLeft | Qt::AlignTop); // устанавливает начало координат сцены в левый верхний угол
        setTransformationAnchor(QGraphicsView::NoAnchor); // не изменяет сцену при изменении размера представления
    //    //===============
        viewport()->installEventFilter(this);

        scene->setSceneRect(0,0,size().width(),126);
        return;
    }


}



