/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление GanttTextGraphicsObject class.
 * \~englist
 * \brief File contains definition of GanttTextGraphicsObject class.
 */
#ifndef GANTTTEXTGRAPHICSOBJECT_H
#define GANTTTEXTGRAPHICSOBJECT_H

#include "ganttproject_global.h"
#include "ganttgraphicsobject.h"

/*!
 * \~russian
 * \brief Класс GanttGraphicsView class событие с подписью
 * \~english
 * \brief The GanttGraphicsView class signed event
 */
class GANTTMODELSHARED_EXPORT GanttTextGraphicsObject : public GanttGraphicsObject
{
    Q_OBJECT

public:
    GanttTextGraphicsObject(GanttInfoItem *item,QGraphicsItem *parent = NULL);


    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    virtual QString textRight() const;
    virtual QString textLeft() const;
    virtual QString textIn() const;
private:
    QRect textLeftRect() const;
    QRect textRightRect() const;
private slots:
    void onExpansionChange();

private:
    static int _spacing;
    static QFont _captureFont;
    static QFontMetrics _fontMetrics;
};

#endif // GANTTTEXTGRAPHICSOBJECT_H
