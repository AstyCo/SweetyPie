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

private slots:
    void setInterval();

};

#endif // MAINWINDOW_H
