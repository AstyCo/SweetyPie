#ifndef GANTTPLAYER_H
#define GANTTPLAYER_H

#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QTimer>
#include <QSpinBox>
#include <QDateTimeEdit>
#include <QSlider>
#include "ganttproject_global.h"

class GANTTMODELSHARED_EXPORT GanttPlayerControl : public QWidget
{
    Q_OBJECT
public:
    explicit GanttPlayerControl(QWidget *parent = 0);
    ~GanttPlayerControl();


    QDateTime beginDt() const;
    void setBeginDt(const QDateTime &beginDt);

    QDateTime endDt() const;
    void setEndDt(const QDateTime &endDt);


    QDateTime currentDt() const;
    void setCurrentDt(const QDateTime &currentDt);

    QDateTimeEdit *dtEdit() const;


private:
    QToolBar * m_playerToolBar;
    QSlider * m_slider;
    QSpinBox * m_spinBox;
    QDateTimeEdit * m_dtEdit;
    QTimer * m_timer;

    QAction * m_playAction;
    QAction * m_pauseAction;
    QAction * m_stopAction;
    QAction * m_playbackAction;
    QAction * m_toBeginAction;
    QAction * m_toEndAction;

    void createActions();
    void createMenusAndToolBar();
    void createConnections();

signals:
    void currentValueChanged(int currentValue);
    void currentDtChanged(const QDateTime &dt);

public slots:

    void playerPlay(bool b);
    void timerPlaySlot();
    void playerPause();
    void playerStop();
    void playerPlayback(bool b);
    void timerPlaybackSlot();
    void spinBoxSlot(int speed);
    void dtEditSlot(const QDateTime &dt);
    void sliderSlot(int value);



};

#endif // GANTTPLAYER_H
