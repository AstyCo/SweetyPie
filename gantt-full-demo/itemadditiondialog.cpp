#include "itemadditiondialog.h"
#include "ui_itemadditiondialog.h"

#include "utcdatetime.h"
#include "extensions/timespan.h"

#include <QColorDialog>


ItemAdditionDialog::ItemAdditionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemAdditionDialog),
    _settings("AstyCo", "QtGantt")
{
    ui->setupUi(this);

    ui->dateTimeEditBegin->setDateTime(QDateTime::currentDateTimeUtc());
    ui->dateTimeEditEnd->setDateTime((start() + TimeSpan(1,20,0)).dateTime());

    connect(ui->dateTimeEditBegin, SIGNAL(dateTimeChanged(QDateTime)),
            this, SLOT(onBeginChanged(QDateTime)));
    connect(ui->dateTimeEditEnd, SIGNAL(dateTimeChanged(QDateTime)),
            this, SLOT(onEndChanged(QDateTime)));

    loadSettings();
}

ItemAdditionDialog::~ItemAdditionDialog()
{
    saveSettings();

    delete ui;
}

void ItemAdditionDialog::moveStart(const UtcDateTime &dt)
{
    TimeSpan duration = finish() - start();
    ui->dateTimeEditBegin->blockSignals(true);
    ui->dateTimeEditEnd->blockSignals(true);

    ui->dateTimeEditBegin->setDateTime(dt.dateTime());
    ui->dateTimeEditEnd->setDateTime((dt + duration).dateTime());

    ui->dateTimeEditBegin->blockSignals(false);
    ui->dateTimeEditEnd->blockSignals(false);

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

    if (dlg.exec() == QDialog::Accepted)
        setColor(dlg.selectedColor());
}

void ItemAdditionDialog::on_checkBoxInterval_stateChanged(int arg1)
{
//    qDebug() << "state changed" << arg1;
    if (arg1)
        showFinish();
    else
        hideFinish();
}

void ItemAdditionDialog::saveSettings()
{
    // store settings
    _settings.beginGroup("QtGantt_demo");
        _settings.beginGroup("event_addition");
        {
            _settings.setValue("color", _color.name());
            _settings.setValue("type", ui->checkBoxInterval->isChecked());
            _settings.setValue("event_name", ui->lineEdit->text());
        }
        _settings.endGroup();
    _settings.endGroup();
}

void ItemAdditionDialog::loadSettings()
{
    // load settings
    _settings.beginGroup("QtGantt_demo");
        _settings.beginGroup("event_addition");
        {
            setColor(QColor(_settings.value("color", "#A96942").toString()));
            bool interval = _settings.value("type", true).toBool();
            if (interval) {
                ui->checkBoxInterval->setChecked(true);
                showFinish();
            }
            else {
                ui->checkBoxDot->setChecked(true);
                hideFinish();
            }
            ui->lineEdit->setPlaceholderText(_settings.value("event_name", QString::fromUtf8("Событие")).toString());
        }
        _settings.endGroup();
    _settings.endGroup();
}

void ItemAdditionDialog::setColor(const QColor &color)
{
    ui->pushButtonColor->setStyleSheet(QString("background-color: %1").arg(color.name()));
    _color = color;
}
