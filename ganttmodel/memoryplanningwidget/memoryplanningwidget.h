#ifndef MEMORYPLANNINGWIDGET_HPP
#define MEMORYPLANNINGWIDGET_HPP

#include "ganttproject_global.h"
#include "memoryview.h"

#include <QGraphicsScene>
#include <QWidget>
#include <QVector>


namespace Ui {
class MemoryPlanningWidget;
}

class Memory;
class MGridScene;
class MLineScene;




class GANTTMODELSHARED_EXPORT MemoryPlanningWidget : public QWidget
{
    Q_OBJECT

public:
    MemoryPlanningWidget(QWidget *parent = 0);
    ~MemoryPlanningWidget();


    void setMemory(const Memory& kaMemory);

    MGridScene* gridScene() const;
    MLineScene* lineScene() const;

    void setShowButtons(bool flag);

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

    void hideGridWidgets();
    void showGridWidgets();


private:
    Ui::MemoryPlanningWidget *ui;
    MGridScene* m_gridScene;
    MLineScene* m_lineScene;

    MemoryView::MemoryViewMode m_mode;
    bool m_showButtons;

    QVector<QWidget*> m_gridWidgets;
};

#endif // MEMORYPLANNINGWIDGET_HPP
