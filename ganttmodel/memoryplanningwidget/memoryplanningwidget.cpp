#include "memoryplanningwidget.h"
#include "ui_memoryplanningwidget.h"

#include "mgrid_widget.h"
#include "memoryview.h"
#include "mgrid_scene.h"
#include "mline_scene.h"
#include "kamemorypart.h"
#include "kamemory.h"
#include "mgrid_status.h"

#include <QDebug>
#include <QScrollBar>

using namespace Memory;

MemoryPlanningWidget::MemoryPlanningWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemoryPlanningWidget)
{
    ui->setupUi(this);

    m_gridScene = NULL;
    m_lineScene = NULL;

    connect(ui->radioButtonGridView,SIGNAL(toggled(bool)),this,SLOT(changeScene()));
}

MemoryPlanningWidget::~MemoryPlanningWidget()
{
    delete ui;
}


void MemoryPlanningWidget::changeScene()
{
    ui->memoryView->changeScene();
    if(m_mode == MemoryGrid)
    {
        m_mode = MemoryLine;
        m_lineScene = static_cast<MLineScene*>(ui->memoryView->scene());
    }
    else if(m_mode == MemoryLine)
    {
        m_mode = MemoryGrid;
        m_gridScene = static_cast<MGridScene*>(ui->memoryView->scene());
    }
}

void MemoryPlanningWidget::setGridView()
{
//    clear();

    m_mode = MemoryGrid;

    if(!m_gridScene)
    {
        m_gridScene = new MGridScene(this);

        connect(m_gridScene,SIGNAL(itemInfoChanged(const QString&)),this,SLOT(setItemInfo(const QString&)));
        connect(m_gridScene,SIGNAL(unitInfoChanged(const QString&)),this,SLOT(setUnitInfo(const QString&)));
        m_gridScene->setBackgroundBrush(QBrush(QColor(Qt::gray).lighter(130)));
    }

    ui->memoryView->setScene(m_gridScene);

}

void MemoryPlanningWidget::setLineView()
{
//    clear();

    m_mode = MemoryLine;

    if(!m_lineScene)
        m_lineScene = new MLineScene(this);

    ui->memoryView->setScene(m_lineScene);
}

void MemoryPlanningWidget::setUnitInfo(const QString &text)
{
    ui->labelUnitInfo->setText(text);
}

void MemoryPlanningWidget::setItemInfo(const QString &text)
{
    ui->labelItemInfo->setText(text);
}

void MemoryPlanningWidget::setMemory(const KaMemory &kaMemory)
{
    ui->memoryView->setMemory(kaMemory);
}

MGridScene *MemoryPlanningWidget::gridScene() const
{
    if(m_mode == MemoryGrid)
        return m_gridScene;
    return NULL;
}

MLineScene *MemoryPlanningWidget::lineScene() const
{
    if(m_mode == MemoryLine)
        return m_lineScene;
    return NULL;
}




