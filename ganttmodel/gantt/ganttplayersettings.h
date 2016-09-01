#ifndef GANTTPLAYERSETTINGS_H
#define GANTTPLAYERSETTINGS_H

#include <QToolBar>

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

};

#endif // GANTTPLAYERSETTINGS_H
