#ifndef GANTTWIDGET_H
#define GANTTWIDGET_H

#include <QWidget>

namespace Ui {
class GanttWidget;
}

class GanttWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GanttWidget(QWidget *parent = 0);
    ~GanttWidget();

private:
    Ui::GanttWidget *ui;
};

#endif // GANTTWIDGET_H
