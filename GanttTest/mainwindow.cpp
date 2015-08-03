#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ganttitem.h"
#include "ganttmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    GanttModel * model = new GanttModel();

    QDateTime begin = QDateTime::fromString("16.02.2015", "dd.MM.yyyy");
    QDateTime end = QDateTime::fromString("24.02.2015 15:30:43", "dd.MM.yyyy hh:mm:ss");
    model->addItem("lol0",begin,end);

    for(int i = 1; i < 6; i++)
    {
        begin = QDateTime::fromString("03.02.2015", "dd.MM.yyyy");
        end = QDateTime::fromString("06.02.2015", "dd.MM.yyyy");
        model->addItem("lol"+QString::number(i),begin,end);
    }

    for(int i = 0; i < 3; i++)
    {
        begin = QDateTime::fromString("04.02.2015", "dd.MM.yyyy");
        end = QDateTime::fromString("07.02.2015", "dd.MM.yyyy");
        model->addItem("lol1."+QString::number(i),begin,end, model->index(1, 0));
    }

    for(int i = 0; i < 3; i++)
    {
        begin = QDateTime::fromString("05.02.2015", "dd.MM.yyyy");
        end = QDateTime::fromString("08.02.2015", "dd.MM.yyyy");
        model->addItem("lol1.0."+QString::number(i),begin,end,
                       model->index(1, 0).child(0, 0));
    }

    for(int i = 0; i < 3; i++)
    {
        begin = QDateTime::fromString("09.02.2015", "dd.MM.yyyy");
        end = QDateTime::fromString("11.02.2015", "dd.MM.yyyy");
        model->addItem("lol1.0.2."+QString::number(i),begin,end,
                       model->index(1, 0).child(0, 0).child(2, 0));
    }




    ui->setupUi(this);
    ui->ganttWidget->setModel(model);
    //ui->ganttWidget->ShowOnlyPlayer(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
