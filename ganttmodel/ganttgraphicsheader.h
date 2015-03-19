#ifndef GANTTGRAPHICSHEADER_H
#define GANTTGRAPHICSHEADER_H

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

#include "ganttgraphicsheaderrectitem.h"



class GanttGraphicsHeader : public QGraphicsWidget
{
public:


    explicit  GanttGraphicsHeader(QDateTime begin, QDateTime end, Scale scale, QGraphicsWidget *parent = 0);
    ~GanttGraphicsHeader();

    //paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QDateTime begin() const;
    void setBegin(const QDateTime &begin);

    QDateTime end() const;
    void setEnd(const QDateTime &end);

private:


    QDateTime increaseDateTime(QDateTime dateTime, Scale scale, int increment);
    int lowItemsCount(QDateTime begin, QDateTime end, Scale scale);
    QString formatOverDateTime(Scale scale);

    QGraphicsLinearLayout * lower;
    QGraphicsLinearLayout * upper;
    GanttGraphicsHeaderRectItem * m_rectItem;
    QColor m_color;
    QDateTime m_begin;
    QDateTime m_end;
    Scale m_scale;

signals:

public slots:
    void createHeader();
    void clearHeader();
    void zoom(Scale scale);

};

#endif // GANTTGRAPHICSHEADER_H
