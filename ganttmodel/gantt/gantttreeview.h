#ifndef GANTTTREEVIEW_H
#define GANTTTREEVIEW_H

#include "utcdatetime.h"

#include <QTreeView>

class GanttGraphicsView;
class GanttHeaderView;

class GanttTreeView : public QTreeView
{
    Q_OBJECT

public:
    GanttTreeView(QWidget * parent = 0);

    void setGraphicsView(GanttGraphicsView *graphicsView);


public slots:
    void repaintHeader();

protected:
    void scrollContentsBy(int dx, int dy);

    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);

private:

    GanttGraphicsView* m_graphicsView;
    GanttHeaderView* m_header;

    int m_hSliderHeight;

    friend class GanttGraphicsView;
    friend class GanttHeaderView;

};



#endif // GANTTTREEVIEW_H
