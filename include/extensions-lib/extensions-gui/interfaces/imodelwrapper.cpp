/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию IModelWrapper class.
 * \~englist
 * \brief File contains realisation of IModelWrapper class.
 */
#include "imodelwrapper.h"

IModelWrapper::~IModelWrapper() {}

bool IModelWrapper::isEvent(const QModelIndex &) const
{
    return true;
}

bool IModelWrapper::isDot(QModelIndex &index)
{
    return length(index).totalMicroseconds() == 0;
}
