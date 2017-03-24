/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию функций для работы с древовидными структурами
 * \~englist
 * \brief File contains realization of extra functions to work with tree structures
 */
#include "treewalker.h"

void printTreeR(GanttInfoItem *item, int nest)
{
//    qDebug() << makeTabs(nest) + item->title();

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
