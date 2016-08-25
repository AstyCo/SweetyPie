#include "ganttplayercontrol.h"
#include "ui_mygantt_playercontrol.h"
#include "ganttplayersettings.h"

#include "ganttglobalvalues.h"

#include <QTimer>
#include <QAbstractButton>

#include <QDebug>

GanttPlayerControl::GanttPlayerControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GanttPlayerControl)
{
    ui->setupUi(this);

    m_settings = NULL;
    m_timer = std::make_pair<QTimer*,PlayMode>(NULL,PlayMode_count);
    m_speedModifier = 1.0;
    setStep(GANTTPLAYER_DEFAULT_STEP);
    setPlayFrequency(GANTTPLAYER_DEFAULT_FREQUENCY);


    ui->stop->setActiveIcon(QIcon(":/images/Buttons 64x64/stop_off_64.png"));
    ui->stop->setCheckedIcon(QIcon(":/images/Buttons 64x64/stop_on_64.png"));

    ui->fragmentBegin->setActiveIcon(QIcon(":/images/Buttons 64x64/begin_off_64.png"));
    ui->fragmentBegin->setCheckedIcon(QIcon(":/images/Buttons 64x64/begin_on_64.png"));

    ui->prevEvent->setActiveIcon(QIcon(":/images/Buttons 64x64/prev_off_64.png"));
    ui->prevEvent->setCheckedIcon(QIcon(":/images/Buttons 64x64/prev_on_64.png"));

    ui->stepBackward->setActiveIcon(QIcon(":/images/Buttons 64x64/back_step_off_64.png"));
    ui->stepBackward->setCheckedIcon(QIcon(":/images/Buttons 64x64/back_step_on_64.png"));

    ui->backward->setActiveIcon(QIcon(":/images/Buttons 64x64/back_off_64.png"));
    ui->backward->setCheckedIcon(QIcon(":/images/Buttons 64x64/back_on_64.png"));

    ui->pause->setActiveIcon(QIcon(":/images/Buttons 64x64/pause_off_64.png"));
    ui->pause->setCheckedIcon(QIcon(":/images/Buttons 64x64/pause_on_64.png"));

    ui->forward->setActiveIcon(QIcon(":/images/Buttons 64x64/forward_off_64.png"));
    ui->forward->setCheckedIcon(QIcon(":/images/Buttons 64x64/forward_on_64.png"));

    ui->stepForward->setActiveIcon(QIcon(":/images/Buttons 64x64/forward_step_off_64.png"));
    ui->stepForward->setCheckedIcon(QIcon(":/images/Buttons 64x64/forward_step_on_64.png"));

    ui->nextEvent->setActiveIcon(QIcon(":/images/Buttons 64x64/next_off_64.png"));
    ui->nextEvent->setCheckedIcon(QIcon(":/images/Buttons 64x64/next_on_64.png"));

    ui->fragmentEnd->setActiveIcon(QIcon(":/images/Buttons 64x64/end_off_64.png"));
    ui->fragmentEnd->setCheckedIcon(QIcon(":/images/Buttons 64x64/end_on_64.png"));

    ui->record->setActiveIcon(QIcon(":/images/Buttons 64x64/rec_off_64.png"));
    ui->record->setCheckedIcon(QIcon(":/images/Buttons 64x64/rec_on_64.png"));


}

GanttPlayerControl::~GanttPlayerControl()
{
    delete ui;
}
long long GanttPlayerControl::step() const
{
    return m_step;
}
void GanttPlayerControl::setStep(long long step)
{
    m_step = step;
}

void GanttPlayerControl::makeStepForward()
{
    emit makeStep(m_step);
}

void GanttPlayerControl::makeStepBackward()
{
    emit makeStep(-m_step);
}

void GanttPlayerControl::setDefaultPlaySpeed()
{
    setSpeedModifier(1.0);

}

void GanttPlayerControl::setX3PlaySpeed()
{
    setSpeedModifier(3.0);
}

void GanttPlayerControl::setX5PlaySpeed()
{
    setSpeedModifier(5.0);
}

void GanttPlayerControl::updateTimers()
{
    if(m_timer.first)
    {
        disconnect(m_timer.first,SIGNAL(timeout()));
        delete m_timer.first;
        m_timer.first = new QTimer();
        connect(m_timer.first,SIGNAL(timeout()),this,(m_timer.second == PlayForward)?(SLOT(makeStepForward()))
                                                                                   :(SLOT(makeStepBackward())));
        m_timer.first->start(msec());

    }
}

int GanttPlayerControl::msec()
{
    return _MILISECONDS_IN_SECOND / m_playFrequency / m_speedModifier;
}

void GanttPlayerControl::setSpeedModifier(const qreal &speedModifier)
{
    m_speedModifier = speedModifier;
    updateTimers();
}

void GanttPlayerControl::setSettings(GanttPlayerSettings *settings)
{

    if(!settings)
        return;
    m_settings = settings;

    connect(m_settings,SIGNAL(speedX1()),this,SLOT(setDefaultPlaySpeed()));
    connect(m_settings,SIGNAL(speedX3()),this,SLOT(setX3PlaySpeed()));
    connect(m_settings,SIGNAL(speedX5()),this,SLOT(setX5PlaySpeed()));
}

qreal GanttPlayerControl::playFrequency() const
{
    return m_playFrequency;
}

void GanttPlayerControl::setPlayFrequency(const qreal &playFrequency)
{
    m_playFrequency = playFrequency;
}


void GanttPlayerControl::on_fragmentBegin_clicked()
{
    emit goToViewStart();
}

void GanttPlayerControl::on_fragmentEnd_clicked()
{
    emit goToViewFinish();
}

void GanttPlayerControl::on_nextEvent_clicked()
{
    emit goToNextEventStart();
}

void GanttPlayerControl::on_prevEvent_clicked()
{
    emit goToPrevEventFinish();
}


void GanttPlayerControl::on_stepBackward_clicked()
{
    makeStepBackward();
}

void GanttPlayerControl::on_stepForward_clicked()
{
    makeStepForward();
}

void GanttPlayerControl::uncheckAll()
{
    foreach(PlayerControlButton* button, m_checkedButtons)
    {
        button->setChecked(false);
        m_checkedButtons.removeOne(button);
    }

    if(m_timer.first)
    {
        disconnect(m_timer.first,SIGNAL(timeout()));
        delete m_timer.first;
        m_timer.first = NULL;
    }
}

void GanttPlayerControl::on_forward_clicked(bool checked)
{
    uncheckAll();

    if(checked)
    {
        m_timer.first = new QTimer();
        connect(m_timer.first, SIGNAL(timeout()), this, SLOT(makeStepForward()));
        m_timer.first->start(msec());
        m_timer.second = PlayForward;

        PlayerControlButton *p_button = qobject_cast<PlayerControlButton*>(sender());
        if(p_button)
            m_checkedButtons.append(p_button);
    }
}

void GanttPlayerControl::on_backward_clicked(bool checked)
{
    uncheckAll();

    if(checked)
    {
        m_timer.first = new QTimer();
        connect(m_timer.first, SIGNAL(timeout()), this, SLOT(makeStepBackward()));
        m_timer.first->start(msec());
        m_timer.second = PlayBack;

        PlayerControlButton *p_button = qobject_cast<PlayerControlButton*>(sender());
        if(p_button)
            m_checkedButtons.append(p_button);
    }
}

void GanttPlayerControl::on_pause_clicked()
{
    uncheckAll();
}

void GanttPlayerControl::on_stop_clicked()
{
    uncheckAll();
    emit stop();
}
