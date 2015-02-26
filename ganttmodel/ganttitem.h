#ifndef GANTTITEM_H
#define GANTTITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include "ganttitemstage.h"

#include <QDateTime>
#include <QPair>
#include <QString>
#include "ganttmodel_global.h"

class GANTTMODELSHARED_EXPORT  GanttItem
{
public:
    //========etm==============
//    GanttItem(const QVector<QVariant> &data, GanttItem *parent = 0);
//    ~GanttItem();

//    GanttItem *child(int number);
//    int childCount() const;
//    int columnCount() const;
//    QVariant data(int column) const;
//    bool insertChildren(int position, int count, int columns);
//    bool insertColumns(int position, int columns);
//    GanttItem *parent();
//    bool removeChildren(int position, int count);
//    bool removeColumns(int position, int columns);
//    int childNumber() const;
//    bool setData(int column, const QVariant &value);
    //==========================

    //=======timelog2============
    //explicit GanttItem(const QString &name=QString(), QDateTime begin, QDateTime end, bool done=false, GanttItem *parent=0);
    explicit GanttItem(const QString &name=QString(),
                       QDateTime begin = QDateTime::currentDateTime(),
                       QDateTime end= QDateTime::currentDateTime(),
                       bool done=false, GanttItem *parent=0);
    ~GanttItem() { qDeleteAll(m_children); }

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }
//    QString title() const { return m_title; }
//    void setTitle(const QString &title) { m_title = title; }
    bool isDone() const { return m_done; }
    void setDone(bool done) { m_done = done; }
    QList<QPair<QDateTime, QDateTime> > dateTimes() const
        { return m_dateTimes; }
    void addDateTime(const QDateTime &start, const QDateTime &end)
        { m_dateTimes << qMakePair(start, end); }
    QString todaysTime() const;
    QString totalTime() const;
    void incrementLastEndTime(int msec);
    GanttItem *parent() const { return m_parent; }
    GanttItem *childAt(int row) const { return m_children.value(row); }
    int rowOfChild(GanttItem *child) const
        { return m_children.indexOf(child); }
    int childCount() const { return m_children.count(); }
    bool hasChildren() const { return !m_children.isEmpty(); }
    QList<GanttItem*> children() const { return m_children; }

    void insertChild(int row, GanttItem *item)
        { item->m_parent = this; m_children.insert(row, item); }
    void addChild(GanttItem *item)
        { item->m_parent = this; m_children << item; }
    void swapChildren(int oldRow, int newRow)
        { m_children.swap(oldRow, newRow); }
    GanttItem* takeChild(int row);
    //==========================
    QString title() const { return m_title; }
    void setTitle(const QString &title) { m_title = title; }
    QDateTime begin();// { return m_commonBegin; }
    QDateTime end();// { return m_commonEnd; }
    QString duration();// { return m_commonDuration; }

    //QList<GanttItemStage*> stageList() { return m_stageList; }


    //GanttItem(QDateTime begin, QDateTime end, GanttItem::ItemType type);

    enum ItemType {Simple,          // элемент не объединяет детей и не зависит от них
                   Union,          // элемент объединяет детей и зависит от них
                   Multiunion};    // элемент состоит из частей, каждая из которых объединяет детей и зависит от них

    //void addStage(QDateTime begin, QDateTime end);


    QDateTime commonBegin() const;
    void setCommonBegin(const QDateTime &commonBegin);

    QDateTime commonEnd() const;
    void setCommonEnd(const QDateTime &commonEnd);

//    int commonDuration() const;
//    void setCommonDuration(const int &commonDuration);

    int commonDuration() const;
    void setCommonDuration(int commonDuration);

    bool isExpanded() const;
    void setIsExpanded(bool isExpanded);

private:
    //=======etm==========
    //    QList<GanttItem*> childItems;
    //    QVector<QVariant> itemData;
//    GanttItem *parentItem;
    //==============

    //======timelog2=====
    int minutesForTask(bool onlyForToday) const;

    QString m_name;
    bool m_done;
    QList<QPair<QDateTime, QDateTime> > m_dateTimes;

    GanttItem *m_parent;
    QList<GanttItem*> m_children;
    //===============

    QString                m_title;
    QDateTime              m_commonBegin;
    QDateTime              m_commonEnd;
    int                    m_commonDuration;


    bool m_isExpanded;



};

#endif // GANTTITEM_H
