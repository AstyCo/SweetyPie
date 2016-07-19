#ifndef MEMORYVIEW_HPP
#define MEMORYVIEW_HPP

#include <QGraphicsView>
#include <QtCore>


class MemoryScene;

class MemoryView : public QGraphicsView
{
public:
    MemoryView(QWidget* parent = 0);
    MemoryView(QGraphicsScene * scene, QWidget * parent = 0);

    void resizeEvent(QResizeEvent *);
    void setScene(QGraphicsScene *scene);



private:


};

#endif // MEMORYVIEW_HPP
