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
#include <QResizeEvent>


MemoryPlanningWidget::MemoryPlanningWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemoryPlanningWidget)
{
    ui->setupUi(this);
    setShowViews(false); // disabled by default

    ui->pushButtonEmpty->setStyleSheet(QString("background-color: %1").arg(MemoryState_to_QColor(KaMemoryPart::Empty).name()));
    ui->pushButtonFree->setStyleSheet(QString("background-color: %1").arg(MemoryState_to_QColor(KaMemoryPart::Free).name()));
    ui->pushButtonPendingRead->setStyleSheet(QString("background-color: %1").arg(MemoryState_to_QColor(KaMemoryPart::PendingRead).name()));
    ui->pushButtonPendingWrite->setStyleSheet(QString("background-color: %1").arg(MemoryState_to_QColor(KaMemoryPart::PendingWrite).name()));


    QPixmap pixmap(":/icons/icons/plus.png");
    QIcon buttonIcon(pixmap);

    ui->pushButtonEmpty->setIcon(buttonIcon);
    ui->pushButtonFree->setIcon(buttonIcon);
    ui->pushButtonPendingRead->setIcon(buttonIcon);
    ui->pushButtonPendingWrite->setIcon(buttonIcon);


//    ui->labelInfo->setMinimumSize(QSize(50,16));
//    ui->labelInfo->setMaximumSize(QSize(530,16));
//    ui->labelItemInfo->setMinimumSize(QSize(100,16));
//    ui->labelItemInfo->setMaximumSize(QSize(100,16));

    m_gridScene = NULL;
    m_lineScene = NULL;

    m_gridWidgets
                  << ui->pushButtonEmpty
                  << ui->pushButtonFree
                  << ui->pushButtonPendingRead
                  << ui->pushButtonPendingWrite
//                  << ui->labelInfo
                  << ui->labelItemInfo
                  << ui->labelUnitInfo;

    setShowButtons(false);
    hideGridWidgets();

    setGridView(); // By default showing grid

    connect(ui->radioButtonGridView,SIGNAL(toggled(bool)),this,SLOT(changeScene()));

}

MemoryPlanningWidget::~MemoryPlanningWidget()
{
    delete ui;
}


void MemoryPlanningWidget::changeScene()
{
    ui->memoryView->changeScene();
    if(m_mode == MemoryView::MemoryGrid)
    {
        m_mode = MemoryView::MemoryLine;
        m_lineScene = static_cast<MLineScene*>(ui->memoryView->scene());

        hideGridWidgets();
    }
    else if(m_mode == MemoryView::MemoryLine)
    {
        m_mode = MemoryView::MemoryGrid;
        m_gridScene = static_cast<MGridScene*>(ui->memoryView->scene());

        showGridWidgets();
    }
}

void MemoryPlanningWidget::clear()
{
    m_gridScene->clear();
}

void MemoryPlanningWidget::hoverPart(const KaMemoryPart &part)
{
    if(m_mode==MemoryView::MemoryGrid && m_gridScene)
        m_gridScene->hoverPart(part);
}

void MemoryPlanningWidget::setGridView()
{
//    clear();

    m_mode = MemoryView::MemoryGrid;

    if(!m_gridScene)
    {
        m_gridScene = new MGridScene(this);

        connect(m_gridScene,SIGNAL(kaMemoryPartHoverEnter(KaMemoryPart)),this,SIGNAL(kaMemoryPartHoverEnter(KaMemoryPart)));
        connect(m_gridScene,SIGNAL(kaMemoryPartHoverLeave(KaMemoryPart)),this,SIGNAL(kaMemoryPartHoverLeave(KaMemoryPart)));

        connect(m_gridScene,SIGNAL(itemInfoChanged(const QString&)),this,SLOT(setItemInfo(const QString&)));
        connect(m_gridScene,SIGNAL(unitInfoChanged(const QString&)),this,SLOT(setUnitInfo(const QString&)));

        connect(m_gridScene,SIGNAL(startHighlightChanged(long)),this,SLOT(onSelectionChanged()));
        connect(m_gridScene,SIGNAL(lengthHighlightChanged(long)),this,SLOT(onSelectionChanged()));

//        connect(this,SIGNAL(selectionChanged(QList<KaMemoryPart*>)),this,SLOT(updateParts()));
//        connect(this,SIGNAL(selectionChanged(QList<KaMemoryPart*>)),this,SLOT(updateParts()));
    }

    ui->memoryView->setScene(m_gridScene);

    showGridWidgets();

}

void MemoryPlanningWidget::setLineView()
{
    if(m_mode == MemoryView::MemoryGrid)
        changeScene();
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
//    return;
    static QString base = tr("Пересекаемые области памяти:\n\n");
    if(m_mode != MemoryView::MemoryGrid)
        return;
    QList<QSharedPointer<KaMemoryPart> > parts = m_gridScene->crossingParts();

    QString text;

    foreach(QSharedPointer<KaMemoryPart> part, parts)
    {
        text += '['+MemoryState_to_QString(part->state())+"] ";
        if(part->length()!=0)
            text += m_gridScene->toAdress(part->start(),part->start()+part->length()-1)+'\n';
    }
//    ui->crossingParts->setText(base+text);
}

void MemoryPlanningWidget::onSelectionChanged()
{
    QList<KaMemoryPart *> res;
    foreach(QSharedPointer<KaMemoryPart> part, m_gridScene->crossingParts())
        res.append(part.data());
    emit selectionChanged( res );
}

void MemoryPlanningWidget::setMemory(QSharedPointer<KaMemory> kaMemory)
{
    ui->memoryView->setMemory(kaMemory);

}

MGridScene *MemoryPlanningWidget::gridScene() const
{
    if(m_mode == MemoryView::MemoryGrid)
        return m_gridScene;
    return NULL;
}

MLineScene *MemoryPlanningWidget::lineScene() const
{
    if(m_mode == MemoryView::MemoryLine)
        return m_lineScene;
    return NULL;
}

bool MemoryPlanningWidget::setSelected(long start, long length)
{
    MGridScene *scene=gridScene();
    if(scene==NULL)
        return false;
    else
        return scene->setSelected(start,length);

}

void MemoryPlanningWidget::setShowButtons(bool flag)
{
    if(flag == m_showButtons)
        return;
    m_showButtons = flag;

    if(m_showButtons)
    {
        ui->pushButtonEmpty->show();
        ui->pushButtonFree->show();
        ui->pushButtonPendingRead->show();
        ui->pushButtonPendingWrite->show();
    }
    else
    {
        ui->pushButtonEmpty->hide();
        ui->pushButtonFree->hide();
        ui->pushButtonPendingRead->hide();
        ui->pushButtonPendingWrite->hide();
    }
}

void MemoryPlanningWidget::setShowViews(bool flag)
{
    ui->radioButtonGridView->setShown(flag);
    ui->radioButtonLineView->setShown(flag);
}

void MemoryPlanningWidget::setSelectionMode(MGridScene::SelectionMode mode)
{
    if(m_mode==MemoryView::MemoryGrid && m_gridScene!=NULL)
        m_gridScene->setSelectionMode(mode);
}

void MemoryPlanningWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    ui->labelUnitInfo->setMaximumWidth(qMax(width() - 50 - ui->labelItemInfo->width(),20));

}





void MemoryPlanningWidget::on_pushButtonEmpty_clicked()
{
    m_gridScene->setEmpty();
}

void MemoryPlanningWidget::on_pushButtonFree_clicked()
{
    m_gridScene->setFree();
}

void MemoryPlanningWidget::on_pushButtonPendingWrite_clicked()
{
    m_gridScene->setPendingWrite();
}

void MemoryPlanningWidget::on_pushButtonPendingRead_clicked()
{
    m_gridScene->setPendingRead();
}

void MemoryPlanningWidget::hideGridWidgets()
{
    foreach(QWidget* widget, m_gridWidgets)
        widget->hide();
}

void MemoryPlanningWidget::showGridWidgets()
{
    foreach(QWidget* widget, m_gridWidgets)
    {
        if(dynamic_cast<QPushButton*>(widget)&&!m_showButtons)
            continue;
        widget->show();
    }
}
