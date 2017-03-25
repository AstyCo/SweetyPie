#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "utcdatetime.h"
#include "gantt/private_extensions/gantt-lib_global_values.h"
#include "gantttreemodel.h"
#include "modelwrapper.h"
#include "gantt/factory/ganttfactory.h"

#include "itemadditiondialog.h"
#include "additiongeneratedwizard.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <QKeyEvent>
#include <QWidgetAction>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

void MainWindow::init()
{
    _model = new GanttTreeModel();
    ui->treeView->setModel(_model);

    ui->ganttWidget->setFactory(new GanttFactory(new ModelWrapper(_model)));

    ui->ganttWidget->setView(ui->treeView);

    connect(ui->ganttWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(onCustomContextMenuRequested(QPoint)));

    setWindowIcon(QIcon(":/images/gantt-chart.png"));
}

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();



//    testGantt();
//    on_pushButton_ganttAddItems_clicked();
//    qDebug() << "test data initialized";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{

    QMainWindow::keyPressEvent(e);
}

void MainWindow::addBefore()
{
    if (_dlg->exec() == QDialog::Accepted) {
        _model->addBefore(produceItem(_dlg), _item);
    }
}

void MainWindow::addAfter()
{
    if (_dlg->exec() == QDialog::Accepted) {
        _model->addAfter(produceItem(_dlg), _item);
    }
}

void MainWindow::addInner()
{
    if (_dlg->exec() == QDialog::Accepted) {
        _model->addInner(produceItem(_dlg), _item);
    }
}

void MainWindow::removeItem()
{
    _model->removeItem(_item);
}

void MainWindow::addFirst()
{
    if (_dlg->exec() == QDialog::Accepted) {
        _model->addBefore(produceItem(_dlg), NULL);
    }
}

void MainWindow::addLast()
{
    if (_dlg->exec() == QDialog::Accepted) {
        _model->addAfter(produceItem(_dlg), NULL);
    }
}

void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
//    qDebug() << "onCustomContextMenuRequested";

//    qDebug() << "dtforpos" << ui->ganttWidget->dtForPos(pos);
//    qDebug() << "indexforpos" << ui->ganttWidget->indexForPos(pos);
    _item = NULL;
    ItemAdditionDialog dlg(this);
    dlg.moveStart(ui->ganttWidget->dtForPos(pos));
    _dlg = &dlg;

    UtcDateTime dt = ui->ganttWidget->dtForPos(pos);
    QModelIndex index = ui->ganttWidget->indexForPos(pos);

    QMenu contextMenu(tr("Context menu"), this);

    if (index.isValid()){
        _item = _model->itemForIndex(index);
        if (!_item) {
            qWarning("CRITICAL MainWindow::onCustomContextMenuRequested");
            return;
        }

        QLabel *titleLabel = new QLabel(QString::fromUtf8("<<%1>>").arg(_item->title()), &contextMenu);
        titleLabel->setMargin(5);
//        titleLabel->setStyleSheet("background-color: #ABABAB");
        QWidgetAction *titleAction = new QWidgetAction(&contextMenu);
        titleAction->setDefaultWidget(titleLabel);
        titleAction->setObjectName("titleAction");
        titleAction->setEnabled(false);
        contextMenu.addAction(titleAction);
        contextMenu.addSeparator();

        QAction action1(QString::fromUtf8("Добавить событие спереди"), this);
        connect(&action1, SIGNAL(triggered()), this, SLOT(addBefore()));
        contextMenu.addAction(&action1);

        QAction action2(QString::fromUtf8("Добавить событие после"), this);
        connect(&action2, SIGNAL(triggered()), this, SLOT(addAfter()));
        contextMenu.addAction(&action2);

        QAction action3(QString::fromUtf8("Добавить вложенное событие"), this);
        connect(&action3, SIGNAL(triggered()), this, SLOT(addInner()));
        contextMenu.addAction(&action3);

        QAction action4(QString::fromUtf8("Удалить событие"), this);
        connect(&action4, SIGNAL(triggered()), this, SLOT(removeItem()));
        contextMenu.addAction(&action4);


        contextMenu.exec(ui->ganttWidget->mapPosToGlobal(pos));
    }
    else {
        QAction action1(QString::fromUtf8("Добавить событие в начало"), this);
        connect(&action1, SIGNAL(triggered()), this, SLOT(addFirst()));
        contextMenu.addAction(&action1);

        QAction action2(QString::fromUtf8("Добавить событие в конец"), this);
        connect(&action2, SIGNAL(triggered()), this, SLOT(addLast()));
        contextMenu.addAction(&action2);
        contextMenu.exec(ui->ganttWidget->mapPosToGlobal(pos));
    }
}



void MainWindow::on_pushButton_ganttClear_clicked()
{
    _model->clear();
}

void MainWindow::on_radioButton_ganttPlayer_toggled(bool checked)
{
    ui->ganttWidget->setVisiblePlayer(checked);
}

void MainWindow::on_pushButton_ganttAddItems_clicked()
{
    AdditionGeneratedWizard wizard;
    if (wizard.exec() == QWizard::Accepted)
        _model->addItems(wizard.generatedItems);
}

GanttInfoItem *MainWindow::produceItem(const ItemAdditionDialog *dlg)
{
//    qDebug() << "isDot?" << dlg->isDot() << "duration" << dlg->finish() - dlg->start();
    return new GanttInfoItem(
                dlg->title(),
                dlg->start(),
                (dlg->isDot() ? TimeSpan() : dlg->finish() - dlg->start()),
//                QModelIndex(),
                dlg->color()
                );
}

void MainWindow::on_pushButton_ganttDump_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Сохранение модели"),
                                                    "",
                                                    "Gantt model dump (*.gmdd);;");
    if (!filename.isEmpty()) {
//        qDebug() << "dump fn" << filename;
        _model->serialize(filename);
    }
}

void MainWindow::on_pushButton_ganttLoad_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Загрузка модели"),
                                                    "./",
                                                    "Gantt model dump (*.gmdd);;");
    if (!filename.isEmpty()) {
//        qDebug() << "load fn" << filename;
        _model->deserialize(filename);
    }
}
