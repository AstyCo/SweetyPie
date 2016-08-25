#ifndef GANTTINTERVALSLIDERZOOM_H
#define GANTTINTERVALSLIDERZOOM_H

#include <QWidget>

namespace Ui {
class GanttIntervalSliderZoom;
}

class GanttIntervalSliderZoom : public QWidget
{
    Q_OBJECT

public:
    explicit GanttIntervalSliderZoom(QWidget *parent = 0);
    ~GanttIntervalSliderZoom();

signals:
    void increaseButtonClicked(bool);
    void restoreButtonClicked(bool);


private:
    Ui::GanttIntervalSliderZoom *ui;
};

#endif // GANTTINTERVALSLIDERZOOM_H
