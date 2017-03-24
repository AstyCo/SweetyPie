/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление GanttDtCrossObject class.
 * \~englist
 * \brief File contains definition of GanttDtCrossObject class.
 */
#ifndef GANTTDTCROSSOBJECT_H
#define GANTTDTCROSSOBJECT_H

#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QPainterPath>
#include <QPointer>
#include <QCursor>

class GanttScene;

/*!
 * \~russian
 * \brief Класс GanttDtCrossObject class отображает окно с текущее временем
 * \~english
 * \brief The GanttDtCrossObject class represents window with current time
 */
class GanttDtCrossObject : public QGraphicsObject
{

    void init();
public:
    GanttDtCrossObject(QGraphicsItem *parent = NULL);
    GanttDtCrossObject(GanttScene *scene, QGraphicsItem *parent = NULL);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setScene(GanttScene *scene);

    void setPos(const QPointF &pos);
    void setPos(qreal x, qreal y);
    void updatePaths();
protected:
    QPainterPath m_linesPath;
    QRect m_textRect;
    QString m_text;

    GanttScene *m_scene;

};

#endif // GANTTDTCROSSOBJECT_H
