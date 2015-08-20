#include "ganttplayer.h"

#include <QIcon>
#include <QHBoxLayout>
#include <QDebug>

#include <QHash>






namespace {

QAction *createAction(const QString &icon, const QString &text,
        QObject *parent,
        const QKeySequence &shortcut=QKeySequence())
{
    QAction *action = new QAction(QIcon(icon), text, parent);
    if (!shortcut.isEmpty())
        action->setShortcut(shortcut);
    return action;
}

} // anonymous namespace

GanttPlayer::GanttPlayer(QWidget *parent) :
    QWidget(parent)
{
    m_timer = new QTimer(this);
    createActions();
    createMenusAndToolBar();
    createConnections();
}

GanttPlayer::~GanttPlayer()
{
    if(m_timer->isActive())
        m_timer->stop();
    delete m_slider;
    delete m_playerToolBar;
    delete m_spinBox;
    delete m_dtEdit;
}
QDateTime GanttPlayer::beginDt() const
{
    return m_dtEdit->minimumDateTime();
}

void GanttPlayer::setBeginDt(const QDateTime &beginDt)
{
    m_dtEdit->setMinimumDateTime(beginDt.toUTC());
    m_slider->setMinimum(beginDt.toTime_t());
}

QDateTime GanttPlayer::endDt() const
{
    return m_dtEdit->maximumDateTime();
}

void GanttPlayer::setEndDt(const QDateTime &endDt)
{
    m_dtEdit->setMaximumDateTime(endDt.toUTC());
    m_slider->setMaximum(endDt.toTime_t());
}
QDateTime GanttPlayer::currentDt() const
{
    return m_dtEdit->dateTime();
}

void GanttPlayer::setCurrentDt(const QDateTime &currentDT)
{
    m_dtEdit->setDateTime(currentDT);
}

QDateTimeEdit *GanttPlayer::dtEdit() const
{
    return m_dtEdit;
}

void GanttPlayer::createActions()
{
    m_playAction = createAction(":/images/play.png", tr("Play"), this);
    m_playAction->setCheckable(true);
    m_pauseAction = createAction(":/images/pause.png", tr("Pause"), this);
    m_stopAction = createAction(":/images/stop.png", tr("Stop"), this);
    m_playbackAction = createAction(":/images/playback.png", tr("Playback"), this);
    m_playbackAction->setCheckable(true);
    m_toBeginAction = createAction(":/images/to-begin.png", tr("To begin"), this);
    m_toEndAction = createAction(":/images/to-end.png", tr("To end"), this);

}

void GanttPlayer::createMenusAndToolBar()
{
    QHBoxLayout * hlayout = new QHBoxLayout;
    hlayout->setSpacing(0);
    hlayout->setMargin(0);

    QAction *emptyAction = 0;
    m_playerToolBar = new QToolBar(tr("Player"));


    foreach (QAction *action, QList<QAction*>()
            << m_toBeginAction << emptyAction
            << m_playAction << m_pauseAction << m_stopAction << m_playbackAction << emptyAction
            << m_toEndAction << emptyAction
             )

    {
        if (action == emptyAction) {
            m_playerToolBar->addSeparator();
            continue;
        }
            m_playerToolBar->addAction(action);
    }


    m_spinBox = new QSpinBox;
    m_spinBox->setMaximum(1000);
    m_spinBox->setMinimum(1);
    m_spinBox->setPrefix("x");
    m_spinBox->setMaximumHeight(m_playerToolBar->height());
    hlayout->addWidget(m_spinBox);
    hlayout->addWidget(m_playerToolBar, 0);
    m_dtEdit = new QDateTimeEdit;
    m_dtEdit->setMaximumHeight(m_playerToolBar->height());
    m_dtEdit->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
    m_dtEdit->setTimeSpec(Qt::UTC);
    hlayout->addWidget(m_dtEdit);
    QVBoxLayout * vlayout = new QVBoxLayout;
    m_slider = new QSlider(Qt::Horizontal);
    vlayout->addWidget(m_slider);
    vlayout->addLayout(hlayout);
    setLayout(vlayout);
}

void GanttPlayer::createConnections()
{
    connect(m_playAction,SIGNAL(toggled(bool)),this,SLOT(playerPlay(bool)));
    connect(m_playbackAction,SIGNAL(toggled(bool)),this,SLOT(playerPlayback(bool)));
    connect(m_pauseAction,SIGNAL(triggered()),this,SLOT(playerPause()));
    connect(m_stopAction,SIGNAL(triggered()),this,SLOT(playerStop()));

    connect(m_dtEdit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(dtEditSlot(QDateTime)));
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderSlot(int)));
    connect(m_spinBox,SIGNAL(valueChanged(int)),this,SLOT(spinBoxSlot(int)));
}


void GanttPlayer::dtEditSlot(const QDateTime &dt)
{
    m_slider->blockSignals(true);
    m_slider->setValue(dt.toTime_t());
    emit currentValueChanged(m_slider->value());
    emit currentDtChanged(dt);
    m_slider->blockSignals(false);

}

void GanttPlayer::sliderSlot(int value)
{
    m_dtEdit->blockSignals(true);
    m_dtEdit->setDateTime(QDateTime::fromTime_t(value).toUTC());
    emit currentValueChanged(value);
    emit currentDtChanged(m_dtEdit->dateTime());
    m_dtEdit->blockSignals(false);
}


void GanttPlayer::playerPlay(bool b)
{
    if(b)
     {
        m_playbackAction->setChecked(false);
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaybackSlot()));
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaySlot()));
        connect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaySlot()));
        m_timer->start(1000/m_spinBox->value());
    }
    else
    {
        m_timer->stop();
    }

}

void GanttPlayer::timerPlaySlot()
{
    if(m_slider->value() >= m_slider->maximum())
    {
        playerStop();
        return;
    }
    m_slider->setValue(m_slider->value()+1);
}

void GanttPlayer::playerPause()
{
    m_playAction->setChecked(false);
    m_playbackAction->setChecked(false);
}

void GanttPlayer::playerStop()
{
    m_playAction->setChecked(false);
    m_playbackAction->setChecked(false);
    m_dtEdit->setDateTime(m_dtEdit->minimumDateTime());
}

void GanttPlayer::playerPlayback(bool b)
{
    if(b)
    {
        m_playAction->setChecked(false);
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaySlot()));
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaybackSlot()));
        connect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaybackSlot()));
        m_timer->start(1000/m_spinBox->value());
    }
    else
    {
        m_timer->stop();
    }

}

void GanttPlayer::timerPlaybackSlot()
{
    if(m_slider->value() <= m_slider->minimum())
    {
        playerStop();
        return;
    }

    m_slider->setValue(m_slider->value()-1);
}

void GanttPlayer::spinBoxSlot(int speed)
{
    m_timer->setInterval(1000/speed);
}


