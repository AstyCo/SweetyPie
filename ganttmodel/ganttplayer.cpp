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
    m_speed = 1;
    m_secWidth = 0.2;
    m_timeInc = 10; //msec
    m_playerBeginValue = 0;
    m_playerCurrentValue = m_playerBeginValue;
    m_timer = new QTimer();
    m_playIsClicked = false;
    m_playbackIsClicked = false;
    createActions();
    createMenusAndToolBar();
    createConnections();
}

GanttPlayer::~GanttPlayer()
{
    delete m_slider;
    delete m_playerToolBar;
    delete m_spinBox;
    delete m_dtEdit;
}
QDateTime GanttPlayer::seedDT() const
{
    return m_seedDT;
}

void GanttPlayer::setSeedDT(const QDateTime &seedDT)
{
    m_seedDT = seedDT;
    m_currentDT = m_seedDT;
    m_dtEdit->setDateTime(m_seedDT);
}
qreal GanttPlayer::playerEndValue() const
{
    return m_playerEndValue;
}

void GanttPlayer::setPlayerEndValue(const qreal &playerEndValue)
{
    m_playerEndValue = playerEndValue;
}
QDateTime GanttPlayer::endDT() const
{
    return m_endDT;
}

void GanttPlayer::setEndDT(const QDateTime &endDT)
{
    m_endDT = endDT;
    m_playerEndValue = (m_seedDT.msecsTo(m_endDT)*m_secWidth)/m_timeInc;
}
QDateTime GanttPlayer::currentDT() const
{
    return m_currentDT;
}

void GanttPlayer::setCurrentDT(const QDateTime &currentDT)
{
    m_currentDT = currentDT;
}



void GanttPlayer::createActions()
{
    m_playAction = createAction(":/images/play.png", tr("Play"), this);
    m_pauseAction = createAction(":/images/pause.png", tr("Pause"), this);
    m_stopAction = createAction(":/images/stop.png", tr("Stop"), this);
    m_playbackAction = createAction(":/images/playback.png", tr("Playback"), this);
    m_stepBackAction = createAction(":/images/step-back.png", tr("Step back"), this);
    m_stepForwardAction = createAction(":/images/step-forward.png", tr("Step forward"), this);
    m_listFirstAction = createAction(":/images/list-first.png", tr("List first"), this);
    m_listLastAction = createAction(":/images/list-last.png", tr("List last"), this);
    m_toBeginAction = createAction(":/images/to-begin.png", tr("To begin"), this);
    m_toEndAction = createAction(":/images/to-end.png", tr("To end"), this);
    m_repeatAction = createAction(":/images/repeat.png", tr("Repeat"), this);
    m_recAction = createAction(":/images/rec.png", tr("Record"), this);
    m_turnOffAction = createAction(":/images/turn-off.png", tr("Turn off"), this);
    m_nextAction = createAction(":/images/next.png", tr("Next"), this);
    m_previousAction = createAction(":/images/previous.png", tr("Previous"), this);
    m_pullAction = createAction(":/images/pull.png", tr("Pull"), this);

}

void GanttPlayer::createMenusAndToolBar()
{
    QHBoxLayout * hlayout = new QHBoxLayout;
    hlayout->setSpacing(0);
    hlayout->setMargin(0);

    QAction *emptyAction = 0;
    m_playerToolBar = new QToolBar(tr("Player"));


    foreach (QAction *action, QList<QAction*>()
            << m_listFirstAction << m_previousAction << m_toBeginAction << emptyAction << emptyAction
            << m_playAction << m_pauseAction << m_stopAction << m_playbackAction << emptyAction << emptyAction
            << m_stepBackAction << m_stepForwardAction << emptyAction << emptyAction
            << m_toEndAction << m_nextAction << m_listLastAction << emptyAction << emptyAction
            //<< m_repeatAction << m_recAction << m_pullAction << m_turnOffAction
             )

    {
        if (action == emptyAction) {
            m_playerToolBar->addSeparator();
            continue;
        }
            m_playerToolBar->addAction(action);
    }


    m_spinBox = new QSpinBox;
    m_spinBox->setMaximum(100000);
    m_spinBox->setMinimum(1);
    m_spinBox->setMaximumHeight(m_playerToolBar->height());
    hlayout->addWidget(m_spinBox);
    hlayout->addWidget(m_playerToolBar, 0);
    m_dtEdit = new QDateTimeEdit;
    m_dtEdit->setMaximumHeight(m_playerToolBar->height());
    hlayout->addWidget(m_dtEdit);
    QVBoxLayout * vlayout = new QVBoxLayout;
    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setMaximum(1000);
    vlayout->addWidget(m_slider);
    vlayout->addLayout(hlayout);
    setLayout(vlayout);
}

void GanttPlayer::createConnections()
{
    QHash<QAction*, QString> slotForAction;

    slotForAction[m_playAction] = SLOT(playerPlay());
    slotForAction[m_pauseAction] = SLOT(playerPause());
    slotForAction[m_stopAction] = SLOT(playerStop());
    slotForAction[m_playbackAction] = SLOT(playerPlayback());

    QHashIterator<QAction*, QString> i(slotForAction);
    while (i.hasNext()) {
        i.next();
        connect(i.key(), SIGNAL(triggered()), this, qPrintable(i.value()));
    }

    connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(spinBoxSlot(int)));
    connect(this->m_dtEdit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(dt2pix(QDateTime)));
    connect(m_slider, SIGNAL(sliderMoved(int)/*valueChanged(int)*/), this, SLOT(sliderSlot(int)));

}


void GanttPlayer::dt2pix(QDateTime dt)
{
    m_playerCurrentValue = (m_seedDT.msecsTo(dt)*m_secWidth)/m_timeInc;
    emit currentValueChanged(m_playerCurrentValue);
    m_currentDT = m_dtEdit->dateTime();
    m_slider->setValue((m_playerCurrentValue * m_slider->maximum()) / m_playerEndValue);
    qDebug()<<"dt2pix";
}

void GanttPlayer::sliderSlot(int value)
{
    m_playerCurrentValue = (value * m_playerEndValue) / m_slider->maximum();
    emit currentValueChanged(m_playerCurrentValue);
    m_currentDT = m_seedDT.addMSecs((m_playerCurrentValue * m_timeInc) / m_secWidth);
    m_dtEdit->setDateTime(m_currentDT);
    qDebug()<<"slider Slot";
}

void GanttPlayer::scaleSlot()
{
    dt2pix(m_currentDT);
}


void GanttPlayer::playerPlay()
{
    if(m_playIsClicked == false)
    {
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaybackSlot()));
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaySlot()));
        connect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaySlot()));

        m_timer->start(m_timeInc);
        m_playIsClicked = true;
        m_playbackIsClicked = false;
    }
}

void GanttPlayer::timerPlaySlot()
{
    m_currentDT.setMSecsSinceEpoch(m_currentDT.toMSecsSinceEpoch()+m_timeInc*m_speed);
    if(m_currentDT >= m_endDT)
    {
        playerPause();
        m_currentDT = m_endDT;
        qDebug()<<m_currentDT<<m_endDT;
    }
    m_dtEdit->setDateTime(m_currentDT);

}

void GanttPlayer::playerPause()
{
    m_timer->stop();
    m_playIsClicked = false;
    m_playbackIsClicked = false;
}

void GanttPlayer::playerStop()
{
    m_timer->stop();
    m_playerCurrentValue = m_playerBeginValue;
    m_playIsClicked = false;
    m_playbackIsClicked = false;
    m_currentDT = m_seedDT;
    m_dtEdit->setDateTime(m_currentDT);
}

void GanttPlayer::playerPlayback()
{
    if(m_playbackIsClicked == false)
    {
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaySlot()));
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaybackSlot()));
        connect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaybackSlot()));
        m_timer->start(m_timeInc);
        m_playbackIsClicked = true;
        m_playIsClicked = false;
    }
}

void GanttPlayer::timerPlaybackSlot()
{
    if(m_currentDT <= m_seedDT)
    {
        playerStop();
        return;
    }
    m_currentDT.setMSecsSinceEpoch(m_currentDT.toMSecsSinceEpoch()-m_timeInc*m_speed);

    m_dtEdit->setDateTime(m_currentDT);
}

void GanttPlayer::spinBoxSlot(int speed)
{
    m_speed = speed;
}
