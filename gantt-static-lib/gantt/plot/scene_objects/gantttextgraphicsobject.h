#ifndef GANTTTEXTGRAPHICSOBJECT_H
#define GANTTTEXTGRAPHICSOBJECT_H

#include "gantt-static-lib-global.h"
#include "ganttgraphicsobject.h"


class GANTTSTATICLIB_EXPORT GanttTextGraphicsObject : public GanttGraphicsObject
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
