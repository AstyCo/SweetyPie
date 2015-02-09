#ifndef GANTTGRAPHICSITEMSTAGE_H
#define GANTTGRAPHICSITEMSTAGE_H

#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include "ganttitem.h"
#include "ganttgraphicsheader.h"

#define TM_DATE_FONT_SIZE 8

class GanttGraphicsItemStage : public QGraphicsLayoutItem, public QGraphicsRectItem
{
public:
//    explicit GanttGraphicsItemStage(QDateTime begin, QDateTime end,
//                                    qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);

    //GanttGraphicsItemStage(GanttItemStage * stage, Scale scale, qreal durationTillBegin, QGraphicsItem * parent = 0);
    GanttGraphicsItemStage(GanttItem * item, Scale scale, qreal durationTillBegin, QGraphicsItem * parent = 0);

    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &geom);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    //=================

    //QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);



signals:

public slots:

private:
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

#endif // GANTTGRAPHICSITEMSTAGE_H
