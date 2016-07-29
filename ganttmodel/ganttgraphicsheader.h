#ifndef GANTTGRAPHICSHEADER_H
#define GANTTGRAPHICSHEADER_H

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

#include "ganttgraphicsheaderrectitem.h"



class GanttGraphicsHeader : public QGraphicsWidget
{
    Q_OBJECT
public:


    explicit  GanttGraphicsHeader(QDateTime begin, QDateTime end, Scale scale, QGraphicsWidget/*QObject*/ *parent = 0);
    ~GanttGraphicsHeader();

    //paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QDateTime begin() const;
    void setBegin(const QDateTime &begin);

    QDateTime end() const;
    void setEnd(const QDateTime &end);

    Scale zoom() const;
    void setZoom(const Scale &scale);

    qreal lowWidth() const;
    void setLowWidth(const qreal &lowWidth);

    QDateTime increaseDateTime(QDateTime dateTime, Scale scale, int increment);

    QRectF m_fullHeaderRect;
    QRectF m_backgroundRect;
    QRectF m_actualHeaderRect;

private:



    int lowItemsCount(QDateTime begin, QDateTime end, Scale scale);
    QString formatOverDateTime(Scale scale);
    QDateTime pix2dt(qreal pix); //переводит координату X в значение даты относительно начала координат.
    qreal dt2pix(QDateTime dt); //наоборот

    QGraphicsLinearLayout * lower;
    QGraphicsLinearLayout * upper;
    GanttGraphicsHeaderRectItem * m_rectItem;
    QColor m_color;
    QDateTime m_begin;
    QDateTime m_end;
    Scale m_scale;
    qreal m_lowWidth;

    int lowCount;
    int lowHeight;

signals:
    void zoomChanged(Scale zoom);

public slots:
    void createHeader();
    void clearHeader();

};

#endif // GANTTGRAPHICSHEADER_H
