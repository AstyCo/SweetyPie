#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private:
    Ui::MainWindow *ui;
    void testChartWidget();
    void testChartGroupWidget();
    void testGanttModel();
    void testMemoryPlanningWidget();

private slots:
    void setInterval();

    void on_checkBox_syncAxisX_toggled(bool checked);
    void on_pushButton_clicked();
    void on_checkBox_showLegend_toggled(bool checked);
    void on_pushButton_ganttAddItems_clicked();
    void on_pushButton_ganttClear_clicked();
    void on_radioButton_ganttPlayer_toggled(bool checked);
};

#endif // MAINWINDOW_H
