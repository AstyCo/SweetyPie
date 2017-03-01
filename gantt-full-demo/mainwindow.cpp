#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <QKeyEvent>
#include "utcdatetime.h"
#include "gantt/private_extensions/gantt-lib_global_values.h"
#include "gantttreemodel.h"
#include "modelwrapper.h"
#include "gantt/factory/ganttfactory.h"


void MainWindow::init()
{
    _model = new GanttTreeModel();
    ui->treeView->setModel(_model);

    ui->ganttWidget->setFactory(new GanttFactory(new ModelWrapper(_model)));

//    ui->treeView->set

//    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
//    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->ganttWidget->setView(ui->treeView);
}

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();

    testGantt();
//    on_pushButton_ganttAddItems_clicked();
    qDebug() << "test data initialized";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{

    QMainWindow::keyPressEvent(e);
}

void MainWindow::testGantt()
{
    QList<GanttInfoItem*> testList;

//! [0] : add only leaf
    GanttInfoItem *i0 = new GanttInfoItem(
                "leaf0",
                QDateTime::currentDateTime(),
                TimeSpan(1,10,12,13)
                );
//! [0]

//! [1]
    GanttInfoItem *i1 = new GanttInfoItem(
                "node1",
                QDateTime::currentDateTime()
                );
    GanttInfoItem   *i10 = new GanttInfoItem(
                            "leaf1.0",
                            UtcDateTime(2017,02,03,15,33,10),
                            TimeSpan(1,10,12,13)
                            ),
                    *i11 = new GanttInfoItem(
                            "leaf1.1",
                            UtcDateTime(2017,02,05,15,33,10),
                            TimeSpan(1,10,12,13)
                            ),
                    *i12 = new GanttInfoItem(
                            "leaf1.2",
                            UtcDateTime(2017,02,04,15,33,10),
                            TimeSpan(1,10,12,13)
                            );
    i1->append(i10);
    i1->append(i11);
    i1->append(i12);
//! [1]

//! [2]
    GanttInfoItem *i2 = new GanttInfoItem(
                "node2",
                QDateTime::currentDateTime()
                );
    GanttInfoItem   *i20 = new GanttInfoItem(
                            "leaf2.0",
                            UtcDateTime(QDateTime::currentDateTime()) - TimeSpan(4,0,0,0),
                            TimeSpan(3,10,12,13)
                            ),
                    *i21 = new GanttInfoItem(
                            "leaf2.1",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(1,10,12,13)
                            );
    i2->append(i20);
    i2->append(i21);
//! [2]

//! [3]
    GanttInfoItem *i3 = new GanttInfoItem(
                "node3",
                QDateTime::currentDateTime() + TimeSpan(1,12,0,0)
                );
    GanttInfoItem   *i30 = new GanttInfoItem(
                            "leaf3.0",
                            UtcDateTime(QDateTime::currentDateTime()) - TimeSpan(1,0,0,0),
                            TimeSpan(0,0,0,1)
                            ),
                    *i31 = new GanttInfoItem(
                            "leaf3.1",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(0,0,0,1)
                            ),
                    *i32 = new GanttInfoItem(
                            "leaf3.2",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(2,0,0,0),
                            TimeSpan(0,0,0,1)
                            ),
                    *i33 = new GanttInfoItem(
                            "leaf3.3",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(0,12,0,0),
                            TimeSpan(3,0,0,1)
                            );
    i3->append(i30);i3->append(i31);i3->append(i32);i3->append(i33);
//! [3]

//! [4]
    GanttInfoItem *i4 = new GanttInfoItem(
                "node4",
                QDateTime::currentDateTime()
                );
    GanttInfoItem   *i40 = new GanttInfoItem(
                            "leaf4.0",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(0,0,0,1)
                            ),
                    *i41 = new GanttInfoItem(
                            "leaf4.1",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(0,0,0,1)
                            ),
                    *i42 = new GanttInfoItem(
                            "leaf4.2",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(0,0,0,1)
                            );
    i4->append(i40);i4->append(i41);i4->append(i42);
//! [4]

//! [5]
    GanttInfoItem *i5 = new GanttInfoItem(
                "node5",
                QDateTime::currentDateTime()
                );
    GanttInfoItem   *i50 = new GanttInfoItem(
                            "leaf5.0",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(2,0,0,0)
                            ),
                    *i51 = new GanttInfoItem(
                            "leaf5.1",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(2,0,0,0)
                            ),
                    *i52 = new GanttInfoItem(
                            "leaf5.2",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(2,0,0,0)
                            );
    i5->append(i50);i5->append(i51);i5->append(i52);
//! [5]

//! [6]
    GanttInfoItem   *i6 = new GanttInfoItem(
                        "node6",
                        QDateTime::currentDateTime()
                        );
    GanttInfoItem   *i60 = new GanttInfoItem(
                        "node6.0",
                        QDateTime::currentDateTime() + TimeSpan(1, 0, 0, 0)
                        );
    GanttInfoItem   *i600 = new GanttInfoItem(
                        "leaf6.0.0",
                        QDateTime::currentDateTime() + TimeSpan(2, 0, 0, 0)
                        );
    GanttInfoItem   *i6000 = new GanttInfoItem(
                        "leaf6.0.0.0",
                        UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(0,0,0,0),
                        TimeSpan(2,0,0,0)
                        );
    GanttInfoItem   *i6001 = new GanttInfoItem(
                        "leaf6.0.0.1",
                        UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                        TimeSpan(2,0,0,0)
                        );
    GanttInfoItem   *i6002 = new GanttInfoItem(
                        "leaf6.0.0.2",
                        UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(2,0,0,0),
                        TimeSpan(2,0,0,0)
                        );
    i6->append(i60);
    i60->append(i600);
    i600->append(i6000);i600->append(i6001);i600->append(i6002);
//! [6]
    // add to model
    testList << i0 << i1 << i2 << i3 << i4 << i5 << i6;
    _model->addItems(testList);
//    _model->printTree();
}

QList<GanttInfoItem*> generateTest()
{
    static int kk = 0;
    QList<GanttInfoItem*> testList;
    for(int i = kk; i<80 + kk; ++i)
    {
        GanttInfoItem *node = new GanttInfoItem;

        node->setTitle("node"+QString::number(i));
        int year = 2014 + qrand()%2,
                month = 1 + qrand()%3,
                day = 1 + qrand()%2,
                hour = qrand()%2,
                minute = qrand()%60,
                sec = qrand()%60,
                microsec = qrand()%1000000;

        node->setStart(UtcDateTime(year,month,day,hour,minute,sec,microsec));

        QColor color = QColor::fromRgb(qrand()%255,qrand()%255,qrand()%255);
        int max = qrand()%4;

        for(int j = 0; j<max; ++j)
        {
            GanttInfoItem *leaf = new GanttInfoItem;

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
                        .addDays(day )
                        .addMonths(month );

            leaf->setStart(start);
            leaf->setTimeSpan(finish - start);
            leaf->setColor(color);
            leaf->setTitle("leaf"+QString::number(1 + i) + ':' + QString::number(1 + j));

            node->append(leaf);
        }


        testList.append(node);
    }
    kk+=10;
    return testList;
}

void MainWindow::on_pushButton_ganttAddItems_clicked()
{
    _model->addItems(generateTest());
//    _model->printTree();
}

void MainWindow::on_pushButton_ganttClear_clicked()
{
    _model->clear();
}

void MainWindow::on_radioButton_ganttPlayer_toggled(bool checked)
{
    ui->ganttWidget->setVisiblePlayer(checked);
}


void MainWindow::on_pushButton_clicked()
{
    testGantt();
}
