#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "utcdatetime.h"

#include "ganttinfonode.h"
#include "ganttglobalvalues.h"

#include <qwt/qwt_scale_div.h>
#include "memoryplanningwidget.h"
#include "mgrid_scene.h"
#include "mline_scene.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  installEventFilter(ui->widgetIntervalSlider);

  testChartWidget();
  testChartGroupWidget();
  testGanttModel();
  testMemoryPlanningWidget();
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
  for(int i = 0; i < 60; i++)
  {    
    data.append(ChartWidget::dtToPoint(baseDt.addSecs(i * 60), (rand() % 100)));    
  }

  for(int i = 60; i < 100; i++)
  {
    data2.append(ChartWidget::dtToPoint(baseDt.addSecs(i * 65), (rand() % 1000) + 100));
  }

  ui->widget->getActionsToolBar()->setChartActions(QSet<ChartActions>()
        << caScale << caGrid << caMaxMinLines << caTimer << caSelectInterval
                                                   << caDetailsPanel << caSettingsDlg);
  ui->widget->setLeftAxis("111111", 0, 0);
  ui->widget->setRightAxis("222222", 0, 800);

  ui->widget->setCountLastPoints(111);

  ui->widget->addZone("zone 1", data[0].x(), data[30].x(), QColor(229, 229, 229), QColor(187, 187, 187));

  //ui->widget->addZone("zone 2", data[60].x(), data[80].x(),  QColor(229, 229, 229), QColor(187, 187, 187));

  ui->widget->setData("11111111111111", data);
  ui->widget->setData("22222222222222", data2, QwtPlot::yRight);

  ui->widgetIntervalSlider->setLimits(0,TimeSpan(ui->widget->maximumDt() - ui->widget->minimumDt()).totalSeconds());
  ui->checkBox_showLegend->setChecked(true);

  connect(ui->widgetIntervalSlider, SIGNAL(valueChanged(IntervalSlider::ClippedHandle, long long)), this, SLOT(setInterval()));


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
  ui->widget_chartGroup->setSyncChartsByAxisX(false);
  ui->widget_chartGroup->setPanelSelectIntervalVisible(true);
}
QList<GanttInfoItem*> generateTest()
{
    QList<GanttInfoItem*> testList;
    for(int i = 0; i<4; ++i)
    {
        GanttInfoNode *node = new GanttInfoNode;

        node->setTitle("node"+QString::number(i));
        int year = 2014 + qrand()%2,
                month = 1 + qrand()%3,
                day = 1 + qrand()%2,
                hour = qrand()%2,
                minute = qrand()%60,
                sec = qrand()%60,
                microsec = qrand()%1000000;

        node->setCalcDt(UtcDateTime(year,month,day,hour,minute,sec,microsec));

        QColor color = QColor::fromRgb(qrand()%255,qrand()%255,qrand()%255);
        int max = qrand()%4;

        for(int j = 0; j<max; ++j)
        {
            GanttInfoLeaf *leaf = new GanttInfoLeaf;

            int year = 2016,
                    month = 1 /*+ qrand()%6*/,
                    day = 1 + qrand()%2,
                    hour = qrand()%2,
                    minute = qrand()%60,
                    sec = qrand()%60,
                    microsec = qrand()%1000000;

            UtcDateTime start(year,month,day,hour,minute,sec,microsec),
                    finish = start
                        .addMicroseconds((hour * SECONDS_IN_HOUR * _MICROSECONDS_IN_SECOND)
                                         + minute * SECONDS_IN_MINUTE * _MICROSECONDS_IN_SECOND
                                         + sec * _MICROSECONDS_IN_SECOND
                                         + microsec)
                        .addDays(day - 1)
                        .addMonths(month - 1);

            leaf->setStart(start);
            leaf->setFinish(finish);
            leaf->setColor(color);
            leaf->setTitle("leaf"+QString::number(1 + i) + ':' + QString::number(1 + j));

            node->append(leaf);
        }


        testList.append(node);
    }

    return testList;
}

void MainWindow::testGanttModel()
{
    connect(ui->ganttWidget,SIGNAL(currentDtChanged(UtcDateTime)),this,SLOT(testSignal(UtcDateTime)));

//    ui->ganttWidget->addItems(generateTest());
}

void MainWindow::testMemoryPlanningWidget()
{

    ui->memoryPlanningWidget->setGridView();

    // Begin init values
    QList<MemoryPart> records;
    int memoryPeaceLength,spaceBetweenUnits;
    int vacantPos = 0;
    int id = 1;

    for(;;)
    {
        memoryPeaceLength = qrand()%100;
        spaceBetweenUnits = qrand()%15;

        MemoryPart newPeace;
        vacantPos+=spaceBetweenUnits;
        newPeace.setStart(vacantPos);
        vacantPos+=memoryPeaceLength;

        if(vacantPos>2000)
            break;

        newPeace.setFinish(vacantPos);
        vacantPos+=1;

        newPeace.setState(static_cast<MemoryPart::MemoryState>(qrand()%MemoryPart::MemoryState_count));
        if(newPeace.state()==MemoryPart::Empty)
            newPeace.setId(0);
        else
        {
            newPeace.setId(id++);
            records.push_back(newPeace);
        }
    }
    // end init values

    Memory kaMemory;
    kaMemory.init(records,2000);

    MGridScene * scene = ui->memoryPlanningWidget->gridScene();
    ui->memoryPlanningWidget->setMemory(kaMemory);
    scene->setLimits(200,1750);
    ui->memoryPlanningWidget->setShowButtons(true);
    scene->setSelectionMode(MGridScene::areaSelection);
}

void MainWindow::setInterval()
{
  UtcDateTime baseDt = ui->widget->minimumDt();
  UtcDateTime rez1 = baseDt.addSecs(ui->widgetIntervalSlider->beginHandle());

  UtcDateTime rez2 = baseDt.addSecs(ui->widgetIntervalSlider->endHandle());
  ui->widget->setIntervalSelectionByDates(rez1, rez2);
}

void MainWindow::on_checkBox_syncAxisX_toggled(bool checked)
{
   ui->widget_chartGroup->setSyncChartsByAxisX(checked);
}

void MainWindow::on_pushButton_clicked()
{
   QwtScaleDiv *div = ui->widget->getPlot()->axisScaleDiv(QwtPlot::xBottom);
   UtcDateTime begin(ChartWidget::pointToDt(QPointF(div->lowerBound(), 0)));
   UtcDateTime end(ChartWidget::pointToDt(QPointF(div->upperBound(), 0)));
   ui->widget->getPlot()->setAxisAutoScale(QwtPlot::xBottom, false);
   ui->widget->getPlot()->setAxisScale(QwtPlot::xBottom, ChartWidget::dtToPoint(begin.addSecs(20 * 60), 0).x(),
                                       ChartWidget::dtToPoint(end.addSecs(-20 * 60), 0).x());

   ui->widget->getPlot()->replot();
}

void MainWindow::on_checkBox_showLegend_toggled(bool checked)
{
  ui->widget->setShowLegend(checked);
}

void MainWindow::on_pushButton_ganttAddItems_clicked()
{
    ui->ganttWidget->addItems(generateTest());
}

void MainWindow::on_pushButton_ganttClear_clicked()
{
    ui->ganttWidget->clear();
}

void MainWindow::on_radioButton_ganttPlayer_toggled(bool checked)
{
    ui->ganttWidget->showPlayer(checked);
}

void MainWindow::testSignal(const UtcDateTime &dt)
{
    qDebug() << dt;
}
