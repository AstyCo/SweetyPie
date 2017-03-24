/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление GanttHoverGraphicsObject class.
 * \~englist
 * \brief File contains definition of GanttHoverGraphicsObject class.
 */
#ifndef GANTTHOVERGRAPHICSOBJECT_H
#define GANTTHOVERGRAPHICSOBJECT_H

#include "gantt/info/ganttinfoitem.h"
#include <QGraphicsObject>
#include <QPointer>

class GanttScene;

/*!
 * \~russian
 * \brief Класс GanttHoverGraphicsObject class подсвечивает выбранное событие
 * \~english
 * \brief The GanttHoverGraphicsObject class highlihts chosen event
 */
class GanttHoverGraphicsObject : public QGraphicsObject
{
    Q_OBJECT

    void init();
public:
    GanttHoverGraphicsObject(QGraphicsItem* parent = NULL);
    GanttHoverGraphicsObject(GanttScene *scene, QGraphicsItem *parent = NULL);

    QRectF boundingRect() const;
    void setScene(GanttScene* scene);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPos(int y);
    void setPos(const QPointF &pos);
    void setPos(qreal x, qreal y);

public slots:
    void setItem(GanttInfoItem *item);
    void onViewResized();

private slots:
    void onItemPosChanged();
    void disconnectItem();
    void connectItem();


private:
    GanttScene *m_scene;
    GanttInfoItem *_item;
};

#endif // GANTTHOVERGRAPHICSOBJECT_H
