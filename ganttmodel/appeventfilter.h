#ifndef APPEVENTFILTER_H
#define APPEVENTFILTER_H

#include "ganttproject_global.h"

#include <QObject>

class GANTTMODELSHARED_EXPORT AppEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit AppEventFilter(QObject *parent = 0);
    bool eventFilter(QObject* object,QEvent* event);
signals:

public slots:
};

#endif // APPEVENTFILTER_H
