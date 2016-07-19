#ifndef KAMEMORYVIEW_H
#define KAMEMORYVIEW_H

#include <QGraphicsView>
#include "kamemory.h"

class KaMemoryView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit KaMemoryView(QWidget *parent = 0);
    ~KaMemoryView();

    void setScene(KaMemoryScene *scene);
private:
    bool eventFilter(QObject *object, QEvent *event);


public slots:

};

#endif // KAMEMORYVIEW_H
