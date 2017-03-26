/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию AbstractGanttFactory class.
 * \~englist
 * \brief File contains realisation of AbstractGanttFactory class.
 */
#include "abstractganttfactory.h"


AbstractGanttFactory::~AbstractGanttFactory() {}

/*!
 * \brief virtual AbstractGanttFactory::isEvent
 * \return true если событие отслеживается плеером
 */
bool AbstractGanttFactory::isEvent(const QModelIndex &/*index*/) const
{
    return true;
}

/*!
 * \brief virtual AbstractGanttFactory::initialize
 *
 * Вызывается в момент установки в GanttWidget, по умолчанию ничего не делает
 */
void AbstractGanttFactory::initialize(GanttInfoTree */*tree*/)
{
    // default does nothing
}

