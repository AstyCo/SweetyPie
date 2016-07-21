#ifndef MEMORYPLANNINGWIDGET_HPP
#define MEMORYPLANNINGWIDGET_HPP

#include <QGraphicsScene>

#include <ganttmodel_global.h>
#include <QWidget>

namespace Ui {
class MemoryPlanningWidget;
}

class KaMemory;

namespace Memory
{
    enum MemoryViewMode
    {
        MemoryLine,
        MemoryGrid,
        MemoryViewMode_count
    };
}

class GANTTMODELSHARED_EXPORT MemoryPlanningWidget : public QWidget
{
    Q_OBJECT

public:
    MemoryPlanningWidget(QWidget *parent = 0);
    ~MemoryPlanningWidget();

    void init(Memory::MemoryViewMode mode);
    void changeScene();
    void setMemory(const KaMemory& kaMemory);

    QGraphicsScene *scene() const;
    void adjustViewSize();

    Memory::MemoryViewMode mode() const;

protected:
    void keyPressEvent(QKeyEvent *); // FOR TEST

private:
    void clear();

private:
    Memory::MemoryViewMode m_mode;
    Ui::MemoryPlanningWidget *ui;
    QGraphicsScene* m_scene;
};

#endif // MEMORYPLANNINGWIDGET_HPP
