/*!
 * \file
 * \~russian
 * \brief Заголовочный файл с объявлением GanttFactory class
 *
 * Класс GanttFactory class реализует абстрактные методы класса AbstractGanttFactory class,
 * используя конкретную реализацию IModelWrapper class. В большинстве случаев, достаточно
 * реализовать IModelWrapper class, и использовать данную реализацию AbstractGanttFactory class.
 * \~english
 * \brief Header with default implementation of AbstractGanttFactory class
 *
 * Class GanttFactory class implements abstract methods of AbstractGanttFactory class,
 * using realization of IModelWrapper class interface (GanttFactory class follows
 * Template Method pattern).
*/
#ifndef GANTTFACTORY_H
#define GANTTFACTORY_H

#include "ganttproject_global.h"
#include "extensions-gui/interfaces/abstractganttfactory.h"
#include "extensions-gui/interfaces/imodelwrapper.h"

/*!
 * \~russian
 * \brief Класс GanttFactory class реализует абстрактные методы класса AbstractGanttFactory class, используя конкретную реализацию IModelWrapper class.
 *
 * Класс GanttFactory class реализует абстрактные методы класса AbstractGanttFactory class,
 * используя конкретную реализацию IModelWrapper class. В большинстве случаев, достаточно
 * реализовать IModelWrapper class, и использовать данную реализацию AbstractGanttFactory class.
 * \~english
 * \brief The GanttFactory class class realise abstract methods of AbstractGanttFactory class, using concrete realization of IModelWrapper class.
 *
 * The GanttFactory class realize abstract methods of AbstractGanttFactory class,
 * using concrete realization of IModelWrapper class.
 */
class GANTTMODELSHARED_EXPORT GanttFactory : public AbstractGanttFactory
{
public:
    enum RegisteredGraphicsObjectType{
        UnregisteredType                = 0,
        GanttIntervalGraphicsObjectType = 1,
        GanttCalcGraphicsObjectType     = 2
    };


    // Construtor
    explicit GanttFactory(IModelWrapper *wrapper);
    ~GanttFactory();

    virtual GanttInfoItem *createInfo(const QModelIndex &index, GanttInfoItem *parent);
    virtual GanttGraphicsObject *createGraphicsObject(GanttInfoItem *info, const QModelIndex &index);
    virtual QAbstractItemModel *model() const;

    virtual bool isEvent(const QModelIndex &index) const;

private:
    IModelWrapper *_wrapper;
    static RegisteredGraphicsObjectType readTag(const QString &tag);
};

#endif // GANTTFACTORY_H
