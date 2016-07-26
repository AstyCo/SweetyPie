#ifndef MEMORYVIEW_HPP
#define MEMORYVIEW_HPP

#include <QGraphicsView>
#include <QtCore>

class KaMemory;

class MGridScene;
class MLineScene;


namespace Memory
{
    enum MemoryViewMode
    {
        MemoryLine=0x1,
        MemoryGrid=0x2,
        MemoryViewMode_count = 2
    };
}

using namespace Memory;

class MemoryView : public QGraphicsView
{
public:
    MemoryView(QWidget* parent = 0);
    MemoryView(QGraphicsScene * scene, QWidget * parent = 0);

    void resizeEvent(QResizeEvent *);
    void setScene(QGraphicsScene *scene);
    void init();
    void changeScene();
    void setMemory(const KaMemory& kaMemory);

    MemoryViewMode mode() const;
    void setMode(const MemoryViewMode &mode);

public slots:

private:

    MemoryViewMode m_mode;
    MGridScene* m_gridScene;
    MLineScene* m_lineScene;

};

#endif // MEMORYVIEW_HPP
