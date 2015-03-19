#ifndef GANTTGRAPHICSITEM_H
#define GANTTGRAPHICSITEM_H

#include <QGraphicsLayoutItem>
#include <QGraphicsItem>
//#include "ganttgraphicsitemstage.h"
#include "ganttitem.h"
#include "ganttgraphicsheader.h"

class GanttGraphicsItem : public QGraphicsLayoutItem, public QGraphicsRectItem/*public QGraphicsLinearLayout*/
{
public:
    //(GanttItem * item, Scale scale, QDateTime headerBegin, QDateTime headerEnd, QGraphicsItem * parent = 0)
    GanttGraphicsItem(GanttItem * item, Scale scale, GanttGraphicsHeader * header,/*QDateTime headerBegin, QDateTime headerEnd,*/ QGraphicsItem * parent = 0);
    ~GanttGraphicsItem();


    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &geom);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    //=================

    //QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:

    //GanttGraphicsItemStage * m_graphicsStage;
    GanttItem * m_ganttItem;
    void setGraphicsStages();
    Scale m_scale;
    QDateTime m_headerBegin;
    QDateTime m_headerEnd;
    GanttGraphicsHeader * m_header;


    QDateTime m_begin;
    QDateTime m_end;
    /*QDateTime*/ int m_duration;
    QString m_text;
    QColor m_color; // цвет блока
    //QSize m_size; // размер блока
    qreal m_width;
    qreal m_height;
    enum StageType{Simple, Union}; // тип блока: обычный или объединяющий
    StageType m_type;
};

#endif // GANTTGRAPHICSITEM_H
