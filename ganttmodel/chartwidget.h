#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>

#include "plotmagnifierx.h"
#include "plotkeyeventhander.h"

#include "timespan.h"
#include "utcdatetime.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_marker.h>
#include <qwt_series_data.h>
#include "qwt_scale_draw.h"
#include "qwt_color_map.h"
#include "qwt_raster_data.h"
#include "plotinterval.h"
#include "curvedetailsgroupbox.h"
#include "ganttmodel_global.h"
#include <QTimer>


enum ChartSelectionState
{
  chartNoSelection,
  chartStartSelection,
  chartEndSelection,
  chartIntervalSelected
};

enum SearchDirection
{
  sdLeft,
  sdRight,
  sdAny
};

enum ChartActions{
    caNo=0,
    caScale=1,
    caGrid=2,
    caPaintIntervals=4,
    caSelectIntervals=8,
    caMaxMinLines=16,
    caTimer=32
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
        dt.setBshvTime((int)v);
        return dt.toString("hh:mm:ss");
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
  void selectIntervalByDates(UtcDateTime beginDt, ///< Начало интервала
                             UtcDateTime endDt ///< Конец интервала
                             );

  /// Отменяет выделенный интервал
  void clearSelectedInterval();


  /// Добавляет специально выделенный интервал
  void addInterval(long beginX, long endX, const QColor &c1=Qt::white, const QColor &c2=QColor());

  /// Добавляет специально выделенный интервал
  void addInterval(const UtcDateTime &begin, const UtcDateTime &endX, const QColor &c1, const QColor &c2);

  /// Добавляет новый график
  void setData(const QString &title, ///< Заголовок графика
               const QColor &defaultColor, ///< Первоначальный цвет графика
               const QVector<QPointF> &data, ///< Значения точке графика
               QwtPlot::Axis axis = QwtPlot::yLeft ///< Привязанность к оси
          );

  /// Обновляет данные графика. Если таймер запущен то график не перерисовывается.
  void updateData(int indexCurve, const QVector<QPointF> &data);

  /// Дата и время начала выделенного интервала
  UtcDateTime getSelIntervalBeginDt() { return m_curStartDt; }

  /// Дата и время конца выделенного интервала
  UtcDateTime getSelIntervalEndDt() { return m_curEndDt; }

  /// Переход в режим выделения интервала
  void beginIntervalSelection();

  /// Выделение первой точки интевала
  void setIntervalSelectionStart(QPointF pos);

  /// Выделение второй (последней) точки интервала
  void setIntervalSelectionEnd(QPointF pos);

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

  int chartActons() const;
  void setChartActions(int chartActons);
  QwtPlot * getPlot();

signals:
  /// Когда выбрана точка
  void pointSelected(CurveIndex index);
  /// Выбрана первая точка при выделении интервала
  void intervalSelectionStarted(QPointF p);
  /// Выбрана вторая точка при выделении интервала
  void intervalSelectionEnded(QPointF p);

public slots:
  /// Полное перерисовка полотна
  void fullReplot();
  /// Установить автоматический масштаб
  void autoZoom();
  /// Отрисовывать сетку
  void setGrid(bool b);

private:
  void setIntervalSelectionByState(QPointF pos);

  Ui::ChartWidget *ui;

  QList<QwtPlotCurve *> m_curves;
  QList<PlotInterval *> m_intervals;
  QList<CurveDetailsGroupBox *> m_detailedPanels;

  QwtPlotGrid * m_pGrid;
  QwtPlotPanner *m_panner;
  PlotMagnifierX * m_zoomer;
  QwtPlotPicker * m_picker;
  PlotKeyEventHander * m_keyEventHandler;

  QwtPlotMarker * m_pMinLeftMarker;
  QwtPlotMarker * m_pMaxLeftMarker;
  QwtPlotMarker * m_pMinRightMarker;
  QwtPlotMarker * m_pMaxRightMarker;


  /** Маркеры обозначения выделенной точки.
  * Один маркер для вертикальной, один для горизонтальной линии
  */
  QwtPlotMarker * m_pMarker[2];
  /** Маркеры обозначения выделенного интервала.
  *   Маркеры в виде вертикальной линии
  */
  QwtPlotMarker * m_pIntervalMarker[2];

  /// Дата и время первой точки
  CurveIndex m_beginLimit;
  /// Дата и время последней точки
  CurveIndex m_endLimit;

  /// Номер текущей выделенной точки
  CurveIndex m_selectedPointIndex;


  /// Дата и время начала выделенного интервала
  UtcDateTime m_curStartDt;
  /// Дата и время окончания выделенного интервала
  UtcDateTime m_curEndDt;
  /// Номер первой точки в выделенном интервале
  CurveIndex m_curStartPointIdx;
  /// Номер последней точки в выделенном интервале
  CurveIndex m_curEndPointIdx;

  /// Текущий выделенный интервал
  bool m_intervalValuesVisible;
  TimeSpan m_selInterval;
  ChartSelectionState m_selectionState;

  QTimer * _timerOnline;
  int _countLastPoints;
  int _chartActions;

  QPointF getTransformedPoint(const CurveIndex &index) const;
  QPointF getTransformedPoint(int indexCurve, int indexPoint) const;

  double calcDistance(const QPointF &p1, const QPointF &p2);

  void createMenuIitervals();

  void createMenuMaxMin();

  void DrawMarkerOnCurve(QwtPlot::Axis axis = QwtPlot::yLeft);
  void ShowSelectionPoint(QwtText xLbl, QwtText yLbl, QPointF point, QwtPlot::Axis axis = QwtPlot::yLeft);

  void ShowSelectionInterval(QPointF start, QPointF end);

  void ShowSelectionIntervalStart(QPointF start);
  void ShowSelectionIntervalEnd(QPointF end);

  CurveIndex FindPointIndexByPos(const QPointF &pos, SearchDirection direction=sdAny);

  QPointF DtToPoint(UtcDateTime dt);

  QVector<QPointF> limittedData(const QVector<QPointF> data) const;

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
  /// Установить начало выделение интервала
  void setIntervalSelection(bool b);

  /// Изменение цвета левых линий максимум и минимум
  void changeColorMaxMinLeft();
  /// Изменение цвета правых линий макс и мин.
  void changeColorMaxMinRight();

  /// Что то делает с зумом
  void onPlotPanned();

  /// По выбору текущей точки
  void onCurvePointSelected(const QPointF& pos);

  void on_pushButtonTimerOnline_toggled(bool checked);
};

#endif // CHARTWIDGET_H
