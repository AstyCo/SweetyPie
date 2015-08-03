#ifndef CHARTSETTINGSDLG_H
#define CHARTSETTINGSDLG_H

#include <QDialog>
#include <QEvent>
#include <QLineEdit>


enum DistanceDimensions
{
  dMillimeters,
  dMeters,
  dKilometers,
  d1000Kilometers
};

enum TimeDimensions
{
  tSeconds,
  tMinutes,
  tHours,
  tDays
};

namespace Ui {
class GraphSettingsDlg;
}

struct ChartSettings
{
  ChartSettings()
  {
    axisX = tDays;
    axisY_R = dMeters;
    axisY_V = dMillimeters;
    axisY_T = tSeconds;

    color_1 = Qt::red;
    color_2 = Qt::green;
    color_3 = Qt::blue;

    width = 1;
    grid = false;
    keyStep = 1;
    precision = 6;
    detailsPanelVisible = false;
  }

  TimeDimensions axisX;
  DistanceDimensions axisY_R;
  DistanceDimensions axisY_V;
  TimeDimensions axisY_T;
  int width;
  QColor color_1;
  QColor color_2;
  QColor color_3;
  QFont m_markerLabelFont;
  int precision;
  bool grid;
  int keyStep;
  bool detailsPanelVisible;
};

class MouseClickEventFilter : public QObject
{
Q_OBJECT

public:
  MouseClickEventFilter(QObject *parent) :
    QObject(parent)
  {
  }

  bool eventFilter(QObject *obj, QEvent *event)
  {
    Q_UNUSED(obj)

    if (event->type() == QEvent::MouseButtonPress)
    {
//        QMouseEvent *ke = static_cast<QKeyEvent *>(event);
//        if (ke->key() == Qt::Key_F1 || ke->key() == Qt::Key_Help)
//        {

//          return true;
//        }
      emit mouseClicked();
      return true;
    }
    return false;
  }

signals:
  void mouseClicked();
};



class ChartSettingsDlg : public QDialog
{
  Q_OBJECT
  
public:
  explicit ChartSettingsDlg(ChartSettings *gs, QWidget *parent = 0);
  ~ChartSettingsDlg();
  
protected:
  /** Обработчик события на закрытие формы */
  void done(int r);

private:
  Ui::GraphSettingsDlg *ui;
  ChartSettings *m_gs;
  ChartSettings m_cur_gs;
  void UpdateLineEditColor(QLineEdit *edit, QColor newColor);

private slots:
  void onColorChange1();
  void onColorChange2();
  void onColorChange3();
  void on_pushButton_FontSettings_clicked();
};

#endif // CHARTSETTINGSDLG_H
