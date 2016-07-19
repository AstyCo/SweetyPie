#include "kamemoryview.h"
#include "QDebug"

#include "kamemorypart.h"
#include "QResizeEvent"

KaMemoryView::KaMemoryView(QWidget *parent) :
    QGraphicsView(parent)
{
    setMinimumSize(100,126);
    setMaximumSize(16777215,126);
    setAlignment(Qt::AlignLeft | Qt::AlignTop); // устанавливает начало координат сцены в левый верхний угол
    setTransformationAnchor(QGraphicsView::NoAnchor); // не изменяет сцену при изменении размера представления
//    //===============
    viewport()->installEventFilter(this);
}

KaMemoryView::~KaMemoryView()
{

}

void KaMemoryView::setScene(KaMemoryScene *scene)
{
    QGraphicsView::setScene((QGraphicsScene*)scene);
    scene->setSceneRect(0,0,size().width(),126);
}

bool KaMemoryView::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::Resize)
    {
        QResizeEvent *res = reinterpret_cast<QResizeEvent*>(event);
        if(scene()!=0)
        {
            scene()->setSceneRect(0,0,res->size().width(),126);
        }
        return true;
    }
    return QGraphicsView::eventFilter(object, event);
}
