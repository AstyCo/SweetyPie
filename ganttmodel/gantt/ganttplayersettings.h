#ifndef GANTTPLAYERSETTINGS_H
#define GANTTPLAYERSETTINGS_H

#include <QToolBar>

namespace Ui {
class GanttPlayerSettings;
}

class GanttPlayerSettings : public QToolBar
{
    Q_OBJECT

public:
    explicit GanttPlayerSettings(QWidget *parent = 0);
    ~GanttPlayerSettings();

signals:
    void speedX1();
    void speedX3();
    void speedX5();

private:
    void initActions();

private:
    Ui::GanttPlayerSettings *ui;

};

#endif // GANTTPLAYERSETTINGS_H
