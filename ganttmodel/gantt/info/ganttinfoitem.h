#ifndef GANTTINFOITEM_H
#define GANTTINFOITEM_H

#include "ganttproject_global.h"
#include "utcdatetime.h"
#include "extensions/myutcdatetimeinterval.h"

#include <QColor>
#include <QString>
#include <QModelIndex>
#include <QMutex>

class GanttInfoNode;
class UtcDateTime;

class GANTTMODELSHARED_EXPORT GanttInfoItem : public QObject
{
    Q_OBJECT
    void init();
public:
    GanttInfoItem(QObject *parent = NULL);
    GanttInfoItem(const QString &title,
                  const UtcDateTime   &start,
                  const TimeSpan      &ts,
                  const QModelIndex   &index = QModelIndex(),
                  const QColor        &color = Qt::green,
                  GanttInfoNode       *parentNode = NULL,
                  QObject             *parent = NULL);
    virtual ~GanttInfoItem();

    GanttInfoNode *parent() const;
    int row() const;

    virtual int columnCount() const = 0;
    virtual qreal height() const = 0;
    virtual qreal calcPos() const;
    int pos() const;
    qreal bottom() const;
    void setPos(int pos);

    QModelIndex index() const;

    QString     title() const;
    UtcDateTime start() const;
    TimeSpan timeSpan() const;
    UtcDateTime finish() const;
    QColor      color() const;

    virtual bool isExpanded() const{
        return false;
    }
    bool hasStart() const;
    bool isDot() const;
    virtual GanttInfoNode *node() = 0;
    QList<GanttInfoItem *> siblings() const;



    static QPair<UtcDateTime,UtcDateTime> getLimits(const GanttInfoItem *root);
    QPair<UtcDateTime,UtcDateTime> getLimits() const;
    virtual bool operator==(const GanttInfoItem& another);

    void increaseLinkCnt();
    void reduceLinkCnt();
    virtual int indexOf(const GanttInfoItem* p_item) const;
signals:
    void expanded();
    void collapsed();
    void parentChanged();
    void indexChanged();
    void titleChanged();
    void aboutToBeDeleted();
    void startChanged();
    void timeSpanChanged();
    void colorChanged();
    void posChanged();

    void changed();

public slots:
    virtual void updatePos();
    void setStart(const UtcDateTime &start);
    void setTimeSpan(const TimeSpan &ts);
    void setColor(const QColor &color);
    void setTitle(const QString &title);
    void setIndex(const QModelIndex &index);
    void setParent(GanttInfoNode *parent);
    void deleteInfoItem();
    void tryDelete();

public:
    static QPair<UtcDateTime, UtcDateTime> myMax(
            const QPair<UtcDateTime, UtcDateTime> &first,
            const QPair<UtcDateTime, UtcDateTime> &second );

    MyUtcDateTimeInterval getInterval() const;

protected:
    virtual void disconnectParent();
    virtual void connectParent();
private:
    GanttInfoNode* _parent;
    QModelIndex _index;

    QString _title;
    MyUtcDateTimeInterval _interval;
    QColor _color;

    int _pos;   // caches vertical pos

    unsigned int _linkCnt;
    bool _deleted;
    QMutex _mutex;
};

#endif // GANTTINFOITEM_H
