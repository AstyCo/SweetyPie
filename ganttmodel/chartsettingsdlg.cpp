#include "chartsettingsdlg.h"
#include "ui_chartsettingsdlg.h"
#include <QPushButton>
//#include "helpclient.h"
#include <QColorDialog>
#include <QFontDialog>

#include <qwt/qwt_plot.h>

ChartSettingsDlg::ChartSettingsDlg(const ChartSettings &gs, const QList<QwtPlotCurve*> &curves, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::GraphSettingsDlg)
{
  ui->setupUi(this);

  ui->buttonBox_OkCancel->button(QDialogButtonBox::Ok)->setText(tr("Применить"));
  ui->buttonBox_OkCancel->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/icons/icons/apply.png"));
  ui->buttonBox_OkCancel->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
  ui->buttonBox_OkCancel->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/icons/icons/denial.png"));

  setWindowFlags((this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowContextHelpButtonHint);
  ui->tabWidget_settings->setCurrentIndex(0);

  m_settings = gs;
  m_curves = curves;

  ui->comboBox_R->setCurrentIndex(gs.axisY_R);
  ui->comboBox_V->setCurrentIndex(gs.axisY_V);
  ui->comboBox_TimeY->setCurrentIndex(gs.axisY_T);

  ui->checkBox_Grid->setChecked(gs.grid);
  ui->spinBox_precision->setValue(gs.precision);
  ui->spinBox_KeyStep->setValue(gs.keyStep);

  //HelpClient * help = new HelpClient("7.2", this);
  //this->installEventFilter(help);

  MouseClickEventFilter *mouseFilter = new MouseClickEventFilter(this);
  ui->lineEdit_curveColor->installEventFilter(mouseFilter);
  connect(mouseFilter, SIGNAL(mouseClicked()), SLOT(onColorChange()));

  for(int i = 0; i < m_curves.size(); i++)
  {    
    QwtPlotCurve *curve = m_curves.at(i);
    QString title = curve->title().text();
    ui->listWidget_curves->addItem(title.isEmpty() ? tr("линия %1").arg(i + 1) : title);

    CurveStyle style;
    style.pen = curve->pen();
    m_curveSettingsMap.insert(curve, style);
  }
  if (ui->listWidget_curves->count() != 0)
  {
    ui->listWidget_curves->setCurrentRow(0);
    on_listWidget_curves_itemClicked(ui->listWidget_curves->item(0));
  }
}

ChartSettingsDlg::~ChartSettingsDlg()
{
  delete ui;
}

ChartSettings ChartSettingsDlg::getChartSettings() const
{
  return m_settings;
}

void ChartSettingsDlg::done(int r)
{
  if (r == QDialog::Accepted)
  {
    m_settings.axisY_R = (DistanceDimensions)ui->comboBox_R->currentIndex();
    m_settings.axisY_V = (DistanceDimensions)ui->comboBox_V->currentIndex();
    m_settings.axisY_T = (TimeDimensions)ui->comboBox_TimeY->currentIndex();

    m_settings.grid = ui->checkBox_Grid->isChecked();
    m_settings.keyStep = ui->spinBox_KeyStep->value();
    m_settings.precision = ui->spinBox_precision->value();
  }
  else
  {
    QSet<QwtPlot *> plots;
    foreach(QwtPlotCurve *curve, m_curves)
    {
      curve->setPen(m_curveSettingsMap.value(curve).pen);
      QwtPlot *plot = curve->plot();
      if (! plots.contains(plot))
      {
        plots << plot;
        plot->replot();
      }
    }
  }

  QDialog::done(r);
}

void ChartSettingsDlg::UpdateLineEditColor(QLineEdit *edit, QColor newColor)
{
  QString cl("QLineEdit{background:%1;}");
  edit->setStyleSheet(cl.arg(newColor.name()));
  edit->update();
}

void ChartSettingsDlg::onColorChange()
{
  int row = ui->listWidget_curves->currentRow();
  QwtPlotCurve *s = m_curves.at(row);
  QPen newPen = s->pen();
  QColor color = QColorDialog::getColor(newPen.color(), this);
  if (color.isValid())
  {
    UpdateLineEditColor(ui->lineEdit_curveColor, color);

    newPen.setColor(color);
    s->setPen(newPen);
    s->plot()->replot();
  }
}

void ChartSettingsDlg::on_pushButton_FontSettings_clicked()
{
  bool ok;
  QFont tmp = QFontDialog::getFont(&ok, m_settings.m_markerLabelFont, this);
  if (ok)
    m_settings.m_markerLabelFont = tmp;
}


void ChartSettingsDlg::on_listWidget_curves_itemClicked(QListWidgetItem *item)
{
  int row = ui->listWidget_curves->row(item);
  QwtPlotCurve *s = m_curves.at(row);
  QPen curvePen = s->pen();
  UpdateLineEditColor(ui->lineEdit_curveColor, curvePen.color());
  ui->spinBox_curveWidth->setValue(curvePen.width());
}

void ChartSettingsDlg::on_spinBox_curveWidth_valueChanged(int arg1)
{
  QwtPlotCurve *s = m_curves.at(ui->listWidget_curves->currentRow());
  QPen newPen = s->pen();
  newPen.setWidth(arg1);
  s->setPen(newPen);
  s->plot()->replot();
}
