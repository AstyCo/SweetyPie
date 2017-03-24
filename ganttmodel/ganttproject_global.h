/*!
 * \file
 * \~russian
 * \brief Файл для экспорта C++ библиотеки
 * \~englist
 * \brief File for export C++ lib
 */
#ifndef GANTTMODEL_GLOBAL_H
#define GANTTMODEL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GANTTMODEL_LIBRARY)
#  define GANTTMODELSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GANTTMODELSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GANTTMODEL_GLOBAL_H
