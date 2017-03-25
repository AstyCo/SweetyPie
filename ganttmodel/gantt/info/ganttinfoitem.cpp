/*!
 * \file
 * \~russian
 * \brief Файл содержит реализацию GanttInfoItem class.
 * \~englist
 * \brief File contains realization of GanttInfoItem class.
 */
#include "ganttinfoitem.h"

#include "gantt/private_extensions/gantt-lib_global_values.h"

GanttInfoItem::GanttInfoItem(QObject *parent)
    : QObject(parent)
{
    init();
}

GanttInfoItem::GanttInfoItem(const QString &title
                             , const UtcDateTime &start
                             , const TimeSpan &ts
//                             , const QModelIndex &index
                             , const QColor &color
                             , GanttInfoItem *parentNode
                             , QObject *parent)
    : QObject(parent)
{
    init();

    setTitle(title);
    setStart(start);
    setTimeSpan(ts);
//    setIndex(index);
    setParent(parentNode);
    setColor(color);
}

GanttInfoItem::~GanttInfoItem()
{
}

QString GanttInfoItem::title() const
{
    return _title;
}

MyUtcDateTimeInterval GanttInfoItem::interval() const
{
    return _interval;
}

void GanttInfoItem::setTitle(const QString &title)
{
    if(title == _title)
        return;
    _title = title;
    emit titleChanged();
}

void GanttInfoItem::setExpanded(bool newExpanded)
{
    if(!_expanded && newExpanded){
        _expanded = newExpanded;
        emit expanded();
    }
    else if(_expanded && !newExpanded){
        _expanded = newExpanded;
        collapseChilds();

        emit collapsed();
    }
    else
        _expanded = newExpanded;
}

void GanttInfoItem::collapse()
{
    setExpanded(false);
}

void GanttInfoItem::expand()
{
    setExpanded(true);
}

void GanttInfoItem::changeExpanding()
{
    setExpanded(!isExpanded());
}

int GanttInfoItem::row() const
{
    if(_parent)
        return _parent->indexOf(this);

    return 0;
}

qreal GanttInfoItem::height() const
{
    qreal res = DEFAULT_ITEM_WIDTH;
    if (_expanded && !_items.isEmpty()) {
        qreal childsHeight = 0;
        foreach (GanttInfoItem* item, _items)
        {
            childsHeight+=item->height();
        }
        return childsHeight + res;
    }

    return res;
}

int GanttInfoItem::indexOf(const GanttInfoItem* p_item) const
{
    GanttInfoItem *p = const_cast<GanttInfoItem*>(p_item);
    return _items.indexOf(p);
}

GanttInfoItem *GanttInfoItem::at(int index) const
{
    if( index < 0 || index >= _items.size() )
        return NULL;
    return _items[index];
}

GanttInfoItem *GanttInfoItem::operator[](int index) const
{
    return at(index);
}

QList<GanttInfoItem *> GanttInfoItem::items() const
{
    return _items;
}

int GanttInfoItem::size() const
{
    return _items.size();
}

bool GanttInfoItem::isEmpty() const
{
    return _items.isEmpty();
}

void GanttInfoItem::clear()
{
    _items.clear();
}

void GanttInfoItem::insert(int at, GanttInfoItem *item)
{
    if(!item)
        return;

    item->setParent(this);
    _items.insert(at, item);
    emit itemsChanged();
}


void GanttInfoItem::append(GanttInfoItem *item)
{
    if(!item)
        return;

    item->setParent(this);
    _items.append(item);
    emit itemsChanged();
}

void GanttInfoItem::append(const QList<GanttInfoItem *> &items)
{
    if(items.isEmpty())
        return;

    foreach(GanttInfoItem* item, items)
        item->setParent(this);
    _items.append(items);
    emit itemsChanged();
}

bool GanttInfoItem::removeOne(GanttInfoItem *item)
{
    if(_items.removeOne(item))
    {
        emit itemsChanged();
        return true;
    }
    return false;
}

GanttInfoItem *GanttInfoItem::closestNode()
{
    if(hasChilds())
        return this;

    return _parent;
}



void GanttInfoItem::updatePos()
{
//    qDebug() << title() << "updatePos";
    setPos(calcPos());

    if (!_items.isEmpty()) {
        foreach (GanttInfoItem* item, _items)
            item->updatePos();
    }
}



void GanttInfoItem::init()
{
    _expanded = false;
    _pos = 0;
    _linkCnt = 0;
    _deleted = false;
    _parent = NULL;
    _color = Qt::green;
//    _index = QModelIndex();

//    connect(this, SIGNAL(indexChanged()),this,SIGNAL(changed()));
    connect(this, SIGNAL(titleChanged()),this,SIGNAL(changed()));
    connect(this, SIGNAL(parentChanged()),this,SIGNAL(changed()));
    connect(this, SIGNAL(startChanged()),this,SIGNAL(changed()));
    connect(this, SIGNAL(timeSpanChanged()),this,SIGNAL(changed()));
    connect(this, SIGNAL(colorChanged()),this,SIGNAL(changed()));
    connect(this, SIGNAL(posChanged()),this, SIGNAL(changed()));

    connect(this,SIGNAL(expanded()),this,SLOT(moveLowerItems()));
    connect(this,SIGNAL(collapsed()),this,SLOT(moveLowerItems()));
}

MyUtcDateTimeInterval GanttInfoItem::getInterval() const
{
    return _interval;
}

void GanttInfoItem::onChildDeleted()
{
    GanttInfoItem *item = qobject_cast<GanttInfoItem*>(sender());

    _items.removeOne(item);
}

void GanttInfoItem::collapseChilds()
{
    foreach (GanttInfoItem *item, _items)
        item->setExpanded(false);
}

void GanttInfoItem::moveLowerItems()
{
    moveLowerItemsBy(-1);
}

void GanttInfoItem::moveLowerItemsBy(int id)
{
    if(id < 0)
        id = -1;

    for (int i = id + 1; i < size(); ++i)
        at(i)->updatePos();

    if(_parent)
        _parent->moveLowerItemsBy(this->row());
}

QColor GanttInfoItem::color() const
{
    return _color;
}

bool GanttInfoItem::isExpanded() const{
    return _expanded;
}

bool GanttInfoItem::hasChilds() const
{
    return _items.size() > 0;
}

bool GanttInfoItem::hasStart() const
{
    return _interval.min().isValid();
}

bool GanttInfoItem::hasDuration() const
{
    return _interval.isValid();
}

QList<GanttInfoItem *> GanttInfoItem::siblings() const
{
    if(_parent)
        return _parent->items();

    return QList<GanttInfoItem *>();
}

void GanttInfoItem::setColor(const QColor &color)
{
    if(color == _color)
        return;
    _color = color;
    emit colorChanged();
}

UtcDateTime GanttInfoItem::finish() const
{
    return _interval.end();
}

UtcDateTime GanttInfoItem::start() const
{
    return _interval.min();
}

TimeSpan GanttInfoItem::timeSpan() const
{
    return _interval.timeSpan();
}

void GanttInfoItem::setStart(const UtcDateTime &newValue)
{
    if(newValue == start())
        return;
    _interval.setMin(newValue);
    emit startChanged();
}

void GanttInfoItem::setTimeSpan(const TimeSpan &ts)
{
    if(timeSpan() == ts)
        return;
    _interval.setTimeSpan(ts);
    emit timeSpanChanged();
}


//QModelIndex GanttInfoItem::index() const
//{
//    return _index;
//}

void GanttInfoItem::increaseLinkCnt()
{
    ++_linkCnt;
}

void GanttInfoItem::reduceLinkCnt()
{
    --_linkCnt;
//    tryDelete();
}

QPair<UtcDateTime,UtcDateTime> GanttInfoItem::myMax(
        const QPair<UtcDateTime,UtcDateTime>&f,
        const QPair<UtcDateTime,UtcDateTime>&s )
{
    UtcDateTime resFirst,resSecond;

    if(f.first.isValid() && s.first.isValid())
        resFirst = qMin(f.first,s.first);
    else if(f.first.isValid())
        resFirst = f.first;
    else
        resFirst = s.first;

    if(f.second.isValid() && s.second.isValid())
        resSecond = qMax(f.second,s.second);
    else if(f.second.isValid())
        resSecond = f.second;
    else
        resSecond = s.second;

    return QPair<UtcDateTime,UtcDateTime>(resFirst,resSecond);
}

void GanttInfoItem::disconnectParent()
{
    if(_parent){
        disconnect(this,SIGNAL(aboutToBeDeleted()),_parent,SLOT(onChildDeleted()));
    }
}

void GanttInfoItem::connectParent()
{
    connect(this,SIGNAL(aboutToBeDeleted()),_parent,SLOT(onChildDeleted()));
}

QPair<UtcDateTime,UtcDateTime> GanttInfoItem::getLimits(const GanttInfoItem *root)
{
    QPair<UtcDateTime,UtcDateTime> res;
    if(!root)
        return res;
    res = qMakePair(root->start(),root->finish());

    for(int i = 0; i<root->size(); ++i)
        res = myMax(res, getLimits(root->at(i)));

    return res;
}

QPair<UtcDateTime, UtcDateTime> GanttInfoItem::getLimits() const
{
    return GanttInfoItem::getLimits(this);
}

bool GanttInfoItem::operator==(const GanttInfoItem &another)
{
    return _title == another._title;
}

//void GanttInfoItem::setIndex(const QModelIndex &index)
//{
//    if(index == _index)
//        return;

//    emit indexAboutToBeChanged();
//    _index = index;
//    emit indexChanged();
//}

void GanttInfoItem::setParent(GanttInfoItem *parent)
{
    if(parent == _parent)
        return;

    disconnectParent();
    _parent = parent;
    connectParent();
    setPos(calcPos());
    emit parentChanged();
}

void GanttInfoItem::deleteInfoItem()
{
    emit aboutToBeDeleted();
    tryDelete();
}

void GanttInfoItem::tryDelete()
{
    if(_linkCnt>0)
        return;

    _mutex.lock();
    if(!_deleted)
    {
        this->deleteLater();
        _deleted = true;
    }
    _mutex.unlock();
}

qreal GanttInfoItem::calcPos() const
{
    if(!_parent)
        return 0.0; // m_root
    qreal base = _parent->calcPos();
    if(!_parent->isExpanded())
        return base;

    qreal offset = (_parent->parent() ? DEFAULT_ITEM_HEIGHT : 0); // if root needn't extra offset
    const QList<GanttInfoItem*>& items = _parent->_items;
    // looking for this item, stops on found
    for(int i = 0; i < items.size(); ++i)
    {
        if( items[i] == this)
            break;

        offset += items[i]->height();
    }

    return base + offset;
}

int GanttInfoItem::pos() const
{
    return _pos;
}

qreal GanttInfoItem::bottom() const
{
    return height() + pos();
}

void GanttInfoItem::setPos(int pos)
{
    _pos = pos;
    emit posChanged();
}


GanttInfoItem *GanttInfoItem::parent() const
{
    return _parent;
}

