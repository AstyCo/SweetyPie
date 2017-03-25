/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление GanttInfoItem class.
 * \~englist
 * \brief File contains definition of GanttInfoItem class.
 */
#ifndef GANTTINFOITEM_H
#define GANTTINFOITEM_H

#include "ganttproject_global.h"
#include "utcdatetime.h"
#include "extensions/myutcdatetimeinterval.h"

#include <QColor>
#include <QString>
#include <QModelIndex>
#include <QMutex>

class UtcDateTime;

/*!
 * \~russian
 * \brief Класс GanttInfoItem class содержит информацию о событии
 * \~english
 * \brief The GanttInfoItem class class contains information of event
 */
class GANTTMODELSHARED_EXPORT GanttInfoItem : public QObject
{
    Q_OBJECT
    void init();
public:
    enum InfoType {
        Node,
        Leaf
    };

    GanttInfoItem(QObject *parent = NULL);
    GanttInfoItem(const QString &title,
                  const UtcDateTime   &start,
                  const TimeSpan      &ts = TimeSpan(),
//                  const QModelIndex   &index = QModelIndex(),
                  const QColor        &color = Qt::green,
                  GanttInfoItem       *parentNode = NULL,
                  QObject             *parent = NULL);
    virtual ~GanttInfoItem();

    GanttInfoItem *parent() const;
    int row() const;

    qreal height() const;
    virtual qreal calcPos() const;
    int pos() const;
    qreal bottom() const;
    void setPos(int pos);

//    QModelIndex index() const;

    QString     title() const;
    MyUtcDateTimeInterval interval() const;
    UtcDateTime start() const;
    TimeSpan timeSpan() const;
    QColor      color() const;
    UtcDateTime finish() const;

    bool isExpanded() const;
    bool hasChilds() const;
    bool hasStart() const;
    bool hasDuration() const;

    QList<GanttInfoItem *> siblings() const;

    static QPair<UtcDateTime,UtcDateTime> getLimits(const GanttInfoItem *root);
    QPair<UtcDateTime,UtcDateTime> getLimits() const;
    virtual bool operator==(const GanttInfoItem& another);

    int indexOf(const GanttInfoItem* p_item) const;

    GanttInfoItem *at(int index) const;
    GanttInfoItem *operator[](int index) const;
    QList<GanttInfoItem *> items() const;

    int size() const;
    bool isEmpty() const;
    void clear();
    void insert(int at, GanttInfoItem* item);
    void append(GanttInfoItem* item);
    void append(const QList<GanttInfoItem*>& items);
    bool removeOne(GanttInfoItem* item);

    GanttInfoItem *closestNode();

signals:
    void expanded();
    void collapsed();

    void parentChanged();
//    void indexAboutToBeChanged();
//    void indexChanged();
    void titleChanged();
    void aboutToBeDeleted();
    void startChanged();
    void timeSpanChanged();
    void colorChanged();
    void posChanged();
    void itemsChanged();

    void changed();

public slots:
//    void setIndex(const QModelIndex &index);
    void setParent(GanttInfoItem *parent);
    void setStart(const UtcDateTime &start);
    void setTimeSpan(const TimeSpan &ts);
    void setColor(const QColor &color);
    void setTitle(const QString &title);

    void setExpanded(bool expanded);
    void collapse();
    void expand();
    void changeExpanding();

    void moveLowerItems();
    void moveLowerItemsBy(int id);
    void updatePos();
    void increaseLinkCnt();
    void reduceLinkCnt();
    void deleteInfoItem();
    void tryDelete();

public:
    static QPair<UtcDateTime, UtcDateTime> myMax(
            const QPair<UtcDateTime, UtcDateTime> &first,
            const QPair<UtcDateTime, UtcDateTime> &second );

    MyUtcDateTimeInterval getInterval() const;

private slots:
    void onChildDeleted();
    void collapseChilds();


private:
    virtual void disconnectParent();
    virtual void connectParent();
private:
    GanttInfoItem *_parent;         ///< represents parent
//    QModelIndex _index;             ///< info connects to model index
//    InfoType _type;                 ///< proper way to check for whatis
    QList<GanttInfoItem*> _items;   ///< list of child elements

    //@{
    /** The Data of GanttInfoItem */

    QString _title;
    MyUtcDateTimeInterval _interval;
    QColor _color;
    //@}

    bool _expanded; ///< current expansion state
    int _pos;       ///< caches vertical pos

    //@{
    /** For thread safe */

    unsigned int _linkCnt;
    bool _deleted;
    QMutex _mutex;
    //@}
};

#endif // GANTTINFOITEM_H
