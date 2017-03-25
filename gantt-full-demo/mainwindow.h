#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "utcdatetime.h"
#include "gantttreemodel.h"
#include "itemadditiondialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    void init();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


protected:
    void keyPressEvent(QKeyEvent *e);

private:
    void testGantt();

    Ui::MainWindow *ui;
    GanttTreeModel *_model;
    GanttInfoItem *_item, *_newItem;
    ItemAdditionDialog *_dlg;

private slots:
    void addBefore();
    void addAfter();
    void addInner();

    void addFirst();
    void addLast();

    void onCustomContextMenuRequested(const QPoint &pos);
    void on_pushButton_ganttClear_clicked();
    void on_radioButton_ganttPlayer_toggled(bool checked);
    void on_pushButton_ganttAddItems_clicked();

private:
    GanttInfoItem *produceItem(const ItemAdditionDialog *dlg);
};

#endif // MAINWINDOW_H
