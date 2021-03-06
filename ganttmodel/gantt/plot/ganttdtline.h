/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление GanttDtLine class.
 * \~englist
 * \brief File contains definition of GanttDtLine class.
 */
#ifndef GANTTDTLINE_H
#define GANTTDTLINE_H

#include "extensions/dtline.h"

/*!
 * \~russian
 * \brief Класс GanttDtLine class временная линейка с отображением текущего времени
 * \~english
 * \brief The GanttDtLine class timeline with current time
 */
class GanttDtLine : public DtLine
{
    Q_OBJECT
    void init();
public:
    explicit GanttDtLine(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

signals:
    void dtChanged(const UtcDateTime &dt);
    void dtChangedManually(const UtcDateTime &dt);


public slots:
    void setCurrentDt(const UtcDateTime &dt, bool manually = false);

    void showCurrentDt(bool show);
    void showCurrentDt();
    void hideCurrentDt();
    void updateCurrentDtPath();
private:
    static QPainterPath initFunction();

private:
    UtcDateTime _dt;
    UtcDateTime _minDt;
    TimeSpan _dtRange;
    QPainterPath _arrowPath;
    static QPainterPath _globalArrowPath;
    bool _drawCurrentDt;
};

#endif // GANTTDTLINE_H
