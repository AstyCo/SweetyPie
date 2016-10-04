#ifndef CHARTINTERVALSELECTOR_H
#define CHARTINTERVALSELECTOR_H

#include <QObject>
#include <QPointF>
#include <QAction>

#include <utcdatetime.h>

#include <qwt/qwt_text.h>
#include <qwt/qwt_plot.h>

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

class QwtPlot;
class QwtPlotCurve;
class QwtPlotMarker;
class PlotNavigator;
class QwtPlotPicker;
class PlotKeyEventHandler;
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

  /// Выделение первой точки интевала
  void setIntervalSelectionStart(QPointF pos);
  /// Выделение второй (последней) точки интервала
  void setIntervalSelectionEnd(QPointF pos);

  /// Выполняет выделение интервала
  void setIntervalSelection(qreal begin, ///< Начало интервала
                             qreal end ///< Конец интервала
                             );
  /// Отменяет выделенный интервал
  void clearIntervalSelection();

  CurveIndex selectedPoint() const { return m_selectedPointIndex; }
  void selectPointByIndex(CurveIndex idx);
  void clearSelectedPoint();
  /// Отображает пояснительный текст к выбранной пользователем точке
  void setSelectedPointLabel(const QwtText &xLbl, const QwtText &yLbl);

  void setSelectionModeTargetingPoint(bool b);
  void setTargetingPoint(qreal value);
  void clearTargetingPoint();

  void clearAllSelections();

  void reset();

public slots:
  void onCurveAdded(QwtPlotCurve *curve);
  /// Установить начало выделение интервала
  void setIntervalSelection(bool b);
  void onCurveVisibilityChanged(int curveIdx, bool b);

signals:
  /// Точка выбрана пользователем
  void pointSelected(CurveIndex index);
  /// Выбрана первая точка при выделении интервала
  void intervalSelectionStarted(QPointF p);
  /// Выбрана вторая точка при выделении интервала
  void intervalSelectionEnded(QPointF p);
  /// Выбрана целевая точка
  void targetingPointSet(qreal val);

protected:
  void setIntervalSelectionByState(QPointF pos);
  void showSelectionPoint(QwtText xLbl, QwtText yLbl, QPointF point, QwtPlot::Axis axis = QwtPlot::yLeft);

  void showSelectionInterval(QPointF start, QPointF end);

  void showSelectionIntervalStart(QPointF start);
  void showSelectionIntervalEnd(QPointF end);

protected slots:
  void onNextCurvePointSelected(bool prev);
  /// По выбору текущей точки
  void onCurvePointSelected(const QPointF& pos);
  void onAction_SelectTarget_toggled(bool checked);

  virtual void drawMarkerOnCurve();

private:  
  void updateDetailsPanelsSelPoint();

  void updateCurvesIntervalStats();

protected:
  enum SelectionState
  {
    ssNone,
    ssTargetingPoint,
    ssIntervalBegin,
    ssIntervalEnd
  };

  ChartXYWidget *m_chart;
  QwtPlot *m_plot;
  PlotNavigator *m_navigator;
  QAction *m_selIntAct;
  QwtPlotPicker *m_picker;
  PlotKeyEventHandler *m_keyEventHandler;

  /// Дата и время первой точки
  CurveIndex m_beginLimit;
  /// Дата и время последней точки
  CurveIndex m_endLimit;

  /// Дата и время начала выделенного интервала
  qreal m_intervalBegin;
  qreal m_targetingPoint;
  /// Дата и время окончания выделенного интервала
  qreal m_intervalEnd;

  /// Номер текущей выделенной точки
  CurveIndex m_selectedPointIndex;


  /// Номер первой точки в выделенном интервале
  CurveIndex m_selectionBeginPointIdx;
  /// Номер последней точки в выделенном интервале
  CurveIndex m_selectionEndPointIdx;

  /// Текущий выделенный интервал
  bool m_hasSelection;
  SelectionState m_selectionState;

  /** Маркеры обозначения выделенной точки.
  * Один маркер для вертикальной, один для горизонтальной линии
  */
  QwtPlotMarker *m_pMarker[2];
  /** Маркеры обозначения выделенного интервала.
  *   Маркеры в виде вертикальной линии
  */
  QwtPlotMarker *m_pIntervalMarker[2];

  /// Маркер для обозначения точки прицеливания
  QwtPlotMarker *m_pTargetingMarker;

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
                              UtcDateTime endDt ///< Конец интервала
                              );

signals:
   void targetingDtSet(UtcDateTime dt);

private slots:
   void onTargetingPointSet(qreal point);
};

#endif // CHARTINTERVALSELECTOR_H
