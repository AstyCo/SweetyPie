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

    testChartWidget();
    testGanttModel();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::testChartWidget()
{
  srand(time(0));
  QVector<QPointF> data, data2;
  for(int i=0; i<100; i++)
  {
      data.append(QPointF(QDateTime::currentDateTime().toTime_t()+(i*60),(rand() % 100)));
      data2.append(QPointF(QDateTime::currentDateTime().toTime_t()+(i*65),(rand() % 1000)+100));
  }

  qDebug()<<int(caScale|caGrid|caMaxMinLines|caTimer);
  ui->widget->setChartActions(caScale|caGrid|caMaxMinLines|caTimer|caSelectIntervals);
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

  connect(ui->widgetIntervalSlider,SIGNAL(valueChanged(IntervalSlider::EventHandle,long)),this,SLOT(setInterval()));

  ChartWidget *chart = new ChartWidget();
  chart->setLeftAxis("Test Chart 1 big title");
  chart->setData("chart 1", Qt::blue, data);
  ui->widget_chartGroup->addChart(chart);

  chart = new ChartWidget();
  chart->setLeftAxis("Test Chart 2");
  chart->setData("chart 2", Qt::red, data);
  ui->widget_chartGroup->addChart(chart);

  chart = new ChartWidget();
  chart->setLeftAxis("Test Chart 3 norm");
  chart->setData("chart 3", Qt::green, data);
  ui->widget_chartGroup->addChart(chart);
}

void MainWindow::testGanttModel()
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

  ui->ganttWidget->setModel(model);
  //ui->ganttWidget->ShowOnlyPlayer(true);
}

void MainWindow::setInterval()
{
    UtcDateTime rez1;
    rez1.setBshvTime(ui->widgetIntervalSlider->beginValue()+ui->widget->minimumDt().toBshvTime());

    UtcDateTime rez2;
    rez2.setBshvTime(ui->widgetIntervalSlider->endValue()+ui->widget->minimumDt().toBshvTime());
    ui->widget->selectIntervalByDates(rez1,rez2);
}
