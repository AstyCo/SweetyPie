#include "chartsettingsdlg.h"
#include "ui_chartsettingsdlg.h"
#include <QPushButton>
//#include "helpclient.h"
#include <QColorDialog>
#include <QFontDialog>


ChartSettingsDlg::ChartSettingsDlg(ChartSettings * gs, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::GraphSettingsDlg)
{
  ui->setupUi(this);

  ui->buttonBox_OkCancel->button(QDialogButtonBox::Ok)->setText(tr("Применить"));
  ui->buttonBox_OkCancel->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/konus/icons/apply.png"));
  ui->buttonBox_OkCancel->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
  ui->buttonBox_OkCancel->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/konus/icons/denial.png"));

  setWindowFlags((this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowContextHelpButtonHint);

  m_gs = gs;
  m_cur_gs = *m_gs;
  ui->comboBox_R->setCurrentIndex(gs->axisY_R);
  ui->comboBox_V->setCurrentIndex(gs->axisY_V);
  ui->comboBox_TimeY->setCurrentIndex(gs->axisY_T);
  ui->comboBox_Time->setCurrentIndex(gs->axisX);
  ui->checkBox_Grid->setChecked(gs->grid);
  ui->spinBox_precision->setValue(gs->precision);
  //HelpClient * help = new HelpClient("7.2", this);
  //this->installEventFilter(help);

  MouseClickEventFilter *mouseFilter = new MouseClickEventFilter(this);
  ui->lineEdit_color1->installEventFilter(mouseFilter);
  connect(mouseFilter, SIGNAL(mouseClicked()), SLOT(onColorChange1()));

  mouseFilter = new MouseClickEventFilter(this);
  ui->lineEdit_color2->installEventFilter(mouseFilter);
  connect(mouseFilter, SIGNAL(mouseClicked()), SLOT(onColorChange2()));

  mouseFilter = new MouseClickEventFilter(this);
  ui->lineEdit_color3->installEventFilter(mouseFilter);
  connect(mouseFilter, SIGNAL(mouseClicked()), SLOT(onColorChange3()));

  UpdateLineEditColor(ui->lineEdit_color1, gs->color_1);
  UpdateLineEditColor(ui->lineEdit_color2, gs->color_2);
  UpdateLineEditColor(ui->lineEdit_color3, gs->color_3);
  ui->spinBox_lineWidth->setValue(gs->width);
  ui->spinBox_KeyStep->setValue(gs->keyStep);
}

ChartSettingsDlg::~ChartSettingsDlg()
{
  delete ui;
}

void ChartSettingsDlg::done(int r)
{
  m_gs->axisY_R = (DistanceDimensions)ui->comboBox_R->currentIndex();
  m_gs->axisY_V = (DistanceDimensions)ui->comboBox_V->currentIndex();
  m_gs->axisY_T = (TimeDimensions)ui->comboBox_TimeY->currentIndex();
  m_gs->axisX = (TimeDimensions)ui->comboBox_Time->currentIndex();
  m_gs->m_markerLabelFont = m_cur_gs.m_markerLabelFont;
  m_gs->color_1 = m_cur_gs.color_1;
  m_gs->color_2 = m_cur_gs.color_2;
  m_gs->color_3 = m_cur_gs.color_3;
  m_gs->grid = ui->checkBox_Grid->isChecked();

  m_gs->precision = ui->spinBox_precision->value();
  m_gs->width = ui->spinBox_lineWidth->value();
  m_gs->keyStep = ui->spinBox_KeyStep->value();

  QDialog::done(r);
}

void ChartSettingsDlg::UpdateLineEditColor(QLineEdit *edit, QColor newColor)
{
  QString cl("QLineEdit{background:%1;}");
  edit->setStyleSheet(cl.arg(newColor.name()));
  edit->update();
}

void ChartSettingsDlg::onColorChange1()
{
  QColor color = QColorDialog::getColor(m_cur_gs.color_1, this);
  if (color.isValid())
  {
    UpdateLineEditColor(ui->lineEdit_color1, color);
    m_cur_gs.color_1 = color;
  }
}

void ChartSettingsDlg::onColorChange2()
{
  QColor color = QColorDialog::getColor(m_cur_gs.color_2, this);
  if (color.isValid())
  {
    UpdateLineEditColor(ui->lineEdit_color2, color);
    m_cur_gs.color_2 = color;
  }
}

void ChartSettingsDlg::onColorChange3()
{
  QColor color = QColorDialog::getColor(m_cur_gs.color_3, this);
  if (color.isValid())
  {
    UpdateLineEditColor(ui->lineEdit_color3, color);
    m_cur_gs.color_3 = color;
  }
}

void ChartSettingsDlg::on_pushButton_FontSettings_clicked()
{
  bool ok;
  QFont tmp = QFontDialog::getFont(&ok, m_cur_gs.m_markerLabelFont, this);
  if (ok)
    m_cur_gs.m_markerLabelFont = tmp;
}
