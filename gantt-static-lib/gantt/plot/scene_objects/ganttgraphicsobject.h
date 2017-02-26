#ifndef GANTTGRAPHICSOBJECT_H
#define GANTTGRAPHICSOBJECT_H

#include "gantt-static-lib-global.h"
#include "extensions/dtline.h"
#include <QGraphicsObject>
#include <QPointer>
#include <QFont>

class GanttInfoItem;
class GanttScene;

class GANTTSTATICLIB_EXPORT GanttGraphicsObject : public QGraphicsObject
{
    Q_OBJECT

    void init();
public:
    GanttGraphicsObject(GanttInfoItem *item,QGraphicsItem *parent = NULL);
    virtual ~GanttGraphicsObject();

    virtual GanttInfoItem *info() const;
    virtual void setScene(GanttScene *scene);

    inline void setDtLine(DtLine *dtline);
    void setCurrent(bool newValue);
    void updateZValue();

    virtual int inactiveZValue() const = 0;

public slots:
    virtual void updateItemGeometry();
    virtual void updateToolTip();

signals:
    void graphicsItemPressed();
    void graphicsItemHoverEnter();
    void graphicsItemHoverLeave();

protected:
    DtLine *_dtline;
    QPointer<GanttInfoItem> m_info;

    bool _current;
};

// INLINE FUNCTIONS
inline void GanttGraphicsObject::setDtLine(DtLine *dtline) { _dtline = dtline;}


#endif // GANTTGRAPHICSOBJECT_H
