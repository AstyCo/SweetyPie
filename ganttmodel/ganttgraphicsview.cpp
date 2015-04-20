#include "ganttgraphicsview.h"

#include "ganttgraphicsscene.h"

#include "QDebug"
#include <QVBoxLayout>
#include <QSlider>

#include <qmath.h>

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

    setViewportMargins(30,0,0,0);
    QHBoxLayout * hLayout = new QHBoxLayout;
    hLayout->setSpacing(0);
    hLayout->setMargin(0);

    QVBoxLayout * zoomLayout = new QVBoxLayout;
    zoomLayout->setSpacing(0);
    zoomLayout->setMargin(0);

    zoomSlider = new QSlider;
//    zoomSlider->setMinimum(0);
//    zoomSlider->setMaximum(1000);
//    zoomSlider->setValue(250);

    zoomSlider->setMinimum(2);
    zoomSlider->setMaximum(5);
    zoomSlider->setValue(5);
    zoomSlider->setSingleStep(1);
    zoomSlider->setTickInterval(1);
    zoomSlider->setTickPosition(QSlider::TicksRight);

    QWidget* fake = new QWidget();
    fake->setBackgroundRole(QPalette::Window);
    fake->setFixedSize(RULER_BREADTH,RULER_BREADTH);

    zoomLayout->addWidget(zoomSlider);
    zoomLayout->addWidget(fake);
    hLayout->addLayout(zoomLayout);
    hLayout->addWidget(this->viewport());

    this->setLayout(hLayout);


    connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));

}

void GanttGraphicsView::setupMatrix()
{
//    qreal scale = qPow(qreal(2), (zoomSlider->value() - 250) / qreal(50));
//    QMatrix matrix;
//    matrix.scale(scale, 1);
//    setMatrix(matrix);

    GanttGraphicsScene * myScene = (GanttGraphicsScene*)this->scene();
    switch (zoomSlider->value()) {
    case 1:
        myScene->m_header->setZoom(ScaleSecond);
        break;
    case 2:
        myScene->m_header->setZoom(ScaleMinute);
        break;
    case 3:
        myScene->m_header->setZoom(ScaleHour);
        break;
    case 4:
        myScene->m_header->setZoom(ScaleDay);
        break;
    case 5:
        myScene->m_header->setZoom(ScaleMonth);
        break;
    default:
        break;
    }
    myScene->m_header->createHeader();
    myScene->update();

}

