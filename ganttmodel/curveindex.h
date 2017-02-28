#ifndef CURVEINDEX_H
#define CURVEINDEX_H

#include "ganttproject_global.h"

/// Структура для индексации точки среди нескольких графиков
struct GANTTMODELSHARED_EXPORT CurveIndex
{
    CurveIndex(){
        indexCurve=-1;
        indexPoint=-1;
    }

    /// Действительный ли индекс
    bool isValid() const {
        return  indexCurve!=-1 &&
                indexPoint!=-1;
    }

    int indexCurve; ///< Номер среди графиков
    long indexPoint; ///< Номер точки на графике
};


#endif // CURVEINDEX_H
