/**
   @file      struct_types.h
   @brief     Файл содержит определения структур для взаимодействия с "С"-функциями расчетных алгоритмов
   @copyright ЗАО "Инженерный Центр ГЕОМИР"
   @author    Созонов А.А.
   @date      2012-12-10
*/

#ifndef STRUCT_TYPES_H
#define STRUCT_TYPES_H
#include <QPolygonF>
#include "utcdatetime.h"



/// Режимы изменения выбранного интервала
enum IntervalEditState { iesNone,  ///< Интервал не изменялся
                         iesBegin, ///< Изменилось начало интервала
                         iesEnd,   ///< Изменилось окончание интервала
                         iesInt    ///< Изменилась длина интервала
                       };

enum DimRadius
{
  drUnknown = 0,
  drMeters,
  drKilometers,
  dr1000Kilometers
};

enum DimVelocity
{
  dvUnknown = 0,
  dvMetersSec,
  dvKilometersSec
};

enum TimeDimensions
{
  tSeconds,
  tMinutes,
  tHours,
  tDays
};

enum DistanceDimensions
{
  dMillimeters,
  dMeters,
  dKilometers,
  d1000Kilometers
};

struct DimensionsRV
{
  DimensionsRV()
  {
    R = drUnknown;
    V = dvUnknown;
  }
  DimensionsRV(DimRadius r, DimVelocity v)
  {
    R = r;
    V = v;
  }

  DimRadius R;      ///< Размерность радиус-вектора
  DimVelocity V;    ///< Размерность вектора скорости
};

enum CoordinateSystem { cssNoneOrUnknown = 0, ///< Система координат неизвестна или не требуется (РКО)
                        cssGSK = 1,     ///< Гринвическая СК
                        cssASK = 2,     ///< Абсолютная СК
                        cssSGP = 3,     ///< SGP СК
                        cssOSK = 4,      ///< Оскулирующие элементы
                        cssJ2000 = 5    ///< Абсолютная СК J2000
                      };

const int ATMOSPHERE_GOST_2004 = 0;
const int ATMOSPHERE_GOST_2001 = 1;

/// Входные параметры для алгоритмов вычислений НУ
struct InputParams
{
  InputParams()
  {
    workDir[0] = '\0';
    fnInput[0] = '\0';
    fnInput2[0] = '\0';
    fnInputAsn[0] = '\0';
    fnTask[0] = '\0';
    fnListing[0] = '\0';
    fnResult[0] = '\0';
    fnResultTLE[0] = '\0';
    fnResiduals[0] = '\0';
    fnResiduals2[0] = '\0';
    fnResiduals3[0] = '\0';
    fnResidualsJointAsn[0] = '\0';
    fnResidualsJointRko[0] = '\0';
    dataDm.R = drMeters;
    dataDm.V = dvMetersSec;
    atmGost = ATMOSPHERE_GOST_2004;
    hasRkoDist = false;
    hasRkoVel = false;
  }

  char workDir[1024];               ///< Рабочая директория алгоритма (для создания сгенерированных файлов)
  char fnInput[1024];               ///< Файл с входными данными
  char fnInput2[1024];              ///< Файл с входными данными АСН для совместного режима
  char fnInputAsn[1024];              ///< Файл с входными данными АСН для совместного режима
  char fnTask[1024];                ///< Файл с параметрами расчета
  char fnListing[1024];             ///< Файл с промежуточными вычислениями
  char fnResult[1024];              ///< Файл с результатами вычислений
  char fnResultTLE[1024];           ///< Файл с результатами вычислений TLE
  char fnResiduals[1024];           ///< Файл с остатками
  char fnResiduals2[1024];          ///< Файл с остатками АСН для совместной обработки
  char fnResiduals3[1024];          ///< Файл с остатками АСН Бортовой
  char fnResidualsJointAsn[1024];   ///< Файл с остатками АСН после совместной обработки
  char fnResidualsJointRko[1024];   ///< Файл с остатками РКО после совместной обработки
  DimensionsRV dataDm;                ///< Размерность входных данных
  int atmGost;                      ///< ГОСТ атмосферы
  bool hasRkoDist;
  bool hasRkoVel;
};

#endif // STRUCT_TYPES_H
