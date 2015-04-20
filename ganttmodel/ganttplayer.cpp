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
    delete m_playerToolBar;
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
            << m_repeatAction << m_recAction << m_pullAction << m_turnOffAction)

    {
        if (action == emptyAction) {
            m_playerToolBar->addSeparator();
            continue;
        }
            m_playerToolBar->addAction(action);
    }

    hlayout->addWidget(m_playerToolBar, 0);
    setLayout(hlayout);
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

}

void GanttPlayer::playerPlay()
{
    if(m_playIsClicked == false)
    {
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaybackSlot()));
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaySlot()));
        connect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaySlot()));
        m_timer->start(100);
        m_playIsClicked = true;
        m_playbackIsClicked = false;
    }
}

void GanttPlayer::timerPlaySlot()
{
    m_playerCurrentValue += 1;
    qDebug()<<m_playerCurrentValue;
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
}

void GanttPlayer::playerPlayback()
{
    if(m_playbackIsClicked == false)
    {
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaySlot()));
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaybackSlot()));
        connect(m_timer, SIGNAL(timeout()), this, SLOT(timerPlaybackSlot()));
        m_timer->start(100);
        m_playbackIsClicked = true;
        m_playIsClicked = false;
    }
}

void GanttPlayer::timerPlaybackSlot()
{
    m_playerCurrentValue -= 1;
    qDebug()<<m_playerCurrentValue;
}
