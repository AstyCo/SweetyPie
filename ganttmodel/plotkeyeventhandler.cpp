#include "plotkeyeventhandler.h"
#include "QKeyEvent"

PlotKeyEventHandler::PlotKeyEventHandler(QObject *parent) :
  QObject(parent)
{
}

bool PlotKeyEventHandler::eventFilter(QObject *obj, QEvent *event)
{
  Q_UNUSED(obj)

  if (event->type() == QEvent::KeyPress)
  {
      QKeyEvent *ke = static_cast<QKeyEvent *>(event);
      if (ke->key() == Qt::Key_Left)
      {
        emit nextPointSelected(true);
        return true;
      }
      else if (ke->key() == Qt::Key_Right)
      {
        emit nextPointSelected(false);
        return true;
      }
  }
  return false;
}
