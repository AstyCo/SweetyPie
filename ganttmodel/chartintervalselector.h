#ifndef CHARTINTERVALSELECTOR_H
#define CHARTINTERVALSELECTOR_H

#include <QObject>
#include <QPointF>
#include <QAction>

#include "chartxywidget.h"

#include <utcdatetime.h>

#include <qwt/qwt_text.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_marker.h>

#include "ganttproject_global.h"
#include "plotnavigator.h"

#include "curveindex.h"

class ChartXYWidget;


class GANTTMODELSHARED_EXPORT ChartIntervalSelector : public QObject
{
  Q_OBJECT

public:    
  ChartIntervalSelector(ChartXYWidget *chart, PlotNavigator *nav);

  /// Точка начала интервала данных
  qreal begin() const;
  /// Точка конца интервала данных
  qreal end() const;

  /// Точка начала выделенного интервала
  qreal intervalSelectionBegin() const { return m_intervalBegin; }
  /// Точка конца выделенного интервала
  qreal intervalSelectionEnd() const { return m_intervalEnd; }



  /// Выполняет выделение интервала
  void setIntervalSelection(qreal begin, ///< Начало интервала
                             qreal end ///< Конец интервала
                             );
  /// Отменяет выделенный интервал
  void clearIntervalSelection();

  void setSelectionModeTargetPoint(bool b);
  void clearTargetPoint();

  virtual void clearAllSelections();

  void reset();

  QColor color() const;

  bool visible() const;

public slots:
  void onCurveAdded(QwtPlotCurve *curve);

  /// Установить начало выделение интервала
  void onAction_SelectInterval_toggled(bool b);
  void onAction_SelectTarget_toggled(bool checked);

  /// Выделение первой точки интевала
  void setIntervalSelectionStart(QPointF pos);
  /// Выделение второй (последней) точки интервала
  void setIntervalSelectionEnd(QPointF pos);

  void setTargetPoint(qreal value);

  virtual void setColor(const QColor &color);

  virtual void setVisible(bool b);

  void onCurvePointSelected(const QPointF &pos);
signals:
  /// Выбрана первая точка при выделении интервала
  void intervalSelectionStarted(QPointF p);
  /// Выбрана вторая точка при выделении интервала
  void intervalSelectionEnded(QPointF p);
  /// Выбрана целевая точка
  void targetPointSet(qreal val);

protected:
  void setIntervalSelectionByState(QPointF pos);

  void showSelectionInterval(QPointF start, QPointF end);

  void showSelectionIntervalStart(QPointF start);
  void showSelectionIntervalEnd(QPointF end);

  void showTargetPoint(QPointF point);

private:  

  void updateCurvesIntervalStats();

protected:
  ChartXYWidget *m_chart;
  QwtPlot *m_plot;
  PlotNavigator *m_navigator;
  QAction *m_selIntAct;

  PlotKeyEventHandler *m_keyEventHandler;

  /// Дата и время первой точки
  CurveIndex m_beginLimit;
  /// Дата и время последней точки
  CurveIndex m_endLimit;

  /// Дата и время начала выделенного интервала
  qreal m_intervalBegin;
  qreal m_targetPoint;
  /// Дата и время окончания выделенного интервала
  qreal m_intervalEnd;

  /// Номер первой точки в выделенном интервале
  CurveIndex m_selectionBeginPointIdx;
  /// Номер последней точки в выделенном интервале
  CurveIndex m_selectionEndPointIdx;

  /// Текущий выделенный интервал
  bool m_hasSelection;

  /** Маркеры обозначения выделенного интервала.
  *   Маркеры в виде вертикальной линии
  */
  QwtPlotMarker *m_pIntervalMarker[2];

  /// Маркер для обозначения точки прицеливания
  QwtPlotMarker *m_pTargetPointMarker;

  QColor _color;

  bool m_visible;

  enum SelectionState
  {
    ssNone,
    ssTargetPoint,
    ssIntervalBegin,
    ssIntervalEnd
  };
  SelectionState m_selectionState;

};

class GANTTMODELSHARED_EXPORT ChartTimeIntervalSelector : public ChartIntervalSelector
{
  Q_OBJECT

public:
   ChartTimeIntervalSelector(ChartXYWidget *chart, PlotNavigator *nav);

   /// Дата и время начала выделенного интервала
   UtcDateTime getIntervalSelectionBeginDt();
   /// Дата и время конца выделенного интервала
   UtcDateTime getIntervalSelectionEndDt();

   /// Возвращает минимальное значение даты и время среди всех графиков
   UtcDateTime minimumDt();
   /// Возвращает максимальное значение даты и времени среди всех графиков
   UtcDateTime maximumDt();

   /// Выполняет выделение интервала
   void setIntervalSelectionByDates(UtcDateTime beginDt, ///< Начало интервала
                              UtcDateTime end ///< Конец интервала
                              );

   void setTargetPointByDates(UtcDateTime dt);

signals:
   void targetPointDtSet(UtcDateTime dt);
   void intervalSelectionFinished(const UtcDateTime &beg, const UtcDateTime &end);

private slots:
   void ontargetPointSet(qreal point);
   void onIntervalSelectionFinish();
};

#endif // CHARTINTERVALSELECTOR_H
