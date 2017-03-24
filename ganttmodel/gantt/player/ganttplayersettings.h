/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление GanttPlayerSettings class.
 * \~englist
 * \brief File contains definition of GanttPlayerSettings class.
 */
#ifndef GANTTPLAYERSETTINGS_H
#define GANTTPLAYERSETTINGS_H

#include <QToolBar>
#include <QPointer>
#include <QDoubleSpinBox>

class QLabel;
class QSlider;

/*!
 * \~russian
 * \brief Класс GanttPlayerSettings class - настройки плеера
 * \~english
 * \brief The GanttPlayerSettings class is player's settings
 */
class GanttPlayerSettings : public QToolBar
{
    Q_OBJECT
    void init();
public:
    explicit GanttPlayerSettings(QWidget *parent = 0);
    ~GanttPlayerSettings();

    void setMultiplies(qreal min,qreal max);

    int precision() const;
    void setCurrentSpeed(qreal speed) const;
    qreal currentSpeed() const;

signals:
    void speedChanged(qreal multiply);

private slots:
    void onSpeedChanged(int m);
    void onCurTextChanged();

private:
    void initActions();


private:
    QPointer<QSlider> m_slider;
    QPointer<QLabel> m_left,m_right;
    QPointer<QDoubleSpinBox> m_cur;
    int m_precision;

};

#endif // GANTTPLAYERSETTINGS_H
