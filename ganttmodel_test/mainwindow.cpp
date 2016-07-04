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
  testChartGroupWidget();
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
  UtcDateTime baseDt = QDateTime::currentDateTime();
  for(int i = 0; i < 100; i++)
  {    
    data.append(ChartWidget::dtToPoint(baseDt.addSecs(i * 60), (rand() % 100)));
    data2.append(ChartWidget::dtToPoint(baseDt.addSecs(i * 65), (rand() % 1000) + 100));
  }

  qDebug() << int(caScale | caGrid | caMaxMinLines | caTimer);
  ui->widget->getActionsToolBar()->setChartActions(QSet<ChartActions>()
        << caScale << caGrid << caMaxMinLines << caTimer << caSelectInterval);
  ui->widget->setLeftAxis("111111", 0, 0);
  ui->widget->setRightAxis("222222", 0, 800);

  ui->widget->setCountLastPoints(111);

  ui->widget->addZone("zone 1", data[0].x(), data[30].x(), QColor(229, 229, 229), QColor(187, 187, 187));

  ui->widget->addZone("zone 2", data[60].x(), data[80].x(),  QColor(229, 229, 229), QColor(187, 187, 187));

  ui->widget->setData("11111111111111", data);
  ui->widget->setData("22222222222222", data2, QwtPlot::yRight);

  ui->widgetIntervalSlider->setMinValue(0);
  ui->widgetIntervalSlider->setMaxValue(TimeSpan(ui->widget->maximumDt() - ui->widget->minimumDt()).totalSeconds());
  ui->widgetIntervalSlider->setEndHandle(ui->widgetIntervalSlider->maxValue());

  connect(ui->widgetIntervalSlider, SIGNAL(valueChanged(IntervalSlider::EventHandle, long)), this, SLOT(setInterval()));


}

void MainWindow::testChartGroupWidget()
{
  QVector<QPointF> data, data2, data3;
  UtcDateTime baseDt = QDateTime::currentDateTime();
  for(int i = 0; i < 100; i++)
  {
    UtcDateTime time = baseDt.addSecs(i * 60);
    data.append(ChartWidget::dtToPoint(time, (rand() % 100)));
    data2.append(ChartWidget::dtToPoint(time, (rand() % 100) * 100));
    data3.append(ChartWidget::dtToPoint(time, (rand() % 100) * 1000));
  }

  ChartWidget *chart = new ChartWidget();
  chart->setLeftAxis("Test Chart 1 big title");
  chart->setData("chart 1", data);
  ui->widget_chartGroup->addChart(chart);

  chart = new ChartWidget();
  chart->setLeftAxis("Test Chart 2");
  chart->setData("chart 2", data2);
  ui->widget_chartGroup->addChart(chart);

  chart = new ChartWidget();
  chart->setLeftAxis("Test Chart 3 norm");
  chart->setData("chart 3", data3);
  ui->widget_chartGroup->addChart(chart);
  ui->widget_chartGroup->setPanelSelectIntervalVisible(true);
}

void MainWindow::testGanttModel()
{
  GanttModel * model = new GanttModel();

  QDateTime begin = QDateTime::fromString("16.02.2015", "dd.MM.yyyy");
  QDateTime end = QDateTime::fromString("24.02.2015 15:30:43", "dd.MM.yyyy hh:mm:ss");
  model->addItem("lol0", begin, end);

  for(int i = 1; i < 6; i++)
  {
    begin = QDateTime::fromString("03.02.2015", "dd.MM.yyyy");
    end = QDateTime::fromString("06.02.2015", "dd.MM.yyyy");
    model->addItem("lol" + QString::number(i), begin, end);
  }

  for(int i = 0; i < 3; i++)
  {
    begin = QDateTime::fromString("04.02.2015", "dd.MM.yyyy");
    end = QDateTime::fromString("07.02.2015", "dd.MM.yyyy");
    model->addItem("lol1." + QString::number(i), begin, end, model->index(1, 0));
  }

  for(int i = 0; i < 3; i++)
  {
    begin = QDateTime::fromString("05.02.2015", "dd.MM.yyyy");
    end = QDateTime::fromString("08.02.2015", "dd.MM.yyyy");
    model->addItem("lol1.0." + QString::number(i), begin, end,
                   model->index(1, 0).child(0, 0));
  }

  for(int i = 0; i < 3; i++)
  {
    begin = QDateTime::fromString("09.02.2015", "dd.MM.yyyy");
    end = QDateTime::fromString("11.02.2015", "dd.MM.yyyy");
    model->addItem("lol1.0.2." + QString::number(i), begin, end,
                   model->index(1, 0).child(0, 0).child(2, 0));
  }

  ui->ganttWidget->setModel(model);
  //ui->ganttWidget->ShowOnlyPlayer(true);
}

void MainWindow::setInterval()
{
  UtcDateTime baseDt = ui->widget->minimumDt();
  UtcDateTime rez1 = baseDt.addSecs(ui->widgetIntervalSlider->beginHandle());

  UtcDateTime rez2 = baseDt.addSecs(ui->widgetIntervalSlider->endHandle());
  ui->widget->setIntervalSelectionByDates(rez1, rez2);
}
