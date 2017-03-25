#include "itemadditiondialog.h"
#include "ui_itemadditiondialog.h"

#include "utcdatetime.h"
#include "extensions/timespan.h"

#include <QColorDialog>


ItemAdditionDialog::ItemAdditionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemAdditionDialog)
{
    ui->setupUi(this);



    ui->dateTimeEditBegin->setDateTime(QDateTime::currentDateTimeUtc());
    ui->dateTimeEditEnd->setDateTime(UtcDateTime(QDateTime::currentDateTimeUtc() + TimeSpan(1, 10, 0)).dateTime());

    connect(ui->dateTimeEditBegin, SIGNAL(dateTimeChanged(QDateTime)),
            this, SLOT(onBeginChanged(QDateTime)));
    connect(ui->dateTimeEditEnd, SIGNAL(dateTimeChanged(QDateTime)),
            this, SLOT(onEndChanged(QDateTime)));

    hideFinish();
}

ItemAdditionDialog::~ItemAdditionDialog()
{
    delete ui;
}

void ItemAdditionDialog::setStart(const UtcDateTime &dt)
{
    ui->dateTimeEditBegin->setDateTime(dt.dateTime());
}

QString ItemAdditionDialog::title() const
{
    return ui->lineEdit->text();
}

QColor ItemAdditionDialog::color() const
{
    return _color;
}

bool ItemAdditionDialog::isDot() const
{
    return ui->checkBoxDot->isChecked();
}

UtcDateTime ItemAdditionDialog::start() const
{
    return ui->dateTimeEditBegin->dateTime();
}

UtcDateTime ItemAdditionDialog::finish() const
{
    return ui->dateTimeEditEnd->dateTime();
}

void ItemAdditionDialog::onBeginChanged(const QDateTime &dt)
{
    if (dt > ui->dateTimeEditEnd->dateTime())
        ui->dateTimeEditEnd->setDateTime(dt);
}

void ItemAdditionDialog::onEndChanged(const QDateTime &dt)
{
    if (dt < ui->dateTimeEditBegin->dateTime())
        ui->dateTimeEditBegin->setDateTime(dt);
}

void ItemAdditionDialog::hideFinish()
{
    ui->labelEnd->hide();
    ui->dateTimeEditEnd->hide();
}

void ItemAdditionDialog::showFinish()
{
    ui->labelEnd->show();
    ui->dateTimeEditEnd->show();
}

void ItemAdditionDialog::on_pushButtonColor_clicked()
{
    QColorDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        qDebug() << "setstylesheet" << QString("background-color: %1").arg(dlg.selectedColor().name());
        ui->pushButtonColor->setStyleSheet(QString("background-color: %1").arg(dlg.selectedColor().name()));
        _color = dlg.selectedColor();
    }
}

void ItemAdditionDialog::on_checkBoxInterval_stateChanged(int arg1)
{
    qDebug() << "state changed" << arg1;
    if (arg1)
        showFinish();
    else
        hideFinish();
}
