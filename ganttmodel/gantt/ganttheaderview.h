#ifndef GANTTHEADERVIEW_H
#define GANTTHEADERVIEW_H

#include "utcdatetime.h"

#include <QHeaderView>

class GanttTreeView;
class GanttWidget;

class GanttHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    GanttHeaderView(Qt::Orientation orientation, QWidget * parent = 0);

public slots:


protected:
    void paintEvent(QPaintEvent *e);

    GanttWidget *m_widget;
};

#endif // GANTTHEADERVIEW_H
