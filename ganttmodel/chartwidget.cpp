#include "chartwidget.h"
#include "ui_chartwidget.h"
#include "curvedetailsgroupbox.h"
#include "math.h"
#include "plotinterval.h"

#include <QColorDialog>
#include <QMenu>


ChartWidget::ChartWidget(QWidget * parent) :
  QWidget(parent),
  ui(new Ui::ChartWidget)
{
    Q_INIT_RESOURCE(images);
  ui->setupUi(this);

  setChartActons(caScale|
                 caGrid|
                 caPaintIntervals|
                 caSelectIntervals|
                 caMaxMinLines);

  ui->m_plot->setCanvasBackground( Qt::white );

  ui->m_plot->setAxisTitle(QwtPlot::xBottom,tr(" Дата и время"));
  ui->m_plot->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
  ui->m_plot->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
  ui->m_plot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);


  // сетка
  m_pGrid = new QwtPlotGrid();

  // возможность перемещения по графику зажимая кнопку мыши
  m_panner = new QwtPlotPanner(ui->m_plot->canvas());
  m_panner->setCursor(QCursor(Qt::ClosedHandCursor));
  m_panner->setMouseButton(Qt::RightButton); // используя правую кнопку мыши
  // добавляем возможность масштабирования колесом мыши
  m_zoomer[0] = new PlotMagnifierX(ui->m_plot->canvas());
  // отключить масштабирование при зажатой кнопке и перемещении мыши
  m_zoomer[0]->setMouseButton(Qt::NoButton);
  m_zoomer[1]=0;




  // создать маркеры, которыми будем помечать выбранную точку
  m_pMarker[0] = new QwtPlotMarker();
  m_pMarker[0]->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
  QwtSymbol * sym = new QwtSymbol(QwtSymbol::Diamond, QBrush(Qt::magenta, Qt::SolidPattern),
                                  QPen(Qt::black, 0.0, Qt::SolidLine), QSize(7,7));
  m_pMarker[0]->setSymbol(sym);
  m_pMarker[0]->setLineStyle(QwtPlotMarker::VLine);
  m_pMarker[0]->setLinePen(QPen( Qt::black, 0, Qt::DashLine ));
  m_pMarker[0]->hide();
  m_pMarker[0]->attach(ui->m_plot);

  m_pMarker[1] = new QwtPlotMarker();
  m_pMarker[1]->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
  m_pMarker[1]->setLineStyle(QwtPlotMarker::HLine);
  m_pMarker[1]->setLinePen(QPen( Qt::black, 0, Qt::DashLine ));
  m_pMarker[1]->hide();
  m_pMarker[1]->attach(ui->m_plot);

  // создать маркеры, которыми будет обозначена область выделения
  m_pIntervalMarker[0] = new QwtPlotMarker();
  m_pIntervalMarker[0]->setLineStyle(QwtPlotMarker::VLine);
  m_pIntervalMarker[0]->setLinePen(QPen( Qt::red, 3, Qt::SolidLine ));
  m_pIntervalMarker[0]->hide();
  m_pIntervalMarker[0]->attach(ui->m_plot);

  m_pIntervalMarker[1] = new QwtPlotMarker();
  m_pIntervalMarker[1]->setLineStyle(QwtPlotMarker::VLine);
  m_pIntervalMarker[1]->setLinePen(QPen( Qt::red, 3, Qt::SolidLine ));
  m_pIntervalMarker[1]->hide();
  m_pIntervalMarker[1]->attach(ui->m_plot);

  m_pMinLeftMarker=0;
  m_pMaxLeftMarker=0;
  m_pMinRightMarker=0;
  m_pMaxRightMarker=0;

  // объект отслеживает перемещение и нажатие кнопки мыши
  m_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                               QwtPicker::CrossRubberBand, QwtPicker::AlwaysOff, ui->m_plot->canvas());
  QwtPickerMachine* pickerMachine = new QwtPickerClickPointMachine();
  m_picker->setStateMachine(pickerMachine);
  // отключить перемещение курсора клавишами влево вправо
  m_picker->setKeyPattern(QwtEventPattern::KeyLeft, Qt::Key_unknown);
  m_picker->setKeyPattern(QwtEventPattern::KeyRight, Qt::Key_unknown);

  m_keyEventHandler = new PlotKeyEventHander(ui->m_plot);
  ui->m_plot->installEventFilter(m_keyEventHandler);

  connect(m_picker, SIGNAL(selected(const QPointF&)), this, SLOT(onCurvePointSelected(const QPointF&)));
  //connect(m_keyEventHandler, SIGNAL(nextPointSelected(bool)), SLOT(onNextCurvePointSelected(bool)));
  connect(m_panner, SIGNAL(panned(int,int)), SLOT(onPlotPanned()));

  connect(ui->pushButtonAutoZoom,SIGNAL(clicked()),this,SLOT(autoZoom()));
  connect(ui->pushButtonGrid,SIGNAL(toggled(bool)),this,SLOT(setGrid(bool)));
  connect(ui->pushButtonClear,SIGNAL(clicked()),this,SLOT(fullReplot()));
  connect(ui->pushButtonSelectInterval,SIGNAL(toggled(bool)),this,SLOT(setIntervalSelection(bool)));


  m_selectedPointIndex = CurveIndex();
  m_selectionState = chartNoSelection;
  m_intervalValuesVisible = false;

  QVBoxLayout * layout = new QVBoxLayout();
  ui->widgetDetail->setLayout(layout);

  createMenuIitervals();
  createMenuMaxMin();

  QList<int> spliterSizes;
  spliterSizes<<60<<40;
  ui->splitter->setSizes(spliterSizes);

  _countLastPoints = 0;
  _timerOnline = new QTimer(this);
  _timerOnline->setInterval(1000);
  connect(_timerOnline,SIGNAL(timeout()),this,SLOT(fullReplot()));


}

ChartWidget::~ChartWidget()
{
    clearChart();
}

void ChartWidget::fullReplot()
{
    // автоматически подобрать масштаб
    ui->m_plot->setAxisAutoScale(QwtPlot::xBottom);
    ui->m_plot->setAxisAutoScale(QwtPlot::yRight);
    ui->m_plot->setAxisAutoScale(QwtPlot::yLeft);
    // скрыть маркеры
    m_pMarker[0]->hide();
    m_pMarker[1]->hide();
    m_pIntervalMarker[0]->hide();
    m_pIntervalMarker[1]->hide();
    // перерисовать
    ui->m_plot->replot();
    // установить новый масштаб в качестве базового
    m_zoomer[0]->SetZoomBase(true);
    if(m_zoomer[1]!=0)
        m_zoomer[1]->SetZoomBase(true);
}

void ChartWidget::autoZoom()
{
    // автоматически подобрать масштаб
    ui->m_plot->setAxisAutoScale(QwtPlot::xBottom);
    ui->m_plot->setAxisAutoScale(QwtPlot::yRight);
    ui->m_plot->setAxisAutoScale(QwtPlot::yLeft);
    ui->m_plot->replot();
}

void ChartWidget::setGrid(bool b)
{
    if(!(_chartActons & caGrid))
    {
        qWarning()<<"grid is bloks. use setChartActions()";
        return;
    }

    if(b)
        m_pGrid->attach(ui->m_plot);
    else
        m_pGrid->detach();

    ui->m_plot->replot();
}

void ChartWidget::ShowSelectionPoint(QwtText xLbl, QwtText yLbl, QPointF point, QwtPlot::Axis axis)
{
  m_pMarker[0]->setLabel(xLbl);
  m_pMarker[0]->setValue(point);
  m_pMarker[0]->show();

  m_pMarker[1]->setLabel(yLbl);
  m_pMarker[1]->setValue(point);
  m_pMarker[1]->setYAxis(axis);
  if(axis==QwtPlot::yRight)
      m_pMarker[1]->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
  else
      m_pMarker[1]->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);

  m_pMarker[1]->show();
  ui->m_plot->replot();
}

void ChartWidget::ShowSelectionInterval(QPointF start, QPointF end)
{
    if(!(_chartActons & caSelectIntervals))
    {
        qWarning()<<"select intervals is bloks. use setChartActions()";
        return;
    }

    m_pIntervalMarker[0]->setValue(start);
    m_pIntervalMarker[0]->show();
    m_pIntervalMarker[1]->setValue(end);
    m_pIntervalMarker[1]->show();
    ui->m_plot->replot();
}
void ChartWidget::ShowSelectionIntervalStart(QPointF start)
{
    if(!(_chartActons & caSelectIntervals))
    {
        qWarning()<<"select intervals is bloks. use setChartActions()";
        return;
    }

    m_pIntervalMarker[0]->setValue(start);
    m_pIntervalMarker[0]->show();
    ui->m_plot->replot();
}

void ChartWidget::ShowSelectionIntervalEnd(QPointF end)
{
    if(!(_chartActons & caSelectIntervals))
    {
        qWarning()<<"select intervals is bloks. use setChartActions()";
        return;
    }

    m_pIntervalMarker[1]->setValue(end);
    m_pIntervalMarker[1]->show();
    ui->m_plot->replot();
}

CurveIndex ChartWidget::FindPointIndexByPos(const QPointF &pos, SearchDirection direction)
{

    if(m_curves.isEmpty())
        return CurveIndex();

    double minLenth=2147483647;
    CurveIndex rez;
    if (direction == sdLeft)
    {
        for(int j=0; j<m_curves.count(); j++)
        {
            if(m_curves[j]->isVisible())
            {
                bool isFind=false;
                for(long i = m_curves[j]->dataSize()-1; i >= 0; i--)
                {
                    QPointF transP = getTransformedPoint(j,i);
                    if (transP.x() <= pos.x())
                    {
                        double lenth = calcDistance(transP,pos);
                        if(lenth<minLenth)
                        {
                            isFind=true;
                            minLenth=lenth;
                            rez.indexCurve = j;
                            rez.indexPoint = i;
                            break;
                        }
                    }
                }
                if(!isFind)
                {
                    QPointF transP = getTransformedPoint(j,0);
                    double lenth = calcDistance(transP,pos);
                    if(lenth<minLenth)
                    {
                        minLenth=lenth;
                        rez.indexCurve = j;
                        rez.indexPoint = 0;
                    }
                }
            }
        }
        return rez;
    }
    else if (direction == sdRight)
    {
        for(int j=0; j<m_curves.count(); j++)
        {
            if(m_curves[j]->isVisible())
            {
                bool isFind=false;
                for(long i = 0; i < m_curves[j]->dataSize(); i++)
                {
                    QPointF transP = getTransformedPoint(j,i);
                    if (transP.x() >= pos.x())
                    {
                        double lenth = calcDistance(transP,pos);
                        if(lenth<minLenth)
                        {
                            isFind=true;
                            minLenth=lenth;
                            rez.indexCurve = j;
                            rez.indexPoint = i;
                            break;
                        }
                    }
                }
                if(!isFind)
                {
                    QPointF transP = getTransformedPoint(j,m_curves[j]->dataSize()-1);
                    double lenth = calcDistance(transP,pos);
                    if(lenth<minLenth)
                    {
                        minLenth=lenth;
                        rez.indexCurve = j;
                        rez.indexPoint =  m_curves[j]->dataSize()-1;
                    }
                }
            }
        }
        return rez;
    }
    else
    {
        for(int j=0; j<m_curves.count(); j++)
        {
            if(m_curves[j]->isVisible())
            {
                bool isFind = false;
                QPointF p, prev;
                for(long i = 0; i < m_curves[j]->dataSize(); i++)
                {
                    p = getTransformedPoint(j,i);
                    if (pos.x() <= p.x())
                    {
                        if (i > 0)
                        {
                            double lenth = calcDistance(prev,pos);
                            if(lenth<minLenth)
                            {
                                isFind=true;
                                minLenth=lenth;
                                rez.indexCurve = j;
                                rez.indexPoint = i-1;
                            }
                        }
                        double lenth = calcDistance(p,pos);
                        if(lenth<minLenth)
                        {
                            isFind=true;
                            minLenth=lenth;
                            rez.indexCurve = j;
                            rez.indexPoint = i;
                        }
                        break;
                    }
                    prev = p;
                }
                if(!isFind)
                {
                    QPointF transP = getTransformedPoint(j,m_curves[j]->dataSize()-1);
                    double lenth = calcDistance(transP,pos);
                    if(lenth<minLenth)
                    {
                        minLenth=lenth;
                        rez.indexCurve = j;
                        rez.indexPoint =  m_curves[j]->dataSize()-1;
                    }
                }
            }
        }
        return rez;
    }
}

void ChartWidget::onCurvePointSelected(const QPointF &pos)
{
  if ((m_selectionState == chartNoSelection) || (m_selectionState == chartIntervalSelected))
  {

    // найти и выделить точку с учетом выделенного интервала
    CurveIndex selPointIdx = FindPointIndexByPos(pos);

    if(!selPointIdx.isValid())
        return;

    m_selectedPointIndex = selPointIdx;

    for(int i=0; i<m_curves.count(); i++)
    {
        m_curves[i]->setPen(QPen(m_curves[i]->pen().color(),0.8));
    }
    m_curves[m_selectedPointIndex.indexCurve]->setPen(QPen(m_curves[m_selectedPointIndex.indexCurve]->pen().color(),2.4));

    if(  m_pMinLeftMarker!=0 &&
         m_pMaxLeftMarker!=0)
    {
        m_pMinLeftMarker->setLinePen(QPen(m_pMinLeftMarker->linePen().color(),1));
        m_pMaxLeftMarker->setLinePen(QPen(m_pMaxLeftMarker->linePen().color(),1));

    }
    if(m_pMinRightMarker!=0 &&
       m_pMaxRightMarker!=0)
    {
        m_pMinRightMarker->setLinePen(QPen(m_pMinRightMarker->linePen().color(),1));
        m_pMaxRightMarker->setLinePen(QPen(m_pMaxRightMarker->linePen().color(),1));
    }

    if(m_curves[m_selectedPointIndex.indexCurve]->yAxis()==QwtPlot::yLeft)
    {
        if(m_pMinLeftMarker!=0 && m_pMaxLeftMarker!=0)
        {
            m_pMinLeftMarker->setLinePen(QPen(m_pMinLeftMarker->linePen().color(),3));
            m_pMaxLeftMarker->setLinePen(QPen(m_pMaxLeftMarker->linePen().color(),3));
        }
    }
    else if(m_curves[m_selectedPointIndex.indexCurve]->yAxis()==QwtPlot::yRight)
    {
        if(m_pMinRightMarker!=0 && m_pMaxRightMarker!=0)
        {
            m_pMinRightMarker->setLinePen(QPen(m_pMinRightMarker->linePen().color(),3));
            m_pMaxRightMarker->setLinePen(QPen(m_pMaxRightMarker->linePen().color(),3));
        }
    }
    //m_zoomer->setYAxis((QwtPlot::Axis)m_curves[m_selectedPointIndex.indexCurve]->yAxis());
    DrawMarkerOnCurve((QwtPlot::Axis)m_curves[m_selectedPointIndex.indexCurve]->yAxis());

    calcDetailsPanel();

    emit pointSelected(m_selectedPointIndex);
  }
  else if (m_selectionState == chartStartSelection)
  {
    setIntervalSelectionStart(pos);

    emit intervalSelectionStarted(pos);
  }
  else if (m_selectionState == chartEndSelection)
  {
    setIntervalSelectionEnd(pos);

    emit intervalSelectionEnded(pos);
  }
}


//void ChartWidget::onNextCurvePointSelected(bool prev)
//{
//  QwtSeriesData<QPointF> *samples = m_curve->data();

//  if (samples->size() == 0)
//    return;

////  int step = m_settings.keyStep;

//  long startIdx = 0;
//  long endIdx = samples->size() - 1;
//  // если выбран интервал, то ограничиваем зону поиска точками из этого интервала
//  if (m_selectionState == chartIntervalSelected)
//  {
//    startIdx = m_curStartPointIdx;
//    endIdx = m_curEndPointIdx;
//  }
//  if (startIdx > endIdx)
//  {
//    // ни одна точка не попала в интервал
//    return;
//  }
//  if (m_selectedPointIndex > -1)
//  {
//    // предыдущая точка имеется
//    if (!prev)
//    {

//      if ((m_selectedPointIndex + step) < endIdx)
//        m_selectedPointIndex += step;
//      else
//        m_selectedPointIndex = endIdx;
//    }
//    else
//    {
//      if ((m_selectedPointIndex - step) >= startIdx)
//        m_selectedPointIndex -= step;
//      else
//        m_selectedPointIndex = startIdx;
//    }
//  }
//  else
//  {
//    // предыдущая точка отсутствует
//    if (!prev)
//      m_selectedPointIndex = step;
//    else
//      m_selectedPointIndex = endIdx - step;

//    if (m_selectedPointIndex < startIdx)
//      m_selectedPointIndex = startIdx;
//    else if (m_selectedPointIndex > endIdx)
//      m_selectedPointIndex = endIdx;
//  }

//  DrawMarkerOnCurve();

//  emit pointSelected(m_selectedPointIndex);
//}


//void ChartWidget::updateLabelAxisX()
//{
//   QString t;
//  QString dim;
//  QString xName;

//  TimeDimensions axisX = m_settings.axisX;

//  switch (axisX)
//  {
//  case tSeconds: t = tr("[сек.]"); break;
//  case tMinutes: t = tr("[мин.]"); break;
//  case tHours: t = tr("[часы]"); break;
//  case tDays: t = tr("[сутки]"); break;
//  }

//  dim = t;
//  xName = tr("Относит. время");

//  QwtText title(t);
//  title.setFont(m_settings.m_markerLabelFont);

//  ui->m_plot->setAxisTitle(QwtPlot::xBottom, title);
//  m_panelControls->m_titleLabels.at(LABEL_X_VALUE)->setText(xName);
//  m_panelControls->m_dimLabels.at(LABEL_X_VALUE)->setText(dim);
//}


QPointF ChartWidget::DtToPoint(UtcDateTime dt)
{
    double offsetMin = dt.toBshvTime();
    return QPointF(offsetMin , 0);
}

QVector<QPointF> ChartWidget::limittedData(const QVector<QPointF> data) const
{
    if(_countLastPoints==0 || _countLastPoints>data.size())
        return data;
    else
    {
        QVector<QPointF> tmp;
        tmp = data.mid(data.size()-_countLastPoints);
        return tmp;

    }
}
int ChartWidget::chartActons() const
{
    return _chartActons;
}

void ChartWidget::setChartActons(int chartActons)
{
    ui->pushButtonAutoZoom->setVisible(false);
    ui->pushButtonGrid->setVisible(false);
    ui->toolButtonIntervalVisibled->setVisible(false);
    ui->pushButtonSelectInterval->setVisible(false);
    ui->toolButtonIntervalVisibled->setVisible(false);
    ui->pushButtonTimerOnline->setVisible(false);

    _chartActons = chartActons;

    // TODO блокировки самих элементов и функций на графике
    if(_chartActons & caScale)
        ui->pushButtonAutoZoom->setVisible(true);
    if(_chartActons & caGrid)
        ui->pushButtonGrid->setVisible(true);
    if(_chartActons & caPaintIntervals)
        ui->toolButtonIntervalVisibled->setVisible(true);
    if(_chartActons & caSelectIntervals)
        ui->pushButtonSelectInterval->setVisible(true);
    if(_chartActons & caMaxMinLines)
        ui->toolButtonIntervalVisibled->setVisible(true);
    if(_chartActons & caTimer)
        ui->pushButtonTimerOnline->setVisible(true);
}


//void ChartWidget::updateLabelAxisY()
//{

//  DistanceDimensions axisY_R = m_settings.axisY_R;
//  DistanceDimensions axisY_V = m_settings.axisY_V;
//  TimeDimensions axisY_T = m_settings.axisY_T;

//  QString yR, yRV, yT;
//  switch (axisY_R)
//  {
//    case dMillimeters: yR = tr("[мм]"); yRV = tr("[мм/c]"); break;
//    case dMeters: yR = tr("[м]"); yRV = tr("[м/c]"); break;
//    case dKilometers: yR = tr("[км]"); yRV = tr("[км/c]"); break;
//    case d1000Kilometers: yR = tr("[1000км]"); yRV = tr("[км/c]"); break;
//  }
//  switch (axisY_V)
//  {
//    case dMillimeters: yRV = tr("[мм/c]"); break;
//    case dMeters: yRV = tr("[м/c]"); break;
//    default: yRV = tr("[км/c]"); break;
//  }
//  switch (axisY_T)
//  {
//    case tSeconds: yT = tr("[сек.]"); break;
//    case tMinutes: yT = tr("[мин.]"); break;
//    case tHours: yT = tr("[часы]"); break;
//    case tDays: yT = tr("[сутки]"); break;
//  }



//  QList<QLabel *> &dims = m_panelControls->m_dimLabels;
//  int idx = 0;
//  foreach (QLabel * dim, dims)
//  {
//    if ((dim != NULL) && (idx != LABEL_X_VALUE))
//      dim->setText(yText);

//    idx++;
//  }
//}

void ChartWidget::setIntervalSelectionByState(QPointF pos)
{
    if(m_curves.isEmpty())
        return;

  QPointF p = pos;
  CurveIndex selPointIdx;

  UtcDateTime selDt;
  selDt.setBshvTime(p.x());

  if (!selDt.isValid())
    return;

  if (p.x() < m_curves.at(m_beginLimit.indexCurve)->sample(m_beginLimit.indexPoint).x())
  {
    selDt.setBshvTime(m_curves.at(m_beginLimit.indexCurve)->sample(m_beginLimit.indexPoint).x());
  }
  else if (p.x() > m_curves.at(m_endLimit.indexCurve)->sample(m_endLimit.indexPoint).x())
  {
      selDt.setBshvTime(m_curves.at(m_endLimit.indexCurve)->sample(m_endLimit.indexPoint).x());
  }

  //this->SetSelectionIntervalDates(selDt, m_curEndDt);
  int markerIdx = 0;
  if (m_selectionState == chartStartSelection)
  {
    selPointIdx = FindPointIndexByPos(p, sdRight);
    m_curStartPointIdx = selPointIdx;
    m_curStartDt = selDt;
  }
  else if (m_selectionState == chartEndSelection)
  {
    if (m_curStartDt > selDt)
    {
      // если конечная дата на самом деле является началом
      // интервала, то меняем местами
      m_curEndDt = m_curStartDt;
      m_curStartDt = selDt;

      // текущие маркеры начала заменяем на маркеры окончания
      m_pIntervalMarker[1]->setValue(m_pIntervalMarker[0]->value());
      m_pIntervalMarker[1]->show();
      ui->m_plot->replot();

      markerIdx = 0;
      m_curStartPointIdx = FindPointIndexByPos(p, sdRight);
      QPointF tmpP(m_curEndDt.toBshvTime(),p.y());
      m_curEndPointIdx = FindPointIndexByPos(tmpP, sdLeft);
    }
    else
    {
      m_curEndDt = selDt;
      selPointIdx = FindPointIndexByPos(p, sdLeft);
      m_curEndPointIdx = selPointIdx;
      markerIdx = 1;
    }

    // обновить поле "длительность"
    //TimeSpan duration = selDt - m_curStartDt;
    //this->SetSelectionInterval(duration);

    // обновить значения мин. сред. макс. для выбранного интервала
    //SetMinMeanMaxLbls(true);
  }
  // показать маркер начала/окончания выделения интервала
  m_pIntervalMarker[markerIdx]->setValue(p);
  m_pIntervalMarker[markerIdx]->show();
  ui->m_plot->replot();
}
int ChartWidget::countLastPoints() const
{
    return _countLastPoints;
}

void ChartWidget::setCountLastPoints(int countLastPoints)
{
    _countLastPoints = countLastPoints;
}


QList<QwtPlotCurve *> ChartWidget::curves() const
{
    return m_curves;
}

void ChartWidget::startOnlineReplot()
{
    on_pushButtonTimerOnline_toggled(true);
}

void ChartWidget::stopOnlineReplot()
{
    on_pushButtonTimerOnline_toggled(false);
}

QList<PlotInterval *> ChartWidget::intervals() const
{
    return m_intervals;
}


QPointF ChartWidget::getTransformedPoint(int indexCurve, int indexPoint) const
{
    if(m_curves[indexCurve]->yAxis()==QwtPlot::yRight)
    {

        return QPointF(m_curves[indexCurve]->sample(indexPoint).x(),
                   ui->m_plot->invTransform(QwtPlot::yLeft,ui->m_plot->transform(QwtPlot::yRight,m_curves[indexCurve]->sample(indexPoint).y())));
    }
    else
    {
        return m_curves[indexCurve]->sample(indexPoint);
    }
}

QPointF ChartWidget::getTransformedPoint(const CurveIndex &index) const
{
    return getTransformedPoint(index.indexCurve,index.indexPoint);
}

double ChartWidget::calcDistance(const QPointF &p1, const QPointF &p2)
{
    double x2 = (p2.x() - p1.x())*(p2.x() - p1.x());
    double y2 = (p2.y() - p1.y())*(p2.y() - p1.y());
    return sqrt(x2+y2);
}

void ChartWidget::createMenuIitervals()
{

    if(!(_chartActons & caPaintIntervals))
    {
        qWarning()<<"intervals is bloks. use setChartActions()";
        return;
    }
    if(ui->toolButtonIntervalVisibled->menu()!=0)
        delete ui->toolButtonIntervalVisibled->menu();

    QMenu * menu = new QMenu(ui->toolButtonIntervalVisibled);
    QList<QAction *> rez;

    for(int i=0; i<m_intervals.count(); i++)
    {
        UtcDateTime beg;
        beg.setBshvTime(m_intervals[i]->beginX());
        UtcDateTime end;
        end.setBshvTime(m_intervals[i]->endX());
        QAction *a = new QAction(beg.toStdString(0)+"-"+end.toStdString(0),ui->toolButtonIntervalVisibled);
        a->setCheckable(true);
        a->setChecked(true);
        connect(a,SIGNAL(toggled(bool)),this,SLOT(setIntervalVisible(bool)));
        rez.append(a);
    }

    menu->clear();
    menu->addActions(rez);

    if(rez.isEmpty())
        ui->toolButtonIntervalVisibled->setVisible(false);
    else
    {
        ui->toolButtonIntervalVisibled->setVisible(true);
        ui->toolButtonIntervalVisibled->setMenu(menu);
    }

}

void ChartWidget::createMenuMaxMin()
{
    if(!(_chartActons & caMaxMinLines))
    {
        qWarning()<<"select intervals is bloks. use setChartActions()";
        return;
    }

    if(ui->toolButtonMaxMinValues->menu()!=0)
        delete ui->toolButtonMaxMinValues->menu();

    QMenu * menu = new QMenu(ui->toolButtonMaxMinValues);
    QList<QAction *> rez;


    if(m_pMaxLeftMarker!=0 && m_pMinLeftMarker!=0)
    {
        QAction *amin = new QAction(tr("Пределы левой оси"),ui->toolButtonMaxMinValues);
        amin->setCheckable(true);
        amin->setChecked(true);
        connect(amin,SIGNAL(toggled(bool)),this,SLOT(setMaxMinLeftVisible(bool)));
        rez.append(amin);

        QAction *acmin = new QAction(tr("Цвет пределов левой оси"),ui->toolButtonMaxMinValues);
        connect(acmin,SIGNAL(triggered()),this,SLOT(changeColorMaxMinLeft()));
        rez.append(acmin);
    }



    if(m_pMaxRightMarker!=0 && m_pMinRightMarker!=0)
    {
        QAction *amin = new QAction(tr("Пределы правой оси"),ui->toolButtonMaxMinValues);
        amin->setCheckable(true);
        amin->setChecked(true);
        connect(amin,SIGNAL(toggled(bool)),this,SLOT(setMaxMinRightVisible(bool)));
        rez.append(amin);

        QAction *acmin = new QAction(tr("Цвет пределов правой оси"),ui->toolButtonMaxMinValues);
        connect(acmin,SIGNAL(triggered()),this,SLOT(changeColorMaxMinRight()));
        rez.append(acmin);
    }


    menu->clear();
    menu->addActions(rez);

    if(rez.isEmpty())
        ui->toolButtonMaxMinValues->setVisible(false);
    else
    {
        ui->toolButtonMaxMinValues->setVisible(true);
        ui->toolButtonMaxMinValues->setMenu(menu);
    }

}
QwtPlotCurve *ChartWidget::curve(int index) const
{
    return m_curves[index];
}


void ChartWidget::setCurveVisible(bool b)
{
    if(!b)
    {
        CurveDetailsGroupBox *curveDlg = (CurveDetailsGroupBox *)QObject::sender();
        bool isFind = false;
        for(int i=0; i<m_curves.count(); i++)
            if(m_curves[i] == curveDlg->curve())
                if(m_selectedPointIndex.indexCurve == i)
                {
                    isFind = true;
                    fullReplot();
                }

        if(!isFind)
            ui->m_plot->replot();
    }
    else
    {
        ui->m_plot->replot();
    }
}

void ChartWidget::setIntervalVisible(bool b)
{
    QAction * a = (QAction *)QObject::sender();
    int index = ui->toolButtonIntervalVisibled->menu()->actions().indexOf(a);
    if(index>=0 && index<m_intervals.count())
    {
        m_intervals[index]->setVisible(b);
        ui->m_plot->replot();
    }
}

void ChartWidget::setMaxMinLeftVisible(bool b)
{
    QAction * a = (QAction *)QObject::sender();
    if(a->text()==tr("Пределы левой оси"))
    {
        m_pMaxLeftMarker->setVisible(b);
        m_pMinLeftMarker->setVisible(b);
        ui->m_plot->replot();
    }
}

void ChartWidget::setMaxMinRightVisible(bool b)
{
    QAction * a = (QAction *)QObject::sender();
    if(a->text()==tr("Пределы правой оси"))
    {
        m_pMaxRightMarker->setVisible(b);
        m_pMinRightMarker->setVisible(b);
        ui->m_plot->replot();
    }
}

void ChartWidget::calcDetailsPanel()
{
    for(int i=0; i<m_detaildPanels.count(); i++)
    {
        if(m_detaildPanels[i]->curve()->yAxis()==QwtPlot::yLeft)
        {
            if(m_pMaxLeftMarker!=0 && m_pMinLeftMarker!=0)
                m_detaildPanels[i]->setMinMaxValue(m_pMinLeftMarker->yValue(),m_pMaxLeftMarker->yValue());
            else
                m_detaildPanels[i]->setMinMaxValue(0,0);
        }
        else if(m_detaildPanels[i]->curve()->yAxis()==QwtPlot::yRight)
        {
            if(m_pMaxRightMarker!=0 && m_pMinRightMarker!=0)
                m_detaildPanels[i]->setMinMaxValue(m_pMinRightMarker->yValue(),m_pMaxRightMarker->yValue());
            else
                m_detaildPanels[i]->setMinMaxValue(0,0);
        }
        else
            m_detaildPanels[i]->setMinMaxValue(0,0);

        if(m_selectedPointIndex.isValid())
        {
            if(m_curves[m_selectedPointIndex.indexCurve]==m_detaildPanels[i]->curve())
            {
                m_detaildPanels[i]->setCurrentIndex(m_selectedPointIndex.indexPoint);
            }
            else
            {
                m_detaildPanels[i]->setCurrentIndex(-1);
            }
        }
        else
            m_detaildPanels[i]->setCurrentIndex(-1);




        if(m_selectionState==chartIntervalSelected)
        {
            m_detaildPanels[i]->setInterval(m_curStartDt.toBshvTime(),m_curEndDt.toBshvTime());
        }
        else
        {
            m_detaildPanels[i]->setInterval(0,0);
        }

    }
}

void ChartWidget::setIntervalSelection(bool b)
{
    if(b)
    {
        beginIntervalSelection();
    }
    else
    {
        if(m_selectionState == chartEndSelection)
            clearSelectedInterval();
    }
}

void ChartWidget::changeColorMaxMinLeft()
{
    QAction * a = (QAction *)QObject::sender();
    if(a->text()==tr("Цвет пределов левой оси"))
    {
        QColor color;
        if(m_pMaxLeftMarker->linePen().color()==Qt::black)
            color = QColorDialog::getColor(Qt::white,this);
        else
            color = QColorDialog::getColor(m_pMaxLeftMarker->linePen().color(),this);
        if(color.isValid())
        {
            m_pMaxLeftMarker->setLinePen(QPen(color,m_pMaxLeftMarker->linePen().width()));
            m_pMinLeftMarker->setLinePen(QPen(color,m_pMinLeftMarker->linePen().width()));

            ui->m_plot->replot();
        }
    }
}

void ChartWidget::changeColorMaxMinRight()
{
    QAction * a = (QAction *)QObject::sender();
    if(a->text()==tr("Цвет пределов правой оси"))
    {
        QColor color;
        if(m_pMaxRightMarker->linePen().color()==Qt::black)
            color = QColorDialog::getColor(Qt::white,this);
        else
            color = QColorDialog::getColor(m_pMaxRightMarker->linePen().color(),this);
        if(color.isValid())
        {
            m_pMaxRightMarker->setLinePen(QPen(color,m_pMaxRightMarker->linePen().width()));
            m_pMinRightMarker->setLinePen(QPen(color,m_pMinRightMarker->linePen().width()));

            ui->m_plot->replot();
        }
    }
}

void ChartWidget::onPlotPanned()
{
    m_zoomer[0]->SetZoomBase(true);
    if(m_zoomer[1]!=0)
        m_zoomer[1]->SetZoomBase(true);
}


void ChartWidget::setData(const QString &title, const QColor &defaultColor, const QVector<QPointF> &data, QwtPlot::Axis axis)
{
    if(data.isEmpty())
        return;

    QwtPlotCurve * curve = new QwtPlotCurve();

    curve->setTitle(title);


    curve->setYAxis(axis);
    curve->setPen(QPen(defaultColor));
    curve->setSamples(limittedData(data));
    curve->attach(ui->m_plot);

    CurveDetailsGroupBox * details = new CurveDetailsGroupBox(curve,this);
    connect(details,SIGNAL(colorChanged(QColor)),ui->m_plot,SLOT(replot()));
    connect(details,SIGNAL(visibledChanged(bool)),this,SLOT(setCurveVisible(bool)));
    QVBoxLayout* lay = (QVBoxLayout*) ui->widgetDetail->layout();
    lay->insertWidget(1,details);

    m_detaildPanels.append(details);

    calcDetailsPanel();

    if(m_curves.isEmpty())
    {
        m_beginLimit.indexCurve = m_curves.size();
        m_beginLimit.indexPoint = 0;

        m_endLimit.indexCurve = m_curves.size();
        m_endLimit.indexPoint = curve->dataSize()-1;
    }
    else
    {
        if(curve->sample(0).x()<m_curves[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint).x())
        {
            m_beginLimit.indexCurve = m_curves.size();
            m_beginLimit.indexPoint = 0;
        }

        if(curve->sample(curve->dataSize()-1).x()>m_curves[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint).x())
        {
            m_endLimit.indexCurve = m_curves.size();
            m_endLimit.indexPoint = curve->dataSize()-1;
        }
    }

    m_curves.append(curve);

    fullReplot();
}

void ChartWidget::updateData(int indexCurve, const QVector<QPointF> &data)
{

    m_curves[indexCurve]->setSamples(limittedData(data));

    if(!_timerOnline->isActive())
        fullReplot();
}

void ChartWidget::clearChart()
{
    for(int i=0; i<m_curves.count(); i++)
        m_curves[i]->detach();
    m_curves.clear();

    for(int i=0; i<m_intervals.count(); i++)
        m_intervals[i]->detach();
    m_intervals.clear();

    for(int i=0; i<m_detaildPanels.count(); i++)
        delete m_detaildPanels[i];
    m_detaildPanels.clear();

  m_selectedPointIndex = CurveIndex();
  m_selectionState = chartNoSelection;
  m_curStartPointIdx = CurveIndex();
  m_curEndPointIdx = CurveIndex();
  ui->m_plot->replot();
}

//void ChartWidget::UpdateChartSettings(ChartSettings newSettings)
//{
//  m_settings = newSettings;
//  redrawCurve();

//  if (newSettings.grid)
//    AttachGrid();
//  else
//    DetachGrid();

//  m_curve->setPen(QPen( newSettings.color_1, newSettings.width ));
//  ui->m_plot->setAxisFont(QwtPlot::xBottom, newSettings.m_markerLabelFont);
//  ui->m_plot->setAxisFont(QwtPlot::yLeft, newSettings.m_markerLabelFont);

//  updateLabelAxisX();
//  updateLabelAxisY();

//  setDetailsPaneVisible(newSettings.detailsPanelVisible);
//}

void ChartWidget::DrawMarkerOnCurve(QwtPlot::Axis axis)
{
  if (!m_selectedPointIndex.isValid())
    return;

  QwtText txtX, txtY;

  QPointF p = m_curves[m_selectedPointIndex.indexCurve]->sample(m_selectedPointIndex.indexPoint);
  QString valY = QString::number(p.y(), 'f', 6);
  //QString valX = QString::number(p.x());
  //QString axisTitleX = ui->m_plot->axisTitle(QwtPlot::xBottom).text();
  QString axisTitleY = ui->m_plot->axisTitle(QwtPlot::yLeft).text();

  UtcDateTime refDt;
  refDt.setBshvTime(p.x());
  QString refDtStr = refDt.toStdString(3);

  txtX = tr("[%1]").arg(refDtStr);

  txtY = QString("[%1 %2]").arg(valY, axisTitleY);

  ShowSelectionPoint(txtX, txtY, p, axis);

}

UtcDateTime ChartWidget::minimumDt()
{
    UtcDateTime rez;
    rez.setBshvTime(m_curves[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint).x());
    return rez;
}

UtcDateTime ChartWidget::maximumDt()
{
    UtcDateTime rez;
    rez.setBshvTime(m_curves[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint).x());
    return rez;
}

void ChartWidget::setLeftAxis(const QString &title, int minLine, int maxLine, const QColor &defaultColor)
{
    ui->m_plot->setAxisTitle(QwtPlot::yLeft,title);
    ui->m_plot->setAxisLabelRotation(QwtPlot::yLeft, -50.0);
    ui->m_plot->setAxisLabelAlignment(QwtPlot::yLeft, Qt::AlignLeft | Qt::AlignBottom);

    setLeftMinMaxValues(minLine,maxLine,defaultColor);


}

void ChartWidget::setRightAxis(const QString &title, int minLine, int maxLine, const QColor &defaultColor)
{
    ui->m_plot->enableAxis( QwtPlot::yRight );
    ui->m_plot->setAxisTitle(QwtPlot::yRight,title);
    ui->m_plot->setAxisLabelRotation(QwtPlot::yRight, -50.0);
    ui->m_plot->setAxisLabelAlignment(QwtPlot::yRight, Qt::AlignLeft | Qt::AlignBottom);

    setRightMinMaxValues(minLine,maxLine,defaultColor);

    m_zoomer[1] = new PlotMagnifierX(ui->m_plot->canvas());
    // отключить масштабирование при зажатой кнопке и перемещении мыши
    m_zoomer[1]->setMouseButton(Qt::NoButton);
    m_zoomer[1]->setYAxis(QwtPlot::yRight);
}

void ChartWidget::setLeftMinMaxValues(int minLine, int maxLine, const QColor &defaultColor)
{
    if(!(_chartActons & caPaintIntervals))
    {
        qWarning()<<"intervals is bloks. use setChartActions()";
        return;
    }

    if(minLine!=0 || maxLine!=0)
    {
        m_pMinLeftMarker = new QwtPlotMarker();
        m_pMinLeftMarker->setLineStyle(QwtPlotMarker::HLine);
        m_pMinLeftMarker->setLinePen(QPen(defaultColor, 1, Qt::SolidLine ));
        m_pMinLeftMarker->setYAxis(QwtPlot::yLeft);
        m_pMinLeftMarker->setYValue(minLine);
        m_pMinLeftMarker->setLabel("{"+QString::number(minLine)+"}");
        m_pMinLeftMarker->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
        m_pMinLeftMarker->attach(ui->m_plot);

        m_pMaxLeftMarker = new QwtPlotMarker();
        m_pMaxLeftMarker->setLineStyle(QwtPlotMarker::HLine);
        m_pMaxLeftMarker->setLinePen(QPen(defaultColor, 1, Qt::SolidLine ));
        m_pMaxLeftMarker->setYAxis(QwtPlot::yLeft);
        m_pMaxLeftMarker->setYValue(maxLine);
        m_pMaxLeftMarker->setLabel("{"+QString::number(maxLine)+"}");
        m_pMaxLeftMarker->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
        m_pMaxLeftMarker->attach(ui->m_plot);

        createMenuMaxMin();
        calcDetailsPanel();
    }
}

void ChartWidget::setRightMinMaxValues(int minLine, int maxLine, const QColor &defaultColor)
{
    if(!(_chartActons & caMaxMinLines))
    {
        qWarning()<<"min max lines is bloks. use setChartActions()";
        return;
    }

    if((minLine!=0 || maxLine!=0) && ui->m_plot->axisEnabled(QwtPlot::yRight ))
    {
        m_pMinRightMarker = new QwtPlotMarker();
        m_pMinRightMarker->setLineStyle(QwtPlotMarker::HLine);
        m_pMinRightMarker->setLinePen(QPen(defaultColor, 1, Qt::SolidLine ));
        m_pMinRightMarker->setYAxis(QwtPlot::yRight);
        m_pMinRightMarker->setYValue(minLine);
        m_pMinRightMarker->setLabel("{"+QString::number(minLine)+"}");
        m_pMinRightMarker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
        m_pMinRightMarker->attach(ui->m_plot);

        m_pMaxRightMarker = new QwtPlotMarker();
        m_pMaxRightMarker->setLineStyle(QwtPlotMarker::HLine);
        m_pMaxRightMarker->setLinePen(QPen(defaultColor, 1, Qt::SolidLine ));
        m_pMaxRightMarker->setYAxis(QwtPlot::yRight);
        m_pMaxRightMarker->setYValue(maxLine);
        m_pMaxRightMarker->setLabel("{"+QString::number(maxLine)+"}");
        m_pMaxRightMarker->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
        m_pMaxRightMarker->attach(ui->m_plot);
        m_pMaxRightMarker->show();

        createMenuMaxMin();
        calcDetailsPanel();
    }
}

void ChartWidget::selectIntervalByDates(UtcDateTime beginDt, UtcDateTime endDt)
{

    if(!(_chartActons & caSelectIntervals))
    {
        qWarning()<<"select intervals is bloks. use setChartActions()";
        return;
    }

    if (beginDt > endDt)
    {
        UtcDateTime tmpDt(beginDt);
        beginDt = endDt;
        endDt = tmpDt;
    }
    if(!m_beginLimit.isValid() || !m_endLimit.isValid())
        return;

    if (beginDt.toBshvTime() < m_curves[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint).x())
    {
        beginDt.setBshvTime(m_curves[m_beginLimit.indexCurve]->sample(m_beginLimit.indexPoint).x());
    }
    if (endDt.toBshvTime() > m_curves[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint).x())
    {
        endDt.setBshvTime(m_curves[m_endLimit.indexCurve]->sample(m_endLimit.indexPoint).x());
    }

    m_selectionState = chartIntervalSelected;

    QPointF startP = DtToPoint(beginDt);
    QPointF endP = DtToPoint(endDt);

    m_curStartPointIdx = FindPointIndexByPos(startP, sdRight);
    m_curEndPointIdx = FindPointIndexByPos(endP, sdLeft);

    ShowSelectionInterval(startP, endP);

    calcDetailsPanel();
}

void ChartWidget::clearSelectedInterval()
{
  m_selectionState = chartNoSelection;
  m_pIntervalMarker[0]->hide();
  m_pIntervalMarker[1]->hide();
  ui->m_plot->replot();
}

void ChartWidget::setIntervalSelectionStart(QPointF pos)
{
    if(!(_chartActons & caSelectIntervals))
    {
        qWarning()<<"select intervals is bloks. use setChartActions()";
        return;
    }

    m_selectionState = chartStartSelection;
    setIntervalSelectionByState(pos);
    m_selectionState = chartEndSelection;
    emit intervalSelectionStarted(pos);
}

void ChartWidget::setIntervalSelectionEnd(QPointF pos)
{
    if(!(_chartActons & caSelectIntervals))
    {
        qWarning()<<"select intervals is bloks. use setChartActions()";
        return;
    }

    m_selectionState = chartEndSelection;
    setIntervalSelectionByState(pos);
    m_selectionState = chartIntervalSelected;
    ui->pushButtonSelectInterval->setChecked(false);
    calcDetailsPanel();
    emit intervalSelectionEnded(pos);
}

void ChartWidget::addInterval(long beginX, long endX, const QColor &c1, const QColor &c2)
{
    if(!(_chartActons & caPaintIntervals))
    {
        qWarning()<<"intervals is bloks. use setChartActions()";
        return;
    }

    PlotInterval * interval = new PlotInterval();

    if(!c2.isValid())
        interval->setBackground(c1);
    else
        interval->setGradient(c1,c2);

    interval->setBeginX(beginX);
    interval->setEndX(endX);
    interval->attach(ui->m_plot);

    m_intervals.append(interval);
    createMenuIitervals();
}

void ChartWidget::addInterval(const UtcDateTime &begin, const UtcDateTime &endX, const QColor &c1, const QColor &c2)
{
    addInterval(begin.toBshvTime(),endX.toBshvTime(),c1,c2);
}

void ChartWidget::beginIntervalSelection()
{
    if(!(_chartActons & caSelectIntervals))
    {
        qWarning()<<"select intervals is bloks. use setChartActions()";
        return;
    }

    clearSelectedInterval();
    m_selectionState = chartStartSelection;
}

void ChartWidget::on_pushButtonTimerOnline_toggled(bool checked)
{
    if(!(_chartActons & caTimer))
    {
        qWarning()<<"timer is bloks. use setChartActions()";
        return;
    }

    if(checked)
    {
        ui->pushButtonTimerOnline->setIcon(QIcon(":/icons/icons/stop.png"));
        _timerOnline->start();
    }
    else
    {
        ui->pushButtonTimerOnline->setIcon(QIcon(":/icons/icons/start.png"));
        _timerOnline->stop();
    }
}
