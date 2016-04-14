#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QTimer>
#include <QScrollArea>

#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_plot.h>

#include <timespan.h>
#include <utcdatetime.h>

#include "ganttmodel_global.h"
#include "plotinterval.h"
#include "curvedetailsgroupbox.h"
#include "plotkeyeventhandler.h"
#include "chartactionstoolbar.h"
#include "chartsettingsdlg.h"

enum ChartSelectionState
{
  ssNone,
  ssTargetingPoint,
  ssIntervalBegin,
  ssIntervalEnd
};

enum SearchDirection
{
  sdLeft,
  sdRight,
  sdAny
};


/// Класс для нижней временной оси
class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw():
        QwtScaleDraw()
    {

    }

    virtual QwtText label (double v) const
    {
        UtcDateTime dt;
        dt = UtcDateTime::fromMicrosecondsSinceEpoch((long long)v * 1000);
        return dt.toString("hh:mm:ss\ndd.MM.yy");
    }
};

namespace Ui {
class ChartWidget;
}

/// Структура для индексации точки среди нескольких графиков
struct GANTTMODELSHARED_EXPORT CurveIndex{
    CurveIndex(){
        indexCurve=-1;
        indexPoint=-1;
    }

    /// Действительный ли индекс
    bool isValid(){
        return  indexCurve!=-1 &&
                indexPoint!=-1;
    }

    int indexCurve; ///< Номер среди графиков
    long indexPoint; ///< Номер точки на графике
};

struct GANTTMODELSHARED_EXPORT ChartCurveStats
{
  QPointF maxValue;
  QPointF minValue;

  QPointF intervalBeginValue;
  QPointF intervalPointingValue;
  QPointF intervalEndValue;
};

class QwtPlotCurve;
class QwtPlotGrid;
class QwtPlotPanner;
class PlotNavigator;
class QwtPlotPicker;
class QwtPlotMarker;

/// Основной класс для графиков
class GANTTMODELSHARED_EXPORT ChartWidget : public QWidget
{
  Q_OBJECT

public:
  /// Конструктор класса
  ChartWidget(QWidget *parent = 0 ///< Указатель на родительский объект
          );

  /// Деструктор класса
  ~ChartWidget();

  /// Возвращает минимальное значение даты и время среди всех графиков
  UtcDateTime minimumDt();
  /// Возвращает максимальное значение даты и времени среди всех графиков
  UtcDateTime maximumDt();

  /// Устанавливает настройки левой оси (Если значения обеих линий равно 0 то они не рисуются)
  void setLeftAxis(const QString &title, ///< Заголовок
                   double minLine=0, ///< Линия минимума
                   double maxLine=0, ///< Линия максимума
                   const QColor &defaultColor=Qt::red ///< Начальный цвет линий макс. и мин.
          );

  /// Создает и устанавливает настройки правой оси (Если значения обеих линий равно 0 то они не рисуются)
  void setRightAxis(const QString &title, ///< Заголовок
                    double minLine=0, ///< Линия минимума
                    double maxLine=0, ///< Линия максимума
                    const QColor &defaultColor=Qt::red ///< Начальный цвет линий макс. и мин.
           );

  /// Устанавливает настройки линий макс. и мин. (Если значения обеих линий равно 0 то они не рисуются)
  void setLeftMinMaxValues(double minLine=0, ///< Линия минимума
                           double maxLine=0, ///< Линия максимума
                           const QColor &defaultColor=Qt::red ///< Начальный цвет линий макс. и мин.
          );

  /// Устанавливает настройки линий макс. и мин. (Если правая ось создана и значения обеих линий равно 0 то они не рисуются)
  void setRightMinMaxValues(double minLine=0, ///< Линия минимума
                            double maxLine=0, ///< Линия максимума
                            const QColor &defaultColor=Qt::red ///< Начальный цвет линий макс. и мин.
          );

  /// Удаляет все графики и другие значения с полотна (ну практически все)
  void clearChart();

  /// Выполняет выделение интервала
  void setIntervalSelectionByDates(UtcDateTime beginDt, ///< Начало интервала
                             UtcDateTime endDt ///< Конец интервала
                             );

  /// Отменяет выделенный интервал
  void clearIntervalSelection();


  /// Добавляет специально выделенную зону
  void addZone(const QString &name, double beginX, double endX, const QColor &c1=Qt::white, const QColor &c2=QColor());

  /// Добавляет специально выделенную зону
  void addZone(const QString &name, const UtcDateTime &begin, const UtcDateTime &endX, const QColor &c1, const QColor &c2);

  /// Добавляет новый график
  void setData(const QString &title, ///< Первоначальный цвет графика
               const QVector<QPointF> &data, ///< Значения точке графика
               QwtPlot::Axis axis = QwtPlot::yLeft);
  void setLeftAxisMargin(int value);
  void setRightAxisMargin(int value);
  /// Обновляет данные графика. Если таймер запущен то график не перерисовывается.
  void updateData(int indexCurve, const QVector<QPointF> &data);

  /// Дата и время начала выделенного интервала
  UtcDateTime getIntervalSelectionBeginDt() { return m_intervalBeginDt; }
  /// Дата и время конца выделенного интервала
  UtcDateTime getIntervalSelectionEndDt() { return m_intervalEndDt; }
  ChartCurveStats getCurveStats(int curveId) { return m_curvesStats.at(curveId); }

  bool panelCurveDetailsVisible();
  void setPanelCurveDetailsVisible(bool vis);

  void setChartToolBarVisible(bool vis);
  /// Выделение первой точки интевала
  void setIntervalSelectionStart(QPointF pos);

  /// Выделение второй (последней) точки интервала
  void setIntervalSelectionEnd(QPointF pos);
  void setSelectionModeTargetingPoint(bool b);
  void setTargetingPoint(UtcDateTime dt);
  void clearTargetingPoint();

  void selectPointByIndex(CurveIndex idx);
  /// Указатель на график
  QwtPlotCurve *curve(int index ///< Номер графика
                      ) const;

  /// Список интервалов
  QList<PlotInterval *> intervals() const;

  /// Список графиков
  QList<QwtPlotCurve *> curves() const;

  /// Запускает таймер для перерисовки графика
  void startOnlineReplot();

  /// Останавливает таймер для перерисовки графика
  void stopOnlineReplot();

  int countLastPoints() const;
  void setCountLastPoints(int countLastPoints);

  QwtPlot * getPlot();
  void zoom(const QRectF &rect);
  void rescale(qreal scaleFactor, QPoint anchorPoint);

  QList<CurveDetailsGroupBox *> detailedPanels() const;
  void setDetailedPanels(const QList<CurveDetailsGroupBox *> &detailedPanels);

  void updateChartSettings(const ChartSettings &newSettings);  

  static QPointF dtToPoint(const UtcDateTime &dt, double y = 0);
  static UtcDateTime pointToDt(const QPointF &p);

  ChartActionsToolBar *getActionsToolBar() const;
  QScrollArea *getPanelCurveDetails() const;
  QList<CurveStyle> getCurveStyles() const;

public slots:
  /// Установить начало выделение интервала
  void setIntervalSelection(bool b);
  /// Полное перерисовка полотна
  void fullReplot();
  /// Установить автоматический масштаб
  void autoZoom();
  /// Отрисовывать сетку
  void setGrid(bool b);
  /// Переместить видимую область графика
  void moveCanvas(int dx, int dy);

signals:
  /// Когда выбрана точка
  void pointSelected(CurveIndex index);
  void targetingDtSet(UtcDateTime dt);
  /// Выбрана первая точка при выделении интервала
  void intervalSelectionStarted(QPointF p);
  /// Выбрана вторая точка при выделении интервала
  void intervalSelectionEnded(QPointF p);
  /// Масштаб графика изменился (фактор масштабирования, точка привязки)
  void scaleChanged(qreal, QPoint);
  /// Видимая область графика была перемещена
  void panned(int, int);

  void zoomed(const QRectF &rect);

protected:
  Ui::ChartWidget *ui;

  ChartActionsToolBar *m_actionsToolBar;
  QList<QwtPlotCurve *> m_curves;
  QList<ChartCurveStats> m_curvesStats;

  QList<PlotInterval *> m_intervals;
  QList<CurveDetailsGroupBox *> m_panelCurveDetailsList;

  QwtPlotGrid *m_pGrid;
  PlotNavigator *m_navigator;
  QwtPlotPicker *m_picker;
  PlotKeyEventHandler *m_keyEventHandler;

  QwtPlotMarker *m_pMinLeftMarker;
  QwtPlotMarker *m_pMaxLeftMarker;
  QwtPlotMarker *m_pMinRightMarker;
  QwtPlotMarker *m_pMaxRightMarker;


  /** Маркеры обозначения выделенной точки.
  * Один маркер для вертикальной, один для горизонтальной линии
  */
  QwtPlotMarker * m_pMarker[2];
  /** Маркеры обозначения выделенного интервала.
  *   Маркеры в виде вертикальной линии
  */
  QwtPlotMarker * m_pIntervalMarker[2];

  /// Маркер для обозначения точки прицеливания
  QwtPlotMarker * m_pTargetingMarker;

  ChartSettings m_settings;
  QList<CurveStyle> m_curveStyles;

  /// Дата и время первой точки
  CurveIndex m_beginLimit;
  /// Дата и время последней точки
  CurveIndex m_endLimit;

  /// Номер текущей выделенной точки
  CurveIndex m_selectedPointIndex;


  /// Дата и время начала выделенного интервала
  UtcDateTime m_intervalBeginDt;
  UtcDateTime m_targetingDt;
  /// Дата и время окончания выделенного интервала
  UtcDateTime m_intervalEndDt;
  /// Номер первой точки в выделенном интервале
  CurveIndex m_selectionBeginPointIdx;
  /// Номер последней точки в выделенном интервале
  CurveIndex m_selectionEndPointIdx;

  /// Текущий выделенный интервал
  bool m_hasSelection;
  TimeSpan m_selInterval;
  ChartSelectionState m_selectionState;

  QTimer * m_timerOnline;
  int m_countLastPoints;

  void updateCurvesIntervalStats();

  QPointF getTransformedPoint(const CurveIndex &index) const;
  QPointF getTransformedPoint(int indexCurve, int indexPoint) const;

  void setIntervalSelectionByState(QPointF pos);

  double calcDistance(const QPointF &p1, const QPointF &p2);

  void createActionsToolBar();
  void createMenuIntervals();

  void createMenuMaxMin();

  virtual void drawMarkerOnCurve(QwtPlot::Axis axis = QwtPlot::yLeft);
  void showSelectionPoint(QwtText xLbl, QwtText yLbl, QPointF point, QwtPlot::Axis axis = QwtPlot::yLeft);

  void showSelectionInterval(QPointF start, QPointF end);

  void showSelectionIntervalStart(QPointF start);
  void showSelectionIntervalEnd(QPointF end);

  long findPointIndexByPos(const QPointF &pos, SearchDirection direction, int curveId);
  CurveIndex findClosestPointAllCurves(const QPointF &pos, SearchDirection direction=sdAny);  

  QVector<QPointF> trimData(const QVector<QPointF> data) const;

private slots:
  /// Установить видимость графика
  void setCurveVisible(bool b);
  /// Установить видимость интервала
  void setIntervalVisible(bool b);
  /// Установить видимость левых линий макс и мин
  void setMaxMinLeftVisible(bool b);
  /// Установить видимость правых линий макс и мин
  void setMaxMinRightVisible(bool b);

  /// Выполнить перерасчет панели детализации
  void calcDetailsPanel();


  /// Изменение цвета левых линий максимум и минимум
  void changeColorMaxMinLeft();
  /// Изменение цвета правых линий макс и мин.
  void changeColorMaxMinRight();
  void onNextCurvePointSelected(bool prev);

  /// По выбору текущей точки
  void onCurvePointSelected(const QPointF& pos);
  void onAction_panelCurveDetails_toggled(bool checked);
  void onAction_grid_toggled(bool checked);
  void onAction_TimerOnline_toggled(bool checked);
  void onAction_SelectTarget_toggled(bool checked);
  void onAction_chartSettings_triggered();

private:
  CurveStyle nextCurveStyle()
  {
    CurveStyle next;
    if (m_curves.size() < m_curveStyles.size())
    {
      next = m_curveStyles.at(m_curves.size());
    }
    else
    {
      if (m_curveStyles.size() < CurveStyle::defColors.size())
        next.pen.setColor(CurveStyle::defColors.at(m_curveStyles.size()));

      m_curveStyles << next;
    }

    return next;
  }

  virtual void updateChartSettingsPrivate() { }
  void updateDetailsPanelsSelPoint();
};

#endif // CHARTWIDGET_H
