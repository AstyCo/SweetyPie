#ifndef MEMORYVIEW_HPP
#define MEMORYVIEW_HPP

#include <QGraphicsView>
#include <QtCore>
#include "memory.h"
#include "ganttproject_global.h"

class Memory;

class MGridScene;
class MLineScene;

class GANTTMODELSHARED_EXPORT MemoryView : public QGraphicsView
{
public:
    enum MemoryViewMode
    {
        MemoryLine=0x1,
        MemoryGrid=0x2,
        MemoryViewMode_count = 2
    };

    MemoryView(QWidget* parent = 0);
    MemoryView(QGraphicsScene * scene, QWidget * parent = 0);

    void resizeEvent(QResizeEvent *);
    void setScene(QGraphicsScene *scene);
    void init();
    void changeScene();
    void setMemory(const Memory& kaMemory);

    MemoryView::MemoryViewMode mode() const;
    void setMode(const MemoryView::MemoryViewMode &mode);
    void leaveEvent(QEvent *);

public slots:

private:

    MemoryViewMode m_mode;
    MGridScene* m_gridScene;
    MLineScene* m_lineScene;

};

QColor GANTTMODELSHARED_EXPORT MemoryState_to_QColor(MemoryPart::MemoryState state, bool isActive = true);

#endif // MEMORYVIEW_HPP
