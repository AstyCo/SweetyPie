#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "utcdatetime.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    srand(time(0));
    QVector<QPointF> data, data2;
    for(int i=0; i<100; i++)
    {
        data.append(QPointF(QDateTime::currentDateTime().toTime_t()+(i*60),(rand() % 100)));
        data2.append(QPointF(QDateTime::currentDateTime().toTime_t()+(i*65),(rand() % 1000)+100));
    }

    //ui->widget->setChartActons(caScale|caGrid);
    ui->widget->setLeftAxis("111111",0,0);
    ui->widget->setRightAxis("222222",0,800);

    ui->widget->setCountLastPoints(111);

    ui->widget->addInterval(data[0].x(), data[30].x(), QColor(229, 229, 229), QColor(187, 187, 187));

    ui->widget->addInterval(data[60].x(), data[80].x(),  QColor(229, 229, 229), QColor(187, 187, 187));

    ui->widget->setData("11111111111111",Qt::blue,data);
    ui->widget->setData("22222222222222",Qt::black,data2,QwtPlot::yRight);


    ui->widgetIntervalSlider->setMinValue(0);
    ui->widgetIntervalSlider->setMaxValue(ui->widget->maximumDt().toBshvTime() - ui->widget->minimumDt().toBshvTime());
    ui->widgetIntervalSlider->setEndValue(ui->widgetIntervalSlider->maxValue());

    connect(ui->widgetIntervalSlider,SIGNAL(valueChanged(EventHandle,long)),this,SLOT(setInterval()));

   // ui->widget->curve()->

    // обновить значения для интервала
    //ui->widget->SetMinMeanMaxLbls(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setInterval()
{
    UtcDateTime rez1;
    rez1.setBshvTime(ui->widgetIntervalSlider->beginValue()+ui->widget->minimumDt().toBshvTime());

    UtcDateTime rez2;
    rez2.setBshvTime(ui->widgetIntervalSlider->endValue()+ui->widget->minimumDt().toBshvTime());
    ui->widget->selectIntervalByDates(rez1,rez2);
}
