#ifndef GANTTINTERVALSLIDERZOOM_H
#define GANTTINTERVALSLIDERZOOM_H

#include <QToolBar>


class GanttIntervalSliderZoom : public QToolBar
{
    Q_OBJECT

public:
    explicit GanttIntervalSliderZoom(QWidget *parent = 0);
    ~GanttIntervalSliderZoom();

signals:
    void increaseButtonClicked(bool);
    void restoreButtonClicked(bool);
private:
    void initActions();

};

#endif // GANTTINTERVALSLIDERZOOM_H
