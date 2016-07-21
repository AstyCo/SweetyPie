#ifndef MEMORYVIEW_HPP
#define MEMORYVIEW_HPP

#include <QGraphicsView>
#include <QtCore>

class KaMemory;

class MGridScene;
class MLineScene;

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

private:

    MGridScene* m_gridScene;
    MLineScene* m_lineScene;

};

#endif // MEMORYVIEW_HPP
