#include "memoryplanningwidget.hpp"
#include "ui_memoryplanningwidget.h"

#include "memorywidget.hpp"
#include "memoryview.hpp"
#include "memoryscene.hpp"
#include "memoryitempresentation.hpp"

#include <QDebug>
#include <QScrollBar>

#define TEST_LINE 1


MemoryPlanningWidget::MemoryPlanningWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemoryPlanningWidget)
{
    ui->setupUi(this);

    if(!TEST_LINE)
    {
        m_scene = new MemoryScene(this);
        m_scene->setBackgroundBrush(QBrush(QColor(Qt::gray).lighter(130)));

        ui->memoryView->setScene(m_scene);

        ui->memoryView->adjustSize();

        ui->memoryView->setFrameStyle(0);
        ui->memoryView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        ui->memoryView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

#if defined(Q_WS_S60) || defined(Q_WS_MAEMO_5) || defined(Q_WS_SIMULATOR)
        //    ui->memoryView->showMaximized();
#else
        //    show();
#endif
    }
    else
    {
        m_scene = new KaMemoryScene(this);

        ui->memoryView->setScene(m_scene);

        ui->memoryView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }


}

MemoryPlanningWidget::~MemoryPlanningWidget()
{
    delete ui;
}
QGraphicsScene *MemoryPlanningWidget::scene() const
{
    return m_scene;
}

void MemoryPlanningWidget::adjustViewSize()
{
    ui->memoryView->adjustSize();
}

