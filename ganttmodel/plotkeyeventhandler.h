#ifndef PLOTKEYEVENTHANDER_H
#define PLOTKEYEVENTHANDER_H

#include <QObject>

class PlotKeyEventHandler : public QObject
{
  Q_OBJECT
public:
  explicit PlotKeyEventHandler(QObject *parent = 0);

protected:
  bool eventFilter(QObject *obj, QEvent *event);

signals:
  void nextPointSelected(bool prev);
};

#endif // PLOTKEYEVENTHANDER_H
