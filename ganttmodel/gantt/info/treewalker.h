/*!
 * \file
 * \~russian
 * \brief Файл содержит вспомогательный функции для работы с древовидными структурами
 * \~englist
 * \brief File contains functions to work with tree structures
 */
#ifndef TREEWALKER_H
#define TREEWALKER_H

#include "ganttproject_global.h"
#include "gantt/info/ganttinfoitem.h"

QString makeTabs(int n);
GANTTMODELSHARED_EXPORT void printTreeR(GanttInfoItem *item, int nest);

template <typename T>
void callRecursively(GanttInfoItem *item, T (*functionPointer)(GanttInfoItem *))
{
    foreach (GanttInfoItem* childItem, item->items())
        callRecursively(childItem, functionPointer);

    (*functionPointer)(item);
}

template <typename T, typename Param1, typename Arg1>
void callRecursively(GanttInfoItem *item, T (*functionPointer)(GanttInfoItem *, Param1), const Arg1 &arg1)
{
    foreach (GanttInfoItem* childItem, item->items())
        callRecursively(childItem, functionPointer, arg1);

    (*functionPointer)(item, arg1);
}

//template <typename T, typename Param1, typename Arg1, typename Param2, typename Arg2>
//void callRecursively(GanttInfoItem *item, T (*functionPointer)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2)
//{
//}
//template <typename T, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
//void callRecursively(GanttInfoItem *item, T (*functionPointer)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
//{
//}
//template <typename T, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
//void callRecursively(GanttInfoItem *item, T (*functionPointer)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
//{
//}
//template <typename T, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
//void callRecursively(GanttInfoItem *item, T (*functionPointer)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
//{
//}


#endif // TREEWALKER_H
