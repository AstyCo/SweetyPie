#include "gantttreemodel.h"

#include "gantt/private_extensions/gantt-lib_global_values.h"
#include "gantt/info/treewalker.h"

#include <QSize>

Q_DECLARE_METATYPE(UtcDateTime)

GanttTreeModel::GanttTreeModel(GanttInfoItem *root,QObject * parent)
    : QAbstractItemModel(parent)
{

    m_root = root;

    if(!m_root)
        m_root = new GanttInfoItem;

    m_root->setExpanded(true);
    setIndex(m_root);
}

GanttTreeModel::~GanttTreeModel()
{
    clear();
}

QVariant GanttTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if (role == Qt::SizeHintRole)
    {
        QSize defSize ;
        defSize.setHeight(DEFAULT_ITEM_WIDTH);
        return defSize ;
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    GanttInfoItem *item = itemForIndex(index);
    if (item == m_root) {
        return QVariant();
    }
    else if (item->hasDuration()) {
        switch (index.column())
        {
        case titleField:
            return item->title();
        case startField:
            return item->start().dateTime();
        case finishField:
            return item->finish().dateTime();
        case durationField:
            return item->timeSpan().toString();
        default:
            Q_ASSERT(false);
        }
    }
    else {
        switch (index.column())
        {
        case 0:
            return item->title();
        case 1:
            if(item->hasStart())
                return item->start().dateTime();
            return QVariant();
        case finishField:
        case durationField:
            return QVariant();
        default:
            Q_ASSERT(false);
        }
    }

    return QVariant();
}

Qt::ItemFlags GanttTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled /*| Qt::ItemIsSelectable*/;
}

QVariant GanttTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);

    if (role == Qt::SizeHintRole)
    {
        QSize defSize ;
        defSize.setHeight(2*DEFAULT_ITEM_WIDTH);
        return defSize ;
    }


    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case titleField:
            return trUtf8("Название");
        case startField:
            return trUtf8("Время расчета/начала");
        case finishField:
            return trUtf8("Время окончания");
        case durationField:
            return trUtf8("Длительность");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QModelIndex GanttTreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    GanttInfoItem *parentNode;

    if (!parent.isValid())
    {
        parentNode = m_root;
    }
    else
        parentNode = itemForIndex(parent);

    GanttInfoItem *childItem = parentNode->at(row);

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();

}

GanttInfoItem *GanttTreeModel::itemForIndex(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return m_root;
    }

    return static_cast<GanttInfoItem*>(index.internalPointer());
}

void GanttTreeModel::printTree() const
{
    qDebug() << "printTree";
    printTreeR(m_root, 0);
}

void GanttTreeModel::setIndex(GanttInfoItem *item)
{
    if(!item->index().isValid())
        item->setIndex((item==m_root)?(QModelIndex()):(createIndex(item->row(),0,item)));
}

GanttInfoItem *GanttTreeModel::itemForNameHelper(const QString &title,GanttInfoItem* node) const
{
    if(!node)
        return NULL;

    for(int i = 0; i < node->size(); ++i)
    {
        GanttInfoItem *p_item = node->at(i);
        GanttInfoItem *tmp;

        if(p_item->title() == title)
            return p_item;

        if(p_item->hasChilds() && (tmp = itemForNameHelper(title,p_item)))
            return tmp;
    }
    return NULL;
}
GanttInfoItem *GanttTreeModel::root() const
{
    return m_root;
}


void deleteFunc(GanttInfoItem* item)
{
    if(item->parent())
    {
        item->deleteInfoItem();
    }
}

void GanttTreeModel::clear()
{
    beginRemoveRows(QModelIndex(),0,m_root->size());

    callRecursively(m_root, &deleteFunc);
//    qDebug() << "MODEL m_root size after clear " << m_root->size();

    endRemoveRows();

}

void GanttTreeModel::reset()
{
    clear();
    QAbstractItemModel::reset();
}




QModelIndex GanttTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    GanttInfoItem *childItem = itemForIndex(index);
    GanttInfoItem *parentNode = childItem->parent();

    if(!parentNode)
    {
        qWarning("GanttTreeModel::parent     !parentNode");
        return QModelIndex();
    }

    if (parentNode == m_root)
    {
        return QModelIndex();
    }

    return createIndex(parentNode->row(), 0, parentNode);
}

int GanttTreeModel::rowCount(const QModelIndex &parent) const
{
    GanttInfoItem *parentNode;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentNode = m_root;
    else if(!(parentNode = itemForIndex(parent)))
        return 0;

    return parentNode->size();
}

int GanttTreeModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 4;
}

bool GanttTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.column() == durationField )
        return false;


    GanttInfoItem *info = itemForIndex(index);

    if (info)
    {
        if (role == Qt::EditRole)
        {
            if (index.column() == titleField)
                info->setTitle(value.toString());
            else if (index.column() == startField)
            {
                UtcDateTime time = UtcDateTime(value.toDateTime());
                if(time >= info->finish())
                    return false;
                UtcDateTime finish = info->finish();
                info->setTimeSpan(finish - time);

                info->setStart(time);
            }
            else if (index.column() == finishField)
            {
                UtcDateTime time = UtcDateTime(value.toDateTime());
                if(info->start() >= time)
                    return false;

                info->setTimeSpan(time - info->start());
            }
        }
        else
            return false;

        emit dataChanged(index, index);
        return true;
    }
    return false;
}

void GanttTreeModel::addItems(const QList<GanttInfoItem *> &items, GanttInfoItem *destNode)
{
    if(destNode == NULL)
        destNode = m_root;
    int cnt = 0;
    QMap<QString, GanttInfoItem*> tmpItemsForNames;
    foreach(GanttInfoItem *item, items){
        GanttInfoItem *tmp;
        if((tmp = itemForName(item->title(), destNode))
           && item->parent() != destNode)
            tmpItemsForNames.insert(item->title(), tmp);
        else
            ++cnt;
    }
    if(cnt > 0){
        int first;
        if (destNode->isEmpty() || destNode == items.at(0)->parent()) /// TODO dirty hack
            first = 0;
        else
            first = destNode->size();
        beginInsertRows(destNode->index(), first, first + cnt - 1);
        for(int i = 0; i < items.size(); ++i){
            if(tmpItemsForNames.contains(items.at(i)->title()))
                continue;

            insertItem(items.at(i), destNode);
        }
        endInsertRows();

        for(int i = 0; i < items.size(); ++i){
            if(tmpItemsForNames.contains(items.at(i)->title()))
                continue;

            addItems(items.at(i)->items(), items.at(i));
        }

    }
    if(cnt!=items.size())
        for(int i = 0; i < items.size(); ++i){
            if(tmpItemsForNames.contains(items.at(i)->title())){
                GanttInfoItem *insertToNode = tmpItemsForNames.value(items.at(i)->title());
                if(items.at(i)->isEmpty())
                    continue;

                addItems(items.at(i)->items(), insertToNode);
            }
        }

    emit itemsAdded();
}

void GanttTreeModel::addNode(GanttInfoItem *node)
{
    if(node->isEmpty())
        return;
    beginInsertRows(node->index(), 0, node->size() - 1);
    for(int i = 0; i < node->size(); ++i){
        setIndex(node->at(i));
    }
    endInsertRows();

    foreach(GanttInfoItem *item, node->items()){
        addItems(item->items(), node);
    }

}

void GanttTreeModel::insertItem(GanttInfoItem *item, GanttInfoItem *parent)
{
    if(!parent || item == parent)
        return;

    if(item->parent() != parent)
        parent->append(item);

    setIndex(item);
}

void GanttTreeModel::addAfter(GanttInfoItem *item, GanttInfoItem *targetItem)
{
    if (!item)
        return;
    if (targetItem == NULL) {
        QList<GanttInfoItem*> tmp;
        tmp.append(item);
        addItems(tmp);
        return;
    }
    else {
        QModelIndex index = targetItem->index();
//        beginInsertRows(index.parent(), index.row()+1, index.row()+1);
        GanttInfoItem *parent = targetItem->parent();
        if (parent)
            parent->insert(index.row()+1, item);
        else
            Q_ASSERT(false);
//        endInsertRows();
    }
    reset();
}

void GanttTreeModel::addBefore(GanttInfoItem *item, GanttInfoItem *targetItem)
{
    if (!item)
        return;
    if (targetItem == NULL) {
        beginInsertRows(QModelIndex(), 0 , 0);
        m_root->insert(0, item);
    }
    else {
        QModelIndex index = targetItem->index();
//        beginInsertRows(index.parent(), index.row(), index.row());
        GanttInfoItem *parent = targetItem->parent();
        if (parent)
            parent->insert(index.row(), item);
        else
            Q_ASSERT(false);
    }
//    endInsertRows();
    reset();
}

GanttInfoItem *GanttTreeModel::itemForName(const QString &title, GanttInfoItem *parent) const
{
    if(!parent)
        parent = m_root;
    return itemForNameHelper(title,parent);
}


