#ifndef MEMORYPLANNINGWIDGET_HPP
#define MEMORYPLANNINGWIDGET_HPP

#include "ganttmodel_global.h"
#include "memoryview.h"

#include <QGraphicsScene>
#include <QWidget>


namespace Ui {
class MemoryPlanningWidget;
}

class KaMemory;
class MGridScene;
class MLineScene;




class GANTTMODELSHARED_EXPORT MemoryPlanningWidget : public QWidget
{
    Q_OBJECT

public:
    MemoryPlanningWidget(QWidget *parent = 0);
    ~MemoryPlanningWidget();


    void setMemory(const KaMemory& kaMemory);

    MGridScene* gridScene() const;
    MLineScene* lineScene() const;

public slots:
    void changeScene();

    void setGridView();
    void setLineView();

    void setUnitInfo(const QString& text);
    void setItemInfo(const QString& text);
    void updateParts();

private slots:
    void on_pushButtonEmpty_clicked();

    void on_pushButtonFree_clicked();

    void on_pushButtonPendingWrite_clicked();

    void on_pushButtonPendingRead_clicked();

protected:
    void resizeEvent(QResizeEvent *);

private:
    Ui::MemoryPlanningWidget *ui;
    MGridScene* m_gridScene;
    MLineScene* m_lineScene;

    Memory::MemoryViewMode m_mode;
};

#endif // MEMORYPLANNINGWIDGET_HPP
