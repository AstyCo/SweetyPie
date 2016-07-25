#ifndef CHARTSETTINGS_H
#define CHARTSETTINGS_H

#include <QColor>
#include <QFont>
#include <QPen>

#include <QSettings>

#include <dimensions.h>

#include "ganttmodel_global.h"


struct GANTTMODELSHARED_EXPORT CurveStyle
{
  CurveStyle()
    : pen(Qt::red)
  {
    pen.setWidth(2);
  }
  QString id;
  QPen pen;

  static QList<Qt::GlobalColor> defColors;
};


struct GANTTMODELSHARED_EXPORT ChartSettings
{
  ChartSettings()
  {
    axisY_R = dMeters;
    axisY_V = dMillimeters;
    axisY_T = tSeconds;

    grid = false;
    keyStep = 1;
    precision = 6;
    detailsPanelVisible = false;
  }

  DistanceDimensions axisY_R;
  DistanceDimensions axisY_V;
  TimeDimensions axisY_T;

  QFont m_markerLabelFont;
  int precision;
  bool grid;
  int keyStep;
  bool detailsPanelVisible;

  void save(QSettings &ss) const;
  void load(const QSettings &ss);
};


#endif // CHARTSETTINGS_H
