#include "ganttgraphicsview.h"

#include "QDebug"
#include <QVBoxLayout>

GanttGraphicsView::GanttGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
//    setViewportMargins(RULER_BREADTH,0,0,0);
//    QHBoxLayout* gridLayout = new QHBoxLayout();
//    gridLayout->setSpacing(0);
//    gridLayout->setMargin(0);

//    //mHorzRuler = new QDRuler(QDRuler::Horizontal, this);
//    mVertRuler = new QDRuler(QDRuler::Vertical, this);

//    //gridLayout->addWidget(mHorzRuler,0);
//    gridLayout->addWidget(mVertRuler,0);
//    gridLayout->addWidget(this->viewport(),1);

//    this->setLayout(gridLayout);


    this->setAlignment(Qt::AlignLeft | Qt::AlignTop); // устанавливает начало координат сцены в левый верхний угол
    this->setTransformationAnchor(QGraphicsView::NoAnchor); // не изменяет сцену при изменении размера представления
}

