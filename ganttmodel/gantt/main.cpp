#include "ganttwidget.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    GanttWidget w;
    w.adjustSize();
    w.resize(1000,300);
    w.show();
    return a.exec();
}
