#include "dtintervalwidget.h"
#include "ui_dtintervalwidget.h"

#include "timespan.h"
#include <QMenu>

DtIntervalWidget::DtIntervalWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DtIntervalWidget)
{
    ui->setupUi(this);
    setFlags(0x0);

    _dtMenu = new QMenu(this);
    ui->toolButtonToDt->setMenu(_dtMenu);

    ui->dateTimeEditBegin->setTimeSpec(Qt::UTC);
    ui->dateTimeEditEnd->setTimeSpec(Qt::UTC);

    ui->labelInterval->setMinimumWidth(135);
    ui->labelInterval->setMaximumWidth(135);


    connect(ui->dateTimeEditBegin,SIGNAL(dateTimeChanged(QDateTime)),this,SIGNAL(beginDtChanged(QDateTime)));
    connect(ui->dateTimeEditEnd,SIGNAL(dateTimeChanged(QDateTime)),this,SIGNAL(endDtChanged(QDateTime)));
    connect(ui->widgetSelectInterval,SIGNAL(beginMoved(long long)),this,SLOT(onBeginHandleMoved(long long)));
    connect(ui->widgetSelectInterval,SIGNAL(endMoved(long long)),this,SLOT(onEndHandleMoved(long long)));

    connect(this,SIGNAL(beginDtChanged(QDateTime)),this,SLOT(matchBegins(QDateTime)));
    connect(this,SIGNAL(endDtChanged(QDateTime)),this,SLOT(matchEnds(QDateTime)));



    createMenuSetDt();
}

void DtIntervalWidget::testDateTimeBegin()
{
    if(ui->dateTimeEditBegin->dateTime() > ui->dateTimeEditEnd->dateTime())
        ui->dateTimeEditEnd->setDateTime(ui->dateTimeEditBegin->dateTime());
}

void DtIntervalWidget::testDateTimeEnd()
{
    if(ui->dateTimeEditBegin->dateTime() > ui->dateTimeEditEnd->dateTime())
        ui->dateTimeEditBegin->setDateTime(ui->dateTimeEditEnd->dateTime());
}


void DtIntervalWidget::createMenuSetDt()
{
    _dtMenu->clear();

    QAction * a1 = new QAction(tr("Тек. ДВ"),this);
    connect(a1,SIGNAL(triggered()),this,SLOT(setCurrentDt()));
    _dtMenu->addAction(a1);

//    if(_nu != 0)
//    {
//        QAction * a2 = new QAction(tr("ДВ НУ"),this);
//        connect(a2,SIGNAL(triggered()),this,SLOT(setNuDt()));
//        _dtMenu->addAction(a2);
//    }

    QAction * a3 = new QAction(tr("На 3 дня"),this);
    connect(a3,SIGNAL(triggered()),this,SLOT(set3DaysIntervalFuture()));
    _dtMenu->addAction(a3);

    QAction * a4 = new QAction(tr("На неделю"),this);
    connect(a4,SIGNAL(triggered()),this,SLOT(setWeekIntervalFuture()));
    _dtMenu->addAction(a4);

    QAction * a5 = new QAction(tr("На месяц"),this);
    connect(a5,SIGNAL(triggered()),this,SLOT(setMounthIntervalFuture()));
    _dtMenu->addAction(a5);

    QAction * a6 = new QAction(tr("На год"),this);
    connect(a6,SIGNAL(triggered()),this,SLOT(setYearIntervalFuture()));
    _dtMenu->addAction(a6);

}

void DtIntervalWidget::setCurrentDt()
{
    int msecTo = ui->dateTimeEditBegin->dateTime().secsTo(ui->dateTimeEditEnd->dateTime());
    ui->dateTimeEditBegin->setDateTime(QDateTime::currentDateTime().toUTC());
    ui->dateTimeEditEnd->setDateTime(ui->dateTimeEditBegin->dateTime().addSecs(msecTo));
}

void DtIntervalWidget::setYearIntervalFuture()
{
    QDate d = ui->dateTimeEditBegin->date();
    ui->dateTimeEditEnd->setDateTime(ui->dateTimeEditBegin->dateTime().addDays(d.daysInYear()));
}

void DtIntervalWidget::setMounthIntervalFuture()
{
    QDate d = ui->dateTimeEditBegin->date();
    ui->dateTimeEditEnd->setDateTime(ui->dateTimeEditBegin->dateTime().addDays(d.daysInMonth()));
}

void DtIntervalWidget::setWeekIntervalFuture()
{
    ui->dateTimeEditEnd->setDateTime(ui->dateTimeEditBegin->dateTime().addDays(7));
}

void DtIntervalWidget::set3DaysIntervalFuture()
{
    ui->dateTimeEditEnd->setDateTime(ui->dateTimeEditBegin->dateTime().addDays(3));
}



DtIntervalWidget::~DtIntervalWidget()
{
    delete ui;
}

void DtIntervalWidget::hideAll()
{
    QList<QWidget*> childWidgets = findChildren<QWidget*>();
    foreach(QWidget* widget, childWidgets)
        if (widget->parentWidget() == this)
            widget->hide();
}

void DtIntervalWidget::setFlags(int fl)
{
    m_flags = fl;

    showIntervalSlider(m_flags&IntervalSlider);
    showDateTimeWidgets(m_flags&DateTime);
    showToolButtonToDt(m_flags&ToolButtonToDt);
}

void DtIntervalWidget::blockAllSignals(bool val)
{
    blockSignals(val);
    ui->dateTimeEditBegin->blockSignals(val);
    ui->dateTimeEditEnd->blockSignals(val);
    ui->widgetSelectInterval->blockSignals(val);
}

void DtIntervalWidget::setLimits(const QDateTime &minDt, const QDateTime& maxDt)
{
    setMinimum(minDt);
    setMaximum(maxDt);

    ui->widgetSelectInterval->setLimits(minDt.toTime_t(),maxDt.toTime_t());
}

QDateTime DtIntervalWidget::minimumDtBegin() const
{
    return ui->dateTimeEditBegin->minimumDateTime();
}

QDateTime DtIntervalWidget::minimumDtEnd() const
{
    return ui->dateTimeEditEnd->minimumDateTime();
}

QDateTime DtIntervalWidget::maximumDtBegin() const
{
    return ui->dateTimeEditBegin->maximumDateTime();
}

QDateTime DtIntervalWidget::maximumDtEnd() const
{
    return ui->dateTimeEditEnd->maximumDateTime();
}

QDateTime DtIntervalWidget::beginDt() const
{
    return ui->dateTimeEditBegin->dateTime();
}

QDateTime DtIntervalWidget::endDt() const
{
    return ui->dateTimeEditEnd->dateTime();
}

long DtIntervalWidget::beginHandle() const
{
    return ui->widgetSelectInterval->beginHandle();
}

long DtIntervalWidget::endHandle() const
{
    return ui->widgetSelectInterval->endHandle();
}

long long DtIntervalWidget::dtToSliderValue(const QDateTime &dt)
{
    return dt.toTime_t();
}

void DtIntervalWidget::showDateTimeWidgets(bool show)
{
    ui->dateLabel->setVisible(show);
    ui->dateTimeEditBegin->setVisible(show);
    ui->betweenDateLabel->setVisible(show);
    ui->dateTimeEditEnd->setVisible(show);
    ui->toolButtonToDt->setVisible(show);
    ui->labelInterval->setVisible(show);
}

void DtIntervalWidget::showIntervalSlider(bool show)
{
    ui->widgetSelectInterval->setVisible(show);
}

void DtIntervalWidget::showToolButtonToDt(bool show)
{
    ui->toolButtonToDt->setVisible(show);
}

void DtIntervalWidget::onBeginHandleMoved(long long val)
{
    if(beginDt() == QDateTime::fromTime_t(val))
        return;
    emit beginDtChanged(QDateTime::fromTime_t(val).toUTC());
}

void DtIntervalWidget::onEndHandleMoved(long long val)
{
    if(endDt() == QDateTime::fromTime_t(val))
        return;

    emit endDtChanged(QDateTime::fromTime_t(val).toUTC());
}

void DtIntervalWidget::updateTimespanLabel()
{
    if(ui->dateTimeEditEnd->dateTime().isValid() && ui->dateTimeEditBegin->dateTime().isValid())
    {
        TimeSpan ts(ui->dateTimeEditBegin->dateTime().secsTo(ui->dateTimeEditEnd->dateTime()),TimeSpan::tiSeconds);
        ui->labelInterval->setText(ts.toString(0,true));
    }
}

void DtIntervalWidget::setBeginDt(const QDateTime &dt)
{
    if(dt == ui->dateTimeEditBegin->dateTime())
        return;    
    ui->dateTimeEditBegin->setDateTime(dt);
    emit beginDtChanged(beginDt());
}

void DtIntervalWidget::setEndDt(const QDateTime &dt)
{
    if(dt == ui->dateTimeEditEnd->dateTime())
        return;
    ui->dateTimeEditEnd->setDateTime(dt);
    emit endDtChanged(endDt());
}

void DtIntervalWidget::setMinimum(const QDateTime &minDt)
{
    if(minDt == ui->dateTimeEditBegin->minimumDateTime()
            && minDt == ui->dateTimeEditEnd->minimumDateTime())
        return;
    ui->dateTimeEditBegin->setMinimumDateTime(minDt);
    ui->dateTimeEditEnd->setMinimumDateTime(minDt);

}

void DtIntervalWidget::setMaximum(const QDateTime &maxDt)
{
    if(maxDt == ui->dateTimeEditBegin->maximumDateTime()
            && maxDt == ui->dateTimeEditEnd->maximumDateTime())
        return;

    ui->dateTimeEditBegin->setMaximumDateTime(maxDt);
    ui->dateTimeEditEnd->setMaximumDateTime(maxDt);
}

void DtIntervalWidget::setBeginHandle(long val)
{
    if(val == ui->widgetSelectInterval->beginHandle())
        return;
    ui->widgetSelectInterval->setBeginHandle(val);
}

void DtIntervalWidget::setEndHandle(long val)
{
    if(val == ui->widgetSelectInterval->endHandle())
        return;
    ui->widgetSelectInterval->setEndHandle(val);
}

void DtIntervalWidget::matchBegins(const QDateTime &dt)
{
    if(!dt.isValid())
        return;
    blockAllSignals(true);

    setBeginDt(dt);
    setBeginHandle(dtToSliderValue(dt));
    updateTimespanLabel();

    blockAllSignals(false);
}

void DtIntervalWidget::matchEnds(const QDateTime &dt)
{
    if(!dt.isValid())
        return;
    blockAllSignals(true);

    setEndDt(dt);
    setEndHandle(dtToSliderValue(dt));
    updateTimespanLabel();

    blockAllSignals(false);
}
