#include "memoryview.h"
#include "mgrid_scene.h"
#include "mline_scene.h"

#include "memory.h"

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

    if(m_mode == MemoryGrid)
    {
        m_gridScene->viewResized(event->size());
        verticalScrollBar()->setValue(0);

        return QGraphicsView::resizeEvent(event);
    }
    // ELSE
    if(m_mode == MemoryLine)
    {
        m_lineScene->setSceneRect(0,0,event->size().width(),126);

        repaint();
        return;
    }
    // ELSE
    return QGraphicsView::resizeEvent(event);
}

void MemoryView::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);

    MGridScene* p_memoryScene = dynamic_cast<MGridScene*>(scene);
    if(p_memoryScene)
    {
        m_mode = MemoryGrid;
        m_gridScene = p_memoryScene;

        setContentsMargins(0, 0, 0, 0);
        setRenderHint(QPainter::Antialiasing,true);

        p_memoryScene->setParent(this);

//        p_memoryScene->setBackgroundBrush(QBrush(QColor(Qt::gray).lighter(130)));

        setFrameStyle(0);
        setAlignment(Qt::AlignLeft | Qt::AlignTop);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setMaximumHeight(100000);

        return;
    }

    MLineScene* p_lineScene = dynamic_cast<MLineScene*>(scene);
    if(p_lineScene)
    {
        m_mode = MemoryLine;

//        if(p_lineScene == m_lineScene)
//            return;

        m_lineScene = p_lineScene;

        setContentsMargins(0, 0, 0, 0);
        setRenderHint(QPainter::Antialiasing,false);

        setMinimumSize(100,126);
        setMaximumSize(16777215,126);
        setAlignment(Qt::AlignLeft | Qt::AlignTop); // устанавливает начало координат сцены в левый верхний угол
        setTransformationAnchor(QGraphicsView::NoAnchor); // не изменяет сцену при изменении размера представления
    //    //===============
        viewport()->installEventFilter(this);

        p_lineScene->setSceneRect(0,0,size().width(),126);

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
    Memory kaMemory;
    if(m_mode == MemoryGrid)
    {
        m_mode = MemoryLine;
        if(!m_lineScene)
        {
            setScene(new MLineScene(parent()));
            kaMemory = m_gridScene->memory();
            setMemory(kaMemory);
        }
        else
            setScene(m_lineScene);
    }
    else if(m_mode == MemoryLine)
    {
        m_mode = MemoryGrid;
        if(!m_gridScene)
        {
            setScene(new MGridScene(parent()));
            kaMemory = m_lineScene->memory();
            setMemory(kaMemory);
        }
        else
            setScene(m_gridScene);
    }
}

void MemoryView::setMemory(const Memory &kaMemory)
{
    if(m_mode == MemoryGrid)
        m_gridScene->setMemory(kaMemory);
    else if(m_mode == MemoryLine)
        m_lineScene->setMemory(kaMemory);

}
MemoryView::MemoryViewMode MemoryView::mode() const
{
    return m_mode;
}

void MemoryView::setMode(const MemoryView::MemoryViewMode &mode)
{
    m_mode = mode;
}

void MemoryView::leaveEvent(QEvent *)
{
    if(m_mode == MemoryGrid)
    {
        m_gridScene->clearMouseOver();
        m_gridScene->update(viewport()->rect());
    }
    else if (m_mode == MemoryLine)
    {

    }
}





QColor MemoryState_to_QColor(MemoryPart::MemoryState state, bool isActive)
{
    QColor result;
    //    if(isActive)
    switch (state) {
    case MemoryPart::Empty:
        result = QColor(Qt::white);
        break;
    case MemoryPart::Free:
        result = QColor::fromRgb(0xCCD2D4);
        break;
    case MemoryPart::Recorded:
        result = QColor::fromRgb(0xE3E187);
        break;
    case MemoryPart::PendingWrite:
        result = QColor::fromRgb(0x73E884);
        break;
    case MemoryPart::PendingRead:
        result = QColor::fromRgb(0x99DDE3);
        break;
    case MemoryPart::ErrorWrite:
        result = QColor::fromRgb(0xE39753);
        break;
    case MemoryPart::ErrorRead:
        result = QColor::fromRgb(0x308C9E);
        break;
    default:
        result = QColor(Qt::black);
        break;
    }

    if(!isActive && state != MemoryPart::Free)
        return result.lighter(130);
    return result;
}
