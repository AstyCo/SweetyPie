#include "chartsettings.h"

QList<Qt::GlobalColor> CurveStyle::defColors(QList<Qt::GlobalColor>()
   << Qt::red << Qt::green << Qt::blue
   << Qt::cyan << Qt::magenta << Qt::yellow << Qt::gray);

void ChartSettings::save(QSettings &ss)
{
  ss.setValue("graph_y_r", (int)axisY_R);
  ss.setValue("graph_y_v", (int)axisY_V);
  ss.setValue("graph_y_t", (int)axisY_T);

  ss.setValue("graph_precision", precision);
  ss.setValue("graph_grid", grid);
  ss.setValue("graph_labels_font", m_markerLabelFont);
  ss.setValue("graph_key_step", keyStep);
  ss.setValue("details_panel_visible", detailsPanelVisible);

  /*
  ss.setValue("graph_color_1", cs.color_1);
  ss.setValue("graph_color_2", cs.color_2);
  ss.setValue("graph_color_3", cs.color_3);

  ss.setValue("graph_width", cs.width);
*/
}


void ChartSettings::load(const QSettings &ss)
{
  axisY_R = (DistanceDimensions)ss.value("graph_y_r", (int)dMeters).toInt();
  axisY_V = (DistanceDimensions)ss.value("graph_y_v", (int)dMeters).toInt();
  axisY_T = (TimeDimensions)ss.value("graph_y_t", (int)tSeconds).toInt();

  m_markerLabelFont = ss.value("graph_labels_font", QFont()).value<QFont>();
  precision = ss.value("graph_precision", 6).toInt();
  grid = ss.value("graph_grid", false).toBool();
  keyStep = ss.value("graph_key_step", 1).toInt();
  detailsPanelVisible = ss.value("details_panel_visible", false).toBool();
/*
  cs.color_1 = ss.value("graph_color_1", QColor(Qt::red)).value<QColor>();
  cs.color_2 = ss.value("graph_color_2", QColor(Qt::green)).value<QColor>();
  cs.color_3 = ss.value("graph_color_3", QColor(Qt::blue)).value<QColor>();
  cs.width = ss.value("graph_width", 1).toInt();
  */
}
