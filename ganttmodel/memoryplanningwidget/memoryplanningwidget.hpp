#ifndef MEMORYPLANNINGWIDGET_HPP
#define MEMORYPLANNINGWIDGET_HPP

#include <QGraphicsScene>

#include <ganttmodel_global.h>
#include <QWidget>

namespace Ui {
class MemoryPlanningWidget;
}

class GANTTMODELSHARED_EXPORT MemoryPlanningWidget : public QWidget
{
    Q_OBJECT

public:
    MemoryPlanningWidget(QWidget *parent = 0);
    ~MemoryPlanningWidget();

    QGraphicsScene *scene() const;
    void adjustViewSize();

private:
    Ui::MemoryPlanningWidget *ui;
    QGraphicsScene* m_scene;
};

#endif // MEMORYPLANNINGWIDGET_HPP
