#include "memoryplanningwidget.h"
#include "ui_memoryplanningwidget.h"

#include "mgrid_widget.h"
#include "memoryview.h"
#include "mgrid_scene.h"
#include "mline_scene.h"
#include "kamemorypart.h"
#include "kamemory.h"

#include <QDebug>
#include <QScrollBar>

using namespace Memory;

MemoryPlanningWidget::MemoryPlanningWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemoryPlanningWidget)
{
    ui->setupUi(this);

    m_scene = NULL;
}

MemoryPlanningWidget::~MemoryPlanningWidget()
{
    delete ui;
}

void MemoryPlanningWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->key()==Qt::Key_Space)
    {
        qDebug() << "ENTER";
        changeScene();
    }

    return QWidget::keyPressEvent(e);
}

void MemoryPlanningWidget::init(MemoryViewMode mode)
{
    clear();

    m_mode = mode;

    if(mode == MemoryGrid)
    {
        m_scene = new MGridScene(this);

        m_scene->setBackgroundBrush(QBrush(QColor(Qt::gray).lighter(130)));

        ui->memoryView->setScene(m_scene);

//        ui->memoryView->adjustSize();

//        ui->memoryView->setFrameStyle(0);
//        ui->memoryView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
//        ui->memoryView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//        ui->memoryView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//        ui->memoryView->setMaximumHeight(100000);

        return;
    }
    else if (mode == MemoryLine)
    {
        m_scene = new MLineScene(this);

        ui->memoryView->setScene(m_scene);

//        ui->memoryView->setFrameStyle(0);
//        ui->memoryView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
//        ui->memoryView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//        ui->memoryView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);



        return;
    }

    qDebug() << "MemoryPlanningWidget::init ignored";
    return;
}

void MemoryPlanningWidget::clear()
{
    if(m_scene)
        delete m_scene;
    m_scene = NULL;
    ui->memoryView->init();

}

void MemoryPlanningWidget::changeScene()
{
    ui->memoryView->changeScene();
    m_scene = ui->memoryView->scene();
}

void MemoryPlanningWidget::setMemory(const KaMemory &kaMemory)
{
    ui->memoryView->setMemory(kaMemory);
}
QGraphicsScene *MemoryPlanningWidget::scene() const
{
    return m_scene;
}

void MemoryPlanningWidget::adjustViewSize()
{
    ui->memoryView->adjustSize();
}

Memory::MemoryViewMode MemoryPlanningWidget::mode() const
{
    return m_mode;
}



