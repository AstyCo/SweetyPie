#ifndef GANTTPLAYER_H
#define GANTTPLAYER_H

#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QTimer>

class GanttPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit GanttPlayer(QWidget *parent = 0);
    ~GanttPlayer();

private:
    QToolBar * m_playerToolBar;
    qreal m_playerCurrentValue;
    qreal m_playerEndValue;
    qreal m_playerBeginValue;
    QTimer * m_timer;

    QAction * m_playAction;
    QAction * m_pauseAction;
    QAction * m_stopAction;
    QAction * m_playbackAction;
    QAction * m_stepBackAction;
    QAction * m_stepForwardAction;
    QAction * m_listFirstAction;
    QAction * m_listLastAction;
    QAction * m_toBeginAction;
    QAction * m_toEndAction;
    QAction * m_repeatAction;
    QAction * m_recAction;
    QAction * m_turnOffAction;
    QAction * m_nextAction;
    QAction * m_previousAction;
    QAction * m_pullAction;

    void createActions();
    void createMenusAndToolBar();
    void createConnections();

    bool m_playIsClicked;
    bool m_playbackIsClicked;

signals:


public slots:

    void playerPlay();
    void timerPlaySlot();
    void playerPause();
    void playerStop();
    void playerPlayback();
    void timerPlaybackSlot();

};

#endif // GANTTPLAYER_H
