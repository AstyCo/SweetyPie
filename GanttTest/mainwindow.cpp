#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ganttitem.h"
#include "ganttmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

//    GanttView * view = new GanttView;
    GanttModel * model = new GanttModel();
//    //model->insertRows(0,4);


    QDateTime begin = QDateTime::fromString("16.02.2015", "dd.MM.yyyy");
    QDateTime end = QDateTime::fromString("24.02.2015 15:30:43", "dd.MM.yyyy hh:mm:ss");
    model->addItem(0,"lol",begin,end);

//    //qDebug()<<"dt1"<< begin<< end;

////    begin = QDateTime::fromString("04.02.2015", "dd.MM.yyyy");
////    end = QDateTime::fromString("07.02.2015", "dd.MM.yyyy");
////    model->addItem(1,"lol2",begin,end);

////    begin = QDateTime::fromString("05.02.2015", "dd.MM.yyyy");
////    end = QDateTime::fromString("06.02.2015", "dd.MM.yyyy");
////    model->addItem(2,"lol3",begin,end);

    for(int i = 1; i < 6; i++)
    {
        begin = QDateTime::fromString("04.02.2015", "dd.MM.yyyy");
        end = QDateTime::fromString("07.02.2015", "dd.MM.yyyy");
        model->addItem(i,"lol"+QString::number(i),begin,end);
    }





    ui->setupUi(this);
    ui->ganttWidget->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}
