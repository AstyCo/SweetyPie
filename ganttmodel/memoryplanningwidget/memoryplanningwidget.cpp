#include "memoryplanningwidget.h"
#include "ui_memoryplanningwidget.h"

#include "memoryview.h"
#include "mgrid_scene.h"
#include "mline_scene.h"
#include "kamemorypart.h"
#include "kamemory.h"
#include <QDebug>
#include <QBitmap>
#include <QScrollBar>

using namespace Memory;

MemoryPlanningWidget::MemoryPlanningWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemoryPlanningWidget)
{
    ui->setupUi(this);

    ui->pushButtonEmpty->setStyleSheet(QString("background-color: %1").arg(MemoryState_to_QColor(Memory::Empty).name()));
    ui->pushButtonFree->setStyleSheet(QString("background-color: %1").arg(MemoryState_to_QColor(Memory::Free).name()));
    ui->pushButtonPendingRead->setStyleSheet(QString("background-color: %1").arg(MemoryState_to_QColor(Memory::PendingRead).name()));
    ui->pushButtonPendingWrite->setStyleSheet(QString("background-color: %1").arg(MemoryState_to_QColor(Memory::PendingWrite).name()));


    QPixmap pixmap(":/icons/icons/plus.png");
    QIcon buttonIcon(pixmap);

    ui->pushButtonEmpty->setIcon(buttonIcon);
    ui->pushButtonFree->setIcon(buttonIcon);
    ui->pushButtonPendingRead->setIcon(buttonIcon);
    ui->pushButtonPendingWrite->setIcon(buttonIcon);


    ui->labelInfo->setMinimumSize(QSize(30,16));
    ui->labelInfo->setMaximumSize(QSize(30,16));
    ui->labelItemInfo->setMinimumSize(QSize(100,16));
    ui->labelItemInfo->setMaximumSize(QSize(100,16));




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
        connect(m_gridScene,SIGNAL(startHighlightChanged(long)),this,SLOT(updateParts()));
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

void MemoryPlanningWidget::updateParts()
{
    static QString base = tr("Crossing parts:\n\n");
    if(m_mode != MemoryGrid)
        return;
    QList<KaMemoryPart> parts = m_gridScene->crossingParts();

    QString text;

    foreach(KaMemoryPart part, parts)
    {
        text += '['+QString::number(part.id())+"] ";
        text += m_gridScene->toAdress(part.start(),part.finish())+'\n';
    }
    ui->crossingParts->setText(base+text);
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





void MemoryPlanningWidget::on_pushButtonEmpty_clicked()
{
    KaMemoryPart part = m_gridScene->setEmpty();
    qDebug() << ' ' << part.start()
             << ' ' << part.finish()
             << ' ' << MemoryState_to_trQString(part.state())
             << ' ' << "["+QString::number(part.id())+"]";
}

void MemoryPlanningWidget::on_pushButtonFree_clicked()
{
    KaMemoryPart part = m_gridScene->setFree();
    qDebug() << ' ' << part.start()
             << ' ' << part.finish()
             << ' ' << MemoryState_to_trQString(part.state())
             << ' ' << "["+QString::number(part.id())+"]";
}

void MemoryPlanningWidget::on_pushButtonPendingWrite_clicked()
{
    KaMemoryPart part = m_gridScene->setPendingWrite();
    qDebug() << ' ' << part.start()
             << ' ' << part.finish()
             << ' ' << MemoryState_to_trQString(part.state())
             << ' ' << "["+QString::number(part.id())+"]";
}

void MemoryPlanningWidget::on_pushButtonPendingRead_clicked()
{
    KaMemoryPart part = m_gridScene->setPendingRead();
    qDebug() << ' ' << part.start()
             << ' ' << part.finish()
             << ' ' << MemoryState_to_trQString(part.state())
             << ' ' << "["+QString::number(part.id())+"]";
}
