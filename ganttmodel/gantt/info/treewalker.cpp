#include "treewalker.h"

void printTreeR(GanttInfoItem *item, int nest)
{
    qDebug() << makeTabs(nest) + item->title();

    foreach(GanttInfoItem *childItem, item->items())
        printTreeR(childItem, nest + 1);
}

QString makeTabs(int n)
{
    QString res;
    for(int i = 0; i < n; ++i)
        res += '\t';
    return res;
}
