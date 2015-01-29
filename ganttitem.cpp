#include "ganttitem.h"

#include <QStringList>
#include <QDebug>

//=========etm===================
/*
GanttItem::GanttItem(const QVector<QVariant> &data, GanttItem *parent)
{
    parentItem = parent;
    itemData = data;
}


GanttItem::~GanttItem()
{
    qDeleteAll(childItems);
}

GanttItem *GanttItem::child(int number)
{
    return childItems.value(number);
}

int GanttItem::childCount() const
{
    return childItems.count();
}

int GanttItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<GanttItem*>(this));

    return 0;
}
*/

//int GanttItem::columnCount() const
//{
//    return itemData.count();
//}

/*
QVariant GanttItem::data(int column) const
{
    return itemData.value(column);
}

bool GanttItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        GanttItem *item = new GanttItem(data, this);
        childItems.insert(position, item);
    }

    return true;
}
*/
//bool GanttItem::insertColumns(int position, int columns)
//{
//    if (position < 0 || position > itemData.size())
//        return false;

//    for (int column = 0; column < columns; ++column)
//        itemData.insert(position, QVariant());

//    foreach (GanttItem *child, childItems)
//        child->insertColumns(position, columns);

//    return true;
//}

/*
GanttItem *GanttItem::parent()
{
    return parentItem;
}

bool GanttItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}
*/

//bool GanttItem::removeColumns(int position, int columns)
//{
//    if (position < 0 || position + columns > itemData.size())
//        return false;

//    for (int column = 0; column < columns; ++column)
//        itemData.remove(position);

//    foreach (GanttItem *child, childItems)
//        child->removeColumns(position, columns);

//    return true;
//}

/*
bool GanttItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}
*/
//=================================

//============timelog2=================
GanttItem::GanttItem(const QString &name, QDateTime begin, QDateTime end, bool done, GanttItem *parent)
        : m_name(name), m_done(done), m_parent(parent)
{
    if (m_parent)
        m_parent->addChild(this);
    m_commonBegin = begin;
    m_commonEnd = end;
    /*
    QDateTime dtBegin = QDateTime::fromString("28.01.2015", "dd.MM.yyyy");
    QDateTime dtEnd = QDateTime::fromString("30.01.2015", "dd.MM.yyyy");
    */
    addStage(begin, end);
}


QString GanttItem::todaysTime() const
{
    int minutes = minutesForTask(true);
    return QString("%1:%2").arg(minutes / 60)
                           .arg(minutes % 60, 2, 10, QChar('0'));
}


QString GanttItem::totalTime() const
{
    int minutes = minutesForTask(false);
    return QString("%1:%2").arg(minutes / 60)
                           .arg(minutes % 60, 2, 10, QChar('0'));
}


int GanttItem::minutesForTask(bool onlyForToday) const
{
    int minutes = 0;
    QListIterator<QPair<QDateTime, QDateTime> > i(m_dateTimes);
    while (i.hasNext()) {
        const QPair<QDateTime, QDateTime> &dateTime = i.next();
        if (onlyForToday &&
            dateTime.first.date() != QDate::currentDate())
            continue;
        minutes += (dateTime.first.secsTo(dateTime.second) / 60);
    }
    foreach (GanttItem *child, m_children)
        minutes += child->minutesForTask(onlyForToday);
    return minutes;
}

void GanttItem::incrementLastEndTime(int msec)
{
    QDateTime &endTime = m_dateTimes.last().second;
    endTime.setTime(endTime.time().addMSecs(msec));
}


GanttItem* GanttItem::takeChild(int row)
{
    GanttItem *item = m_children.takeAt(row);
    Q_ASSERT(item);
    item->m_parent = 0;
    return item;
}
//=====================================
QString GanttItem::begin()
{
    //m_commonBegin = QDateTime::fromString("01.11.2014", "dd.MM.yyyy");
    return m_commonBegin.toString("dd.MM.yyyy hh:mm:ss");
}

QString GanttItem::end()
{
    //m_commonEnd = QDateTime::currentDateTime();
    return m_commonEnd.toString("dd.MM.yyyy hh:mm:ss");
}

QString GanttItem::duration() const
{
    //m_commonDuration = QDateTime::fromTime_t(m_commonBegin.secsTo(m_commonEnd));
    qreal secs = m_commonBegin.secsTo(m_commonEnd)/86400;
    QString string = QString::number(secs, 'f', 1) + " " + QObject::trUtf8("дней");
    //return QString::number(secs);
    qDebug()<<"sex"<<secs;
    return string;
}

void GanttItem::addStage(QDateTime begin, QDateTime end)
{
    m_stage = new GanttItemStage(begin, end);
    m_stageList.append(m_stage);
    if (begin<m_commonBegin)
        m_commonBegin = begin;
    if (end>m_commonEnd)
        m_commonEnd = end;
}

//void GanttItem::createGraphicsItem(QList<GanttItemStage *> stages)
//{
//    m_graphicsItem = new GanttGraphicsItem(stages);
//}
