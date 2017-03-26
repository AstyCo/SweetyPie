/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление AbstractGanttFactory class.
 * \~englist
 * \brief File contains definition of AbstractGanttFactory class.
 */

#ifndef ABSTRACTGANTTFACTORY_H
#define ABSTRACTGANTTFACTORY_H

#include "extensions_gui_global.h"

#include <QAbstractItemModel>

class GanttInfoItem;
class GanttGraphicsObject;
class GanttInfoTree;

/*!
 * \~russian
 * \brief Класс AbstractGanttFactory class представляет набор функций, которые необходимо реализовать классу, чтобы выступать в роли конструктора для диаграммы Гантта
 * \~english
 * \brief The AbstractGanttFactory class class represents the functions which should be implemented by descendant to be Gantt factory
 */
class EXTENSIONS_GUI_SHARED_EXPORT AbstractGanttFactory
{
public:
    /*!
     * \brief createInfo
     * \param index
     * \param parent
     * \return GanttInfoItem class, соотнесенный с index, дочерний для parent
     */
    virtual GanttInfoItem *createInfo(const QModelIndex &index, GanttInfoItem *parent) = 0;

    /*!
     * \brief createGraphicsObject
     * \param info
     * \param index
     * \return Объект сцены соотнесенный с info, index
     */
    virtual GanttGraphicsObject *createGraphicsObject(GanttInfoItem *info, const QModelIndex &index) = 0;

    /*!
     * \brief model
     * \return Указатель на конкретную QAbstractItemModel class
     */
    virtual QAbstractItemModel *model() const = 0;

    /*!
     * \brief virtual infoForIndex
     *
     * Переопределите, чтобы диаграмма могла работать с данными представленными в специальном виде.
     * \param index QModelIndex class
     * \return GanttInfoItem class для QModelIndex class
     */
    virtual GanttInfoItem *infoForIndex(const QModelIndex &index) const = 0;
    /*!
     * \brief virtual indexForInfo
     *
     * Переопределите, чтобы диаграмма могла работать с данными представленными в специальном виде.
     * \param item Указатель на GanttInfoItem class
     * \return QModelIndex class для GanttInfoItem class
     */
    virtual QModelIndex indexForInfo(const GanttInfoItem *item) const = 0;
    virtual bool isEvent(const QModelIndex &index) const;
    virtual void initialize(GanttInfoTree *tree = NULL);
};

#endif // ABSTRACTGANTTFACTORY_H
