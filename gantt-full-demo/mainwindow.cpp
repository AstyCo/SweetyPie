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
    GanttInfoLeaf *i0 = new GanttInfoLeaf(
                "leaf0",
                QDateTime::currentDateTime(),
                TimeSpan(1,10,12,13)
                );
//! [0]

//! [1]
    GanttInfoNode *i1 = new GanttInfoNode(
                "node1",
                QDateTime::currentDateTime()
                );
    GanttInfoLeaf   *i10 = new GanttInfoLeaf(
                            "leaf1.0",
                            UtcDateTime(2017,02,03,15,33,10),
                            TimeSpan(1,10,12,13)
                            ),
                    *i11 = new GanttInfoLeaf(
                            "leaf1.1",
                            UtcDateTime(2017,02,05,15,33,10),
                            TimeSpan(1,10,12,13)
                            ),
                    *i12 = new GanttInfoLeaf(
                            "leaf1.2",
                            UtcDateTime(2017,02,04,15,33,10),
                            TimeSpan(1,10,12,13)
                            );
    i1->append(i10);
    i1->append(i11);
    i1->append(i12);
//! [1]

//! [2]
    GanttInfoNode *i2 = new GanttInfoNode(
                "node2",
                QDateTime::currentDateTime()
                );
    GanttInfoLeaf   *i20 = new GanttInfoLeaf(
                            "leaf2.0",
                            UtcDateTime(QDateTime::currentDateTime()) - TimeSpan(4,0,0,0),
                            TimeSpan(3,10,12,13)
                            ),
                    *i21 = new GanttInfoLeaf(
                            "leaf2.1",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(1,10,12,13)
                            );
    i2->append(i20);
    i2->append(i21);
//! [2]

//! [3]
    GanttInfoNode *i3 = new GanttInfoNode(
                "node3",
                QDateTime::currentDateTime() + TimeSpan(1,12,0,0)
                );
    GanttInfoLeaf   *i30 = new GanttInfoLeaf(
                            "leaf3.0",
                            UtcDateTime(QDateTime::currentDateTime()) - TimeSpan(1,0,0,0),
                            TimeSpan(0,0,0,1)
                            ),
                    *i31 = new GanttInfoLeaf(
                            "leaf3.1",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(0,0,0,1)
                            ),
                    *i32 = new GanttInfoLeaf(
                            "leaf3.2",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(2,0,0,0),
                            TimeSpan(0,0,0,1)
                            ),
                    *i33 = new GanttInfoLeaf(
                            "leaf3.3",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(0,12,0,0),
                            TimeSpan(3,0,0,1)
                            );
    i3->append(i30);i3->append(i31);i3->append(i32);i3->append(i33);
//! [3]

//! [4]
    GanttInfoNode *i4 = new GanttInfoNode(
                "node4",
                QDateTime::currentDateTime()
                );
    GanttInfoLeaf   *i40 = new GanttInfoLeaf(
                            "leaf4.0",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(0,0,0,1)
                            ),
                    *i41 = new GanttInfoLeaf(
                            "leaf4.1",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(0,0,0,1)
                            ),
                    *i42 = new GanttInfoLeaf(
                            "leaf4.2",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(0,0,0,1)
                            );
    i4->append(i40);i4->append(i41);i4->append(i42);
//! [4]

//! [5]
    GanttInfoNode *i5 = new GanttInfoNode(
                "node5",
                QDateTime::currentDateTime()
                );
    GanttInfoLeaf   *i50 = new GanttInfoLeaf(
                            "leaf5.0",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(2,0,0,0)
                            ),
                    *i51 = new GanttInfoLeaf(
                            "leaf5.1",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(2,0,0,0)
                            ),
                    *i52 = new GanttInfoLeaf(
                            "leaf5.2",
                            UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                            TimeSpan(2,0,0,0)
                            );
    i5->append(i50);i5->append(i51);i5->append(i52);
//! [5]

//! [6]
    GanttInfoNode   *i6 = new GanttInfoNode(
                        "node6",
                        QDateTime::currentDateTime()
                        );
    GanttInfoNode   *i60 = new GanttInfoNode(
                        "node6.0",
                        QDateTime::currentDateTime() + TimeSpan(1, 0, 0, 0)
                        );
    GanttInfoNode   *i600 = new GanttInfoNode(
                        "leaf6.0.0",
                        QDateTime::currentDateTime() + TimeSpan(2, 0, 0, 0)
                        );
    GanttInfoLeaf   *i6000 = new GanttInfoLeaf(
                        "leaf6.0.0.0",
                        UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(0,0,0,0),
                        TimeSpan(2,0,0,0)
                        );
    GanttInfoLeaf   *i6001 = new GanttInfoLeaf(
                        "leaf6.0.0.1",
                        UtcDateTime(QDateTime::currentDateTime()) + TimeSpan(1,0,0,0),
                        TimeSpan(2,0,0,0)
                        );
    GanttInfoLeaf   *i6002 = new GanttInfoLeaf(
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
}

QList<GanttInfoItem*> generateTest()
{
    static int kk = 0;
    QList<GanttInfoItem*> testList;
    for(int i = kk; i<80 + kk; ++i)
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

        node->setStart(UtcDateTime(year,month,day,hour,minute,sec,microsec));

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
