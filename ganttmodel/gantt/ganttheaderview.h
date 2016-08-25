#ifndef GANTTHEADERVIEW_H
#define GANTTHEADERVIEW_H

#include "utcdatetime.h"

#include <QHeaderView>

class GanttTreeView;

class GanttHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    GanttHeaderView(Qt::Orientation orientation, QWidget * parent = 0);

public slots:


protected:
    void paintEvent(QPaintEvent *e);
};

#endif // GANTTHEADERVIEW_H
