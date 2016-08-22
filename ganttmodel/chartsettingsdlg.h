#ifndef CHARTSETTINGSDLG_H
#define CHARTSETTINGSDLG_H

#include <QDialog>
#include <QEvent>
#include <QLineEdit>
#include <QListWidgetItem>

#include <qwt/qwt_plot_curve.h>

#include "chartsettings.h"
#include "ganttproject_global.h"

namespace Ui {
class GraphSettingsDlg;
}

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



class GANTTMODELSHARED_EXPORT ChartSettingsDlg : public QDialog
{
  Q_OBJECT
  
public:
  explicit ChartSettingsDlg(const ChartSettings &gs, const QList<QwtPlotCurve*> &curves, QWidget *parent = 0);
  ~ChartSettingsDlg();
  
  ChartSettings getChartSettings() const;

protected:
  /** Обработчик события на закрытие формы */
  void done(int r);

private:
  Ui::GraphSettingsDlg *ui;
  ChartSettings m_settings;
  QList<QwtPlotCurve*> m_curves;
  QMap<QwtPlotCurve*, CurveStyle> m_curveSettingsMap;

  void UpdateLineEditColor(QLineEdit *edit, QColor newColor);

private slots:
  void onColorChange();
  void on_pushButton_FontSettings_clicked();
  void on_listWidget_curves_itemClicked(QListWidgetItem *item);
  void on_spinBox_curveWidth_valueChanged(int arg1);
};

#endif // CHARTSETTINGSDLG_H
