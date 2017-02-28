#include "treewalker.h"

void printTreeR(GanttInfoItem *item, int nest)
{
    if (GanttInfoNode *node = qobject_cast<GanttInfoNode*>(item)){
        qDebug() << makeTabs(nest) + node->title();

        foreach(GanttInfoItem *childItem, node->items())
            printTreeR(childItem, nest + 1);
    }
    else {
        qDebug() << makeTabs(nest) + item->title();
    }
}

QString makeTabs(int n)
{
    QString res;
    for(int i = 0; i < n; ++i)
        res += '\t';
    return res;
}
