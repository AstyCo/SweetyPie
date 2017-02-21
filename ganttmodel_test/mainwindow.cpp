#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "utcdatetime.h"

#include <qwt/qwt_scale_div.h>
#include <qwt/qwt_scale_widget.h>
#include <qwt/qwt_plot_canvas.h>

#include "memoryplanningwidget.h"
#include "mgrid_scene.h"
#include "mline_scene.h"

#include "charttimexywidget.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->tabWidget_chartTimeXYWidget->setCurrentIndex(0);


  testChartWidget();
  testChartTimeXYWidget();
  testChartsGroupWidget();
  testChartXYGroupWidget();
  testMemoryPlanningWidget();
  testDtIntervalWidget();
  qDebug() << "test data initialized";
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::testChartWidget()
{
  srand(time(0));
  QVector<QPointF> data, data2;  
  for(int i = 0; i < 60; i++)
  {    
    data.append(QPointF(i * 60, (rand() % 100)));
  }

  for(int i = 60; i < 100; i++)
  {
    data2.append(QPointF((i * 65), (rand() % 1000) + 100));
  }

  ui->widget_chartXYWidget->getActionsToolBar()->setChartActions(QSet<ChartActions>()
        << caScale << caGrid << caMaxMinLines << caTimer << caSelectInterval << caSelectTarget
                                                   << caDetailsPanel << caSettingsDlg);

  ui->widget_chartXYWidget->setLeftAxis("111111", 0, 0);
  ui->widget_chartXYWidget->setRightAxis("222222", 0, 800);

  ui->widget_chartXYWidget->setCountLastPoints(111);

  ui->widget_chartXYWidget->addZone("zone 1", data[0].x(), data[30].x(), QColor(229, 229, 229), QColor(187, 187, 187));

  //ui->widget_chartXYWidget->addZone("zone 2", data[60].x(), data[80].x(),  QColor(229, 229, 229), QColor(187, 187, 187));

  ui->widget_chartXYWidget->setData("11111111111111", data);
  ui->widget_chartXYWidget->setData("22222222222222", data2, QwtPlot::yRight);
}


void MainWindow::testChartTimeXYWidget()
{
  srand(time(0));
  QVector<QPointF> data, data2;
  UtcDateTime baseDt = QDateTime::currentDateTime();
  for(int i = 0; i < 60; i++)
  {
    data.append(ChartTimeXYWidget::dtToPoint(baseDt.addSecs(i * 60), (rand() % 100)));
  }

  for(int i = 60; i < 100; i++)
  {
    data2.append(ChartTimeXYWidget::dtToPoint(baseDt.addSecs(i * 65), (rand() % 1000) + 100));
  }

  ui->widget_chartTimeXYWidget->getActionsToolBar()->setChartActions(QSet<ChartActions>()
        << caScale << caGrid << caMaxMinLines << caTimer << caSelectInterval << caSelectTarget
                                                   << caDetailsPanel << caSettingsDlg);
  ui->widget_chartTimeXYWidget->setLeftAxis("111111", 0, 0);
  ui->widget_chartTimeXYWidget->setRightAxis("222222", 0, 800);

  ui->widget_chartTimeXYWidget->setCountLastPoints(111);

  ui->widget_chartTimeXYWidget->addZone("zone 1", data[0].x(), data[30].x(), QColor(229, 229, 229), QColor(187, 187, 187));

  //ui->widget->addZone("zone 2", data[60].x(), data[80].x(),  QColor(229, 229, 229), QColor(187, 187, 187));

  ui->widget_chartTimeXYWidget->setData("11111111111111", data);
  ui->widget_chartTimeXYWidget->setData("22222222222222", data2, QwtPlot::yRight);
/*
  ui->widgetIntervalSlider->setLimits(0,TimeSpan(ui->widget->selectionModel()->maximumDt()
                                                 - ui->widget->selectionModel()->minimumDt()).totalSeconds());
                                                 */
  ui->checkBox_showLegend->setChecked(true);

  connect(ui->widgetIntervalSlider, SIGNAL(valueChanged(IntervalSlider::ClippedHandle, long long)), this, SLOT(setInterval()));


}

void MainWindow::testChartsGroupWidget()
{
  QVector<QPointF> data, data2, data3;

  for(int i = 0; i < 100; i++)
  {   
    data.append(QPointF(i, (rand() % 100)));
    data2.append(QPointF(i, (rand() % 100) * 100));
    data3.append(QPointF(i, (rand() % 100) * 1000));
  }

  ChartXYWidget *chart = new ChartXYWidget();
  chart->setLeftAxis("Test Chart 1 big title");
  chart->setData("chart 1", data);
  ui->widget_chartsGroupWidget->addChart(chart);

  chart = new ChartXYWidget();
  chart->setLeftAxis("Test Chart 2");
  chart->setData("chart 2", data2);
  ui->widget_chartsGroupWidget->addChart(chart);

  chart = new ChartXYWidget();
  chart->setLeftAxis("Test Chart 3 norm");
  chart->setData("chart 3", data3);
  ui->widget_chartsGroupWidget->addChart(chart);
  ui->widget_chartsGroupWidget->setSyncChartsByAxisX(false);
}


void MainWindow::testChartXYGroupWidget()
{
  QVector<QPointF> data, data2, data3;
  UtcDateTime baseDt = QDateTime::currentDateTime();
  for(int i = 0; i < 100; i++)
  {
    UtcDateTime time = baseDt.addSecs(i * 60);
    data.append(ChartTimeXYWidget::dtToPoint(time, (rand() % 100)));
    data2.append(ChartTimeXYWidget::dtToPoint(time, (rand() % 100) * 100));
    data3.append(ChartTimeXYWidget::dtToPoint(time, (rand() % 100) * 1000));
  }

  ChartTimeXYWidget *chart = new ChartTimeXYWidget();
  chart->setLeftAxis("Test Chart 1 big title");
  chart->setData("chart 1", data);
  ui->widget_chartXYGroupWidget->addChart(chart);

  chart = new ChartTimeXYWidget();
  chart->setLeftAxis("Test Chart 2");
  chart->setData("chart 2", data2);
  ui->widget_chartXYGroupWidget->addChart(chart);

  chart = new ChartTimeXYWidget();
  chart->setLeftAxis("Test Chart 3 norm");
  chart->setData("chart 3", data3);
  ui->widget_chartXYGroupWidget->addChart(chart);
  ui->widget_chartXYGroupWidget->setSyncChartsByAxisX(false);
  ui->widget_chartXYGroupWidget->setPanelSelectIntervalVisible(true);
}

void MainWindow::testMemoryPlanningWidget()
{

    ui->memoryPlanningWidget->setGridView();

    // Begin init values
    QList<QSharedPointer<KaMemoryPart> > records;
    int memoryPeaceLength,spaceBetweenUnits;
    int vacantPos = 0;
    int id = 1;
    int memLen = 4080;
    QSharedPointer<KaMemoryPart> last_part;

    for(;;)
    {
        memoryPeaceLength = qrand()%100;
        spaceBetweenUnits = qrand()%15;

        QSharedPointer<KaMemoryPart> newPeace(new KaMemoryPart());

        vacantPos+=spaceBetweenUnits;
        newPeace->setStart(vacantPos);
        vacantPos+=memoryPeaceLength;

        if(vacantPos>memLen)
            break;

        last_part=newPeace;

        newPeace->setLength(memoryPeaceLength);
        vacantPos+=1;

        newPeace->setState(static_cast<KaMemoryPart::KaMemoryState>(qrand()%KaMemoryPart::KaMemoryState_count));
        if(newPeace->state()==KaMemoryPart::Empty)
            newPeace->setId(0);
        else
        {
            newPeace->setId(id++);
            records.push_back(newPeace);
        }
    }
    // end init values


    QSharedPointer<KaMemory> kaMemory(new KaMemory());
    kaMemory->init(records,memLen);


    ui->memoryPlanningWidget->setMemory(kaMemory);

    ui->memoryPlanningWidget->setSelected(10,100);



    ui->memoryPlanningWidget->setShowButtons(true);
//    MGridScene * grisScene = ui->memoryPlanningWidget->gridScene();
//    grisScene->setLengthSelection(100);
    ui->memoryPlanningWidget->setSelectionMode(MGridScene::positionSelection);

    if(!last_part.isNull())
        ui->memoryPlanningWidget->hoverPart(*last_part);
}


void MainWindow::testDtIntervalWidget()
{
    ui->dtIntervalWidget->setMode(DtIntervalWidget::NegativeEnabled, false);
    ui->dtIntervalWidget->setFlags(DtIntervalWidget::DateTime|
                                   DtIntervalWidget::IntervalSlider|
                                   DtIntervalWidget::ToolButtonToDt);
    ui->dtIntervalWidget->setLimits(QDateTime::currentDateTime(),QDateTime::currentDateTime().addDays(300));

}

void MainWindow::setInterval()
{
  /*
  UtcDateTime baseDt = ui->widget->selectionModel()->minimumDt();
  UtcDateTime rez1 = baseDt.addSecs(ui->widgetIntervalSlider->beginHandle());

  UtcDateTime rez2 = baseDt.addSecs(ui->widgetIntervalSlider->endHandle());
  ui->widget->selectionModel()->setIntervalSelectionByDates(rez1, rez2);
  */
}

void MainWindow::on_checkBox_syncAxisX_toggled(bool checked)
{
   ui->widget_chartXYGroupWidget->setSyncChartsByAxisX(checked);
}

void MainWindow::on_pushButton_clicked()
{
   QwtScaleDiv *div = ui->widget_chartTimeXYWidget->getPlot()->axisScaleDiv(QwtPlot::xBottom);
   UtcDateTime begin(ChartTimeXYWidget::pointToDt(QPointF(div->lowerBound(), 0)));
   UtcDateTime end(ChartTimeXYWidget::pointToDt(QPointF(div->upperBound(), 0)));
   ui->widget_chartTimeXYWidget->getPlot()->setAxisAutoScale(QwtPlot::xBottom, false);
   ui->widget_chartTimeXYWidget->getPlot()->setAxisScale(QwtPlot::xBottom, ChartTimeXYWidget::dtToPoint(begin.addSecs(20 * 60), 0).x(),
                                       ChartTimeXYWidget::dtToPoint(end.addSecs(-20 * 60), 0).x());

   ui->widget_chartTimeXYWidget->getPlot()->replot();
}

void MainWindow::on_checkBox_showLegend_toggled(bool checked)
{
  ui->widget_chartTimeXYWidget->setShowLegend(checked);
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->memoryPlanningWidget->clear();
}

void MainWindow::on_pushButton_3_clicked()
{
    foreach(ChartXYWidget *cxy, ui->widget_chartXYGroupWidget->charts())
      qDebug() << cxy->getPlot()->canvas()->sizeHint();
}

void MainWindow::on_pushButton_4_clicked()
{
    MGridScene * grisScene = ui->memoryPlanningWidget->gridScene();
    grisScene->clearInteractiveUnit();
    qDebug()<<grisScene->startSelection();
    qDebug()<<grisScene->lengthSelection();
}
