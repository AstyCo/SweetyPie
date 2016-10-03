#ifndef CHARTXYWIDGET_H
#define CHARTXYWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QTimer>
#include <QScrollArea>


#include <qwt/qwt_plot.h>

#include "ganttproject_global.h"
#include "plotinterval.h"
#include "curvedetailsgroupbox.h"
#include "plotkeyeventhandler.h"
#include "chartactionstoolbar.h"
#include "chartsettingsdlg.h"
#include "chartintervalselectionmodel.h"

namespace Ui {
class ChartWidget;
}

enum SearchDirection
{
  sdLeft,
  sdRight,
  sdAny
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
class GANTTMODELSHARED_EXPORT ChartXYWidget : public QWidget
{
  Q_OBJECT

  friend class ChartIntervalSelectionModel;

public:
  /// Конструктор класса
  ChartXYWidget(QWidget *parent = 0 ///< Указатель на родительский объект
          );

  /// Деструктор класса
  ~ChartXYWidget();

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


  /// Добавляет специально выделенную зону
  void addZone(const QString &name, double beginX, double endX, const QColor &c1=Qt::white, const QColor &c2=QColor());

  /// Добавляет новый график
  void setData(const QString &title, ///< Первоначальный цвет графика
               const QVector<QPointF> &data, ///< Значения точке графика
               QwtPlot::Axis axis = QwtPlot::yLeft);

  void setLeftAxisMargin(int value);
  void setRightAxisMargin(int value);

  /// Обновляет данные графика. Если таймер запущен то график не перерисовывается.
  void updateData(int indexCurve, const QVector<QPointF> &data);


  ChartCurveStats getCurveStats(int curveId) { return m_curvesStats.at(curveId); }

  bool panelCurveDetailsVisible();
  void setPanelCurveDetailsVisible(bool vis);

  void setChartToolBarVisible(bool vis);

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

  ChartActionsToolBar *getActionsToolBar() const;
  QScrollArea *getPanelCurveDetails() const;
  QList<CurveStyle> getCurveStyles() const;

  /// Показывать ли легенду, если боковая панель скрыта
  bool showLegend() const;
  /// true - показывать легенду, если боковая панель скрыта
  /// false - всегда скрывать
  void setShowLegend(bool show);

  CurveIndex findClosestPointAllCurves(const QPointF &pos, SearchDirection direction=sdAny);

  ChartIntervalSelectionModel *selectionModel() const;



public slots:
  /// Полное перерисовка полотна
  void fullReplot();
  /// Установить автоматический масштаб
  void autoZoom();
  /// Отрисовывать сетку
  void setGrid(bool b);
  /// Переместить видимую область графика
  void moveCanvas(int dx, int dy);

signals: 
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

  QList<PlotInterval *> m_zones;
  QList<QLabel *> m_zoneLegendWidgets;
  QList<CurveDetailsGroupBox *> m_panelCurveDetailsList;
  ChartIntervalSelectionModel *m_selectionModel;

  QwtPlotGrid *m_pGrid;
  PlotNavigator *m_navigator;
  QwtPlotPicker *m_picker;

  QwtPlotMarker *m_pMinLeftMarker;
  QwtPlotMarker *m_pMaxLeftMarker;
  QwtPlotMarker *m_pMinRightMarker;
  QwtPlotMarker *m_pMaxRightMarker;


  ChartSettings m_settings;
  QList<CurveStyle> m_curveStyles;

  QTimer * m_timerOnline;
  int m_countLastPoints;
  /// true - Показывать легенду, если боковая панель скрыта
  /// false - Всегда скрывать легенду
  bool m_showLegend;


  QPointF getTransformedPoint(const CurveIndex &index) const;
  QPointF getTransformedPoint(int indexCurve, int indexPoint) const;

  double calcDistance(const QPointF &p1, const QPointF &p2);

  void createActionsToolBar();
  void createMenuIntervals();

  void createMenuMaxMin();

  long findPointIndexByPos(const QPointF &pos, SearchDirection direction, int curveId);


  QVector<QPointF> trimData(const QVector<QPointF> data) const;

protected slots:
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

  void onAction_panelCurveDetails_toggled(bool checked);
  void onAction_grid_toggled(bool checked);
  void onAction_TimerOnline_toggled(bool checked);

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
};

#endif // CHARTXYWIDGET_H
