#ifndef GANTTPLAYER_H
#define GANTTPLAYER_H

#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QTimer>
#include <QSpinBox>
#include <QDateTimeEdit>
#include <QSlider>
#include "ganttmodel_global.h"

class GANTTMODELSHARED_EXPORT GanttPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit GanttPlayer(QWidget *parent = 0);
    ~GanttPlayer();


    QDateTime seedDT() const;
    void setSeedDT(const QDateTime &seedDT);

    qreal playerEndValue() const;
    void setPlayerEndValue(const qreal &playerEndValue);

    QDateTime endDT() const;
    void setEndDT(const QDateTime &endDT);


    QDateTime currentDT() const;
    void setCurrentDT(const QDateTime &currentDT);

    QDateTimeEdit *dtEdit() const;
    void setDtEdit(QDateTimeEdit *dtEdit);

private:
    QToolBar * m_playerToolBar;
    QSlider * m_slider;
    qreal m_playerCurrentValue;
    qreal m_playerEndValue;
    qreal m_playerBeginValue;
    qreal m_secWidth; //pixels
    int m_speed;
    int m_timeInc; //time increment in msecs
    QSpinBox * m_spinBox;
    QDateTimeEdit * m_dtEdit;
    QTimer * m_timer;
    QDateTime m_seedDT;
    QDateTime m_currentDT;
    QDateTime m_endDT;

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
    void currentValueChanged(qreal currentValue);

public slots:

    void playerPlay();
    void timerPlaySlot();
    void playerPause();
    void playerStop();
    void playerPlayback();
    void timerPlaybackSlot();
    void spinBoxSlot(int speed);
    void dt2pix(QDateTime dt);
    void sliderSlot(int value);
    void scaleSlot();


};

#endif // GANTTPLAYER_H
