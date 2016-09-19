#ifndef GANTTGRAPHICSOBJECT_H
#define GANTTGRAPHICSOBJECT_H

#include <QGraphicsObject>
#include <QPointer>

class GanttInfoItem;
class GanttHeader;
class GanttScene;

class GanttGraphicsObject : public QGraphicsObject
{
public:
    GanttGraphicsObject(GanttInfoItem *item,QGraphicsItem *parent = NULL);

    virtual GanttInfoItem *info() const;

    virtual void setHeader(const QPointer<GanttHeader> &header);
    virtual void setScene(const QPointer<GanttScene> &scene);

protected:
    QPointer<GanttHeader> m_header;
    QPointer<GanttScene> m_scene;
    QPointer<GanttInfoItem> m_info;
};

#endif // GANTTGRAPHICSOBJECT_H
