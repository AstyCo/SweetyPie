/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление GanttGraphicsObject class.
 * \~englist
 * \brief File contains definition of GanttGraphicsObject class.
 */
#ifndef GANTTGRAPHICSOBJECT_H
#define GANTTGRAPHICSOBJECT_H

#include "ganttproject_global.h"
#include "extensions/dtline.h"
#include <QGraphicsObject>
#include <QPointer>
#include <QFont>

class GanttInfoItem;
class GanttScene;

/*!
 * \~russian
 * \brief Класс GanttGraphicsObject class отображает событие
 * \~english
 * \brief The GanttGraphicsObject class represents event
 */
class GANTTMODELSHARED_EXPORT GanttGraphicsObject : public QGraphicsObject
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
