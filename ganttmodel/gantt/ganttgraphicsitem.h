#ifndef GANTTGRAPHICSITEM_H
#define GANTTGRAPHICSITEM_H

#include <QGraphicsLayoutItem>
#include <QGraphicsItem>
//#include "ganttgraphicsitemstage.h"
#include "ganttitem.h"
#include "ganttgraphicsheader.h"

class GanttGraphicsItem : public QObject, public QGraphicsLayoutItem, public QGraphicsRectItem/*public QGraphicsLinearLayout*/
{
    Q_OBJECT
public:
    //(GanttItem * item, Scale scale, QDateTime headerBegin, QDateTime headerEnd, QGraphicsItem * parent = 0)
    GanttGraphicsItem(GanttItem * item, GanttGraphicsHeader * header,/*QDateTime headerBegin, QDateTime headerEnd,*/ QGraphicsItem * parent = 0);
    ~GanttGraphicsItem();


    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &geom);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    //=================

    //QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

public slots:

    void calcSizeSlot();

private:

    //GanttGraphicsItemStage * m_graphicsStage;
    GanttItem * m_ganttItem;
    GanttGraphicsHeader * m_header;


    QDateTime m_begin;
    QDateTime m_end;
    /*QDateTime*/ int m_duration;
    QString m_text;
    QColor m_color; // цвет блока
    qreal m_width;
    qreal m_height;
    enum StageType{Simple, Union}; // тип блока: обычный или объединяющий
    StageType m_type;
};

#endif // GANTTGRAPHICSITEM_H
