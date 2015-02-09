#include "ganttwidget.h"
#include "ui_ganttwidget.h"

GanttWidget::GanttWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GanttWidget)
{
    ui->setupUi(this);
}

GanttWidget::~GanttWidget()
{
    delete ui;
}
