/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление IModelWrapper class.
 * \~englist
 * \brief File contains definition of IModelWrapper class.
 */

#ifndef IMODELWRAPPER_H
#define IMODELWRAPPER_H

#include "extensions_gui_global.h"
#include "utcdatetime.h"

#include <QAbstractItemModel>
#include <QString>
/*!
 * \~russian
 * \brief Класс IModelWrapper class определяет набор функций, которые необходимо реализовать наследнику QAbstractItemModel class
 * \~english
 * \brief The IModelWrapper class class contains the functions which should be realized by desendant of QAbstractItemModel class 
 */
class EXTENSIONS_GUI_SHARED_EXPORT IModelWrapper
{
public:
    virtual ~IModelWrapper();

    // --- Interface

    /*!
     * \brief model
     * \return Указатель на QAbstractItemModel class
     */
    virtual QAbstractItemModel *model() const = 0;
    /*!
     * \brief tag
     * \param index
     * \return Тип события
     */
    virtual QString     tag(const QModelIndex &index) const = 0;
    /*!
     * \brief title
     * \param iGanttIndex
     * \return Имя события
     */
    virtual QString     title(const QModelIndex &iGanttIndex) const = 0;
    /*!
     * \brief start
     * \param iGanttIndex
     * \return Начало события
     */
    virtual UtcDateTime start(const QModelIndex &iGanttIndex) const = 0;
    /*!
     * \brief length
     * \param iGanttIndex
     * \return Длительность события
     */
    virtual TimeSpan    length(const QModelIndex &iGanttIndex) const = 0;
    /*!
     * \brief color
     * \param iGanttIndex
     * \return Цвет для события
     */
    virtual QColor      color(const QModelIndex &iGanttIndex) const = 0;

    // ---
    /*!
     * \brief isEvent
     * \return Отслеживать событие
     */
    virtual bool        isEvent(const QModelIndex &/*index*/) const;

    bool isDot(QModelIndex &index);
};

#endif // IMODELWRAPPER_H
