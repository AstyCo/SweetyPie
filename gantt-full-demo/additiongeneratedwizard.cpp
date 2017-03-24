#include "additiongeneratedwizard.h"
#include "ui_additiongeneratedwizard.h"

#include "extensions/utcdatetime.h"
#include "extensions/timespan.h"
#include "extensions/timespan_extension.h"

#include <QIntValidator>

#define INTRO_PAGE 0
#define GENERATE_PAGE 1

AdditionGeneratedWizard::AdditionGeneratedWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::AdditionGeneratedWizard)
{
    ui->setupUi(this);

    ui->lineEditEventCount->setValidator(new QIntValidator(1, 10000, ui->lineEditEventCount));
    ui->lineEditMaxNest->setValidator(new QIntValidator(1, 10, ui->lineEditMaxNest));
}

AdditionGeneratedWizard::~AdditionGeneratedWizard()
{
    delete ui;
}

//int AdditionGeneratedWizard::nextId() const
//{
//    if (currentId() == INTRO_PAGE) {
//        if (ui->checkBoxGenerate->isChecked())
//            return 1;
//    }
//    return QWizard::nextId();
//}

void AdditionGeneratedWizard::initializePage(int id)
{
    QWizard::initializePage(id);

    if (currentId() == GENERATE_PAGE) {
        if (!ui->checkBoxGenerate->isChecked())
            done(QWizard::Accepted);

        ui->dateTimeEditMin->setDateTime(QDateTime::currentDateTimeUtc());
        ui->dateTimeEditMax->setDateTime(UtcDateTime(QDateTime::currentDateTimeUtc() + TimeSpan(1, 10, 0)).dateTime());
    }
}

void AdditionGeneratedWizard::done(int result)
{
    if (ui->checkBoxGenerate->isChecked())
        addGenerated();
    else
        addTest();

    QWizard::done(result);
}

void AdditionGeneratedWizard::addTest()
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
                        "leafButNode6.0",
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
    generatedItems = testList;
}

void AdditionGeneratedWizard::addGenerated()
{
    static long eventNumber = 0;
    int cnt = ui->lineEditEventCount->text().toInt();
    int nestMax = ui->lineEditMaxNest->text().toInt();

    UtcDateTime start(ui->dateTimeEditMin->dateTime());
    TimeSpan ts = ui->dateTimeEditMax->dateTime() - ui->dateTimeEditMin->dateTime();


    GanttInfoItem *item = NULL;
    int nest = qrand() % (nestMax+1);
    while (cnt-- > 0) {
        qreal coef1 = (qrand() % 400) * 1. / 400;
        qreal coef2 = (qrand() % 400) * 1. / 400;
        const qreal &minCoef = (coef1>coef2 ? coef2 : coef1);
        const qreal &maxCoef = (coef1>coef2 ? coef1 : coef2);
        bool isDot = maxCoef - minCoef < 0.05;
//        qDebug() << (isDot ? "TRUE":"FALSE");
        GanttInfoItem *newItem = new GanttInfoItem(
                        QString::fromUtf8("событие %1").arg(++eventNumber),
                        start + minCoef * ts,
                        (isDot ? TimeSpan() : ts * (maxCoef-minCoef)),
                        QModelIndex(),
                        QColor::fromRgb(qrand()%255,qrand()%255,qrand()%255)
                        );


        bool fl = qrand() % 2;

        if (item)
            item->append(newItem);
        else
            generatedItems.append(newItem);

        if (fl) {
            if (nest == 0) {
                item = NULL;
                nest = qrand() % (nestMax+1);
            }
            else {
                item = newItem;
                --nest;
            }
        }
    }
}
