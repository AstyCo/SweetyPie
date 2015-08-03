#ifndef PLOTKEYEVENTHANDER_H
#define PLOTKEYEVENTHANDER_H

#include <QObject>

class PlotKeyEventHander : public QObject
{
  Q_OBJECT
public:
  explicit PlotKeyEventHander(QObject *parent = 0);

protected:
  bool eventFilter(QObject *obj, QEvent *event);

signals:
  void nextPointSelected(bool prev);
};

#endif // PLOTKEYEVENTHANDER_H
