#include "memoryview.h"
#include "mgrid_scene.h"
#include "mline_scene.h"

#include "kamemory.h"

#include <QDebug>
#include <QResizeEvent>
#include <QtCore>

#include <QScrollBar>



MemoryView::MemoryView(QWidget *parent) :
    QGraphicsView(parent)
{
    init();
}

MemoryView::MemoryView(QGraphicsScene * scene, QWidget * parent) :
    QGraphicsView(parent)
{
    init();
    setScene(scene);
}

void MemoryView::resizeEvent(QResizeEvent *event)
{
    if(m_gridScene)
    {
        m_gridScene->viewResized(event->size());
        if(isTransformed())
            m_gridScene->transformChanged(transform());

        qDebug()<< event->size();
        qDebug()<< sceneRect();

        verticalScrollBar()->setValue(0);

        return QGraphicsView::resizeEvent(event);
    }
    // ELSE
    if(m_lineScene)
    {
        m_lineScene->setSceneRect(0,0,event->size().width(),126);

//        update();
        repaint();
        return;
    }
    // ELSE
    return QGraphicsView::resizeEvent(event);
}

void MemoryView::setScene(QGraphicsScene *scene)
{
    init();
    QGraphicsView::setScene(scene);

    MGridScene* p_memoryScene = dynamic_cast<MGridScene*>(scene);
    if(p_memoryScene)
    {
        m_gridScene = p_memoryScene;

        setContentsMargins(0, 0, 0, 0);
        setRenderHint(QPainter::Antialiasing,true);

        p_memoryScene->setParent(this);

        p_memoryScene->setBackgroundBrush(QBrush(QColor(Qt::gray).lighter(130)));

//        setScene(p_memoryScene);

        adjustSize();

        setFrameStyle(0);
        setAlignment(Qt::AlignLeft | Qt::AlignTop);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setMaximumHeight(100000);

        return;
    }

    MLineScene* p_kaMemoryScene = dynamic_cast<MLineScene*>(scene);
    if(p_kaMemoryScene)
    {
        m_lineScene = p_kaMemoryScene;

        setContentsMargins(0, 0, 0, 0);
        setRenderHint(QPainter::Antialiasing,false);

        setMinimumSize(100,126);
        setMaximumSize(16777215,126);
        setAlignment(Qt::AlignLeft | Qt::AlignTop); // устанавливает начало координат сцены в левый верхний угол
        setTransformationAnchor(QGraphicsView::NoAnchor); // не изменяет сцену при изменении размера представления
    //    //===============
        viewport()->installEventFilter(this);

        p_kaMemoryScene->setSceneRect(0,0,size().width(),126);

//        setScene(p_kaMemoryScene);

        setFrameStyle(0);
        setAlignment(Qt::AlignLeft | Qt::AlignTop);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        return;
    }


}

void MemoryView::init()
{
    m_gridScene = NULL;
    m_lineScene = NULL;
}

void MemoryView::changeScene()
{
    KaMemory kaMemory;
    if(m_gridScene)
    {
        kaMemory = m_gridScene->memory();
        m_lineScene = new MLineScene(parent());
        setScene(m_lineScene);
    }
    else if(m_lineScene)
    {
        kaMemory = m_lineScene->memory();
        m_gridScene = new MGridScene(parent());
        setScene(m_gridScene);
    }
    setMemory(kaMemory);
    adjustSize();
}

void MemoryView::setMemory(const KaMemory &kaMemory)
{
    if(m_gridScene)
        m_gridScene->setMemory(kaMemory);
    else if(m_lineScene)
        m_lineScene->setMemory(kaMemory);

}



