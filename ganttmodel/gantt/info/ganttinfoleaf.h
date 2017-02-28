#ifndef GANTTINFOLEAF_H
#define GANTTINFOLEAF_H

#include "ganttinfoitem.h"

#include "utcdatetime.h"

#include <QColor>

class GanttInfoNode;

class GANTTSTATICLIB_EXPORT GanttInfoLeaf : public GanttInfoItem
{
    Q_OBJECT

public:
    GanttInfoLeaf(QObject *parent = NULL);
    GanttInfoLeaf(const QString &title
                  , const UtcDateTime   &start
                  , const TimeSpan      &ts
                  , const QModelIndex   &index = QModelIndex()
                  , const QColor        &color = Qt::green
                  , GanttInfoNode       *parentNode = NULL
                  , QObject             *parent = NULL);

    int columnCount() const;
    qreal height() const;
    GanttInfoNode *node();

private:
    virtual void disconnectParent();
    virtual void connectParent();

};

#endif // GANTTINFOITEM_H
