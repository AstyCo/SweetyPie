/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление GanttGraphicsView class.
 * \~englist
 * \brief File contains definition of GanttGraphicsView class.
 */
#ifndef GANTTGRAPHICSVIEW_H
#define GANTTGRAPHICSVIEW_H

#include "gantt/private_extensions/mousepresshelper.h"
#include <QGraphicsView>

#include <QModelIndex>

class GanttWidget;
class HFitScene;

/*!
 * \~russian
 * \brief Класс GanttGraphicsView class отвечает за отрисовку GanttScene class
 * \~english
 * \brief The GanttGraphicsView class do drawing of GanttScene class
 */
class GanttGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GanttGraphicsView(QWidget *parent = 0);
    GanttGraphicsView(HFitScene * scene, QWidget * parent = 0);

    void setScene(HFitScene *scene);
    void setMousePressH(MousePressHelper *mph){
        _mousePressH = mph;
    }

    void setHSliderHeight(int hSliderHeight);

signals:
    void viewResized(const QSize& newSize);
    void maximumHeightChanged();

protected:
//    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);
private slots:
    void updateMaximumHeight();
private:
    void initialize();

private:
    int m_hSliderHeight;
    QCursor _lastCursor;
    QPoint _lastPos;
    MousePressHelper *_mousePressH;
};

#endif // GANTTVIEW_H
