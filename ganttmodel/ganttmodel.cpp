#include <QtGui>

#include "ganttmodel.h"
#include "ganttitem.h"

#include <QMimeData>

#define ROW_HEIGHT 20


namespace {
const int ColumnCount = 4;
const int MaxCompression = 9;
enum Column {Title, Begin, End, Duration};
const QString MimeType = "application/vnd.qtrac.xml.task.z";
}

//=============editabletreemodel===============
/*
GanttModel::GanttModel(const QStringList &headers, const QString &data, QObject *parent) :
    QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    foreach (QString header, headers)
        rootData << header;

    rootItem = new GanttItem(rootData);
    setupModelData(data.split(QString("\n")), rootItem);
}


GanttModel::~GanttModel()
{
    delete rootItem;
}

QVariant GanttModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    GanttItem *item = getItem(index);

    return item->data(index.column());
}

GanttItem *GanttModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        GanttItem *item = static_cast<GanttItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem;
}


bool GanttModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    GanttItem *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}


bool GanttModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    GanttItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}
*/

/*
bool GanttModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}


bool GanttModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}
*/


/*
bool GanttModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

void GanttModel::setupModelData(const QStringList &lines, GanttItem *parent)
{
    QList<GanttItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].mid(position, 1) != " ")
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QVector<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            GanttItem *parent = parents.last();
            parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());
            for (int column = 0; column < columnData.size(); ++column)
                parent->child(parent->childCount() - 1)->setData(column, columnData[column]);
        }

        number++;
    }
}
*/
//==========================================


//==========timelog2=================
QVariant GanttModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if (GanttItem *item = itemForIndex(index)) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (index.column()) {
                case Title: return item->name();
                case Begin: return item->begin();
                case End: return item->end();
                case Duration: return item->duration();
                default: Q_ASSERT(false);
            }
        }
        if (role == Qt::CheckStateRole && index.column() == Title)
            return static_cast<int>(item->isDone() ? Qt::Checked
                                                   : Qt::Unchecked);
        if (role == Qt::TextAlignmentRole) {
            if (index.column() == Title)
                return static_cast<int>(Qt::AlignVCenter|
                                        Qt::AlignLeft);
            return static_cast<int>(Qt::AlignVCenter|Qt::AlignLeft/*Right*/);
        }

        if (role == Qt::SizeHintRole) {
            QSize defSize ;
            defSize.setHeight(ROW_HEIGHT);
            return defSize ;
        }
    }
    return QVariant();
}

/*
bool GanttModel::insertRows(int row, int count,
                           const QModelIndex &parent)
{
    if (!rootItem)
        rootItem = new GanttItem;
    GanttItem *parentItem = parent.isValid() ? itemForIndex(parent)
                                            : rootItem;
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        //GanttItem *item = new GanttItem(tr("New Task"), false);
        GanttItem *item = new GanttItem();
        parentItem->insertChild(row, item);
    }
    endInsertRows();
    return true;
}
*/

bool GanttModel::removeRows(int row, int count,
                           const QModelIndex &parent)
{

    GanttItem *item = parent.isValid() ? itemForIndex(parent)
                                      : NULL;
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
    {
        if(parent.isValid())
            delete item->takeChild(row);
        else
            m_itemlist.removeAt(row);
    }
    endRemoveRows();
    return true;
}


/*
QStringList GanttModel::mimeTypes() const
{
    return QStringList() << MimeType;
}


QMimeData *GanttModel::mimeData(const QModelIndexList &indexes) const
{
    Q_ASSERT(indexes.count());
    if (indexes.count() != 1)
        return 0;
    if (GanttItem *item = itemForIndex(indexes.at(0))) {
        QMimeData *mimeData = new QMimeData;
        QByteArray xmlData;
        QXmlStreamWriter writer(&xmlData);
        writeTaskAndChildren(&writer, item);
        mimeData->setData(MimeType, qCompress(xmlData,
                                              MaxCompression));
        return mimeData;
    }
    return 0;
}


bool GanttModel::dropMimeData(const QMimeData *mimeData,
        Qt::DropAction action, int row, int column,
        const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;
    if (action != Qt::MoveAction || column > 0 ||
        !mimeData || !mimeData->hasFormat(MimeType))
        return false;
    if (GanttItem *item = itemForIndex(parent)) {
        //emit stopTiming();
        QByteArray xmlData = qUncompress(mimeData->data(MimeType));
        QXmlStreamReader reader(xmlData);
        if (row == -1)
            row = parent.isValid() ? parent.row()
                                   : rootItem->childCount();
        beginInsertRows(parent, row, row);
        readTasks(&reader, item);
        endInsertRows();
        return true;
    }
    return false;
}
*/

bool GanttModel::isChecked(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;
    return data(index, Qt::CheckStateRole).toInt() == Qt::Checked;
}


QModelIndex GanttModel::moveUp(const QModelIndex &index)
{
    if (!index.isValid() || index.row() <= 0)
        return index;
    GanttItem *item = itemForIndex(index);
    Q_ASSERT(item);
    GanttItem *parent = item->parent();
    qDebug()<<parent;
    Q_ASSERT(parent);
    return moveItem(parent, index.row(), index.row() - 1);
}


QModelIndex GanttModel::moveItem(GanttItem *parent, int oldRow,
                                int newRow)
{
    Q_ASSERT(0 <= oldRow && oldRow < parent->childCount() &&
             0 <= newRow && newRow < parent->childCount());
    parent->swapChildren(oldRow, newRow);
    QModelIndex oldIndex = createIndex(oldRow, 0,
                                       parent->childAt(oldRow));
    QModelIndex newIndex = createIndex(newRow, 0,
                                       parent->childAt(newRow));
    emit dataChanged(oldIndex, newIndex);
    return newIndex;
}

QList<GanttItem *> GanttModel::itemlist() const
{
    return m_itemlist;
}

void GanttModel::setItemlist(const QList<GanttItem *> &itemlist)
{
    m_itemlist = itemlist;
}



QModelIndex GanttModel::moveDown(const QModelIndex &index)
{
    if (!index.isValid())
        return index;
    GanttItem *item = itemForIndex(index);
    Q_ASSERT(item);
    GanttItem *parent = item->parent();
    int newRow = index.row() + 1;
    if (!parent || parent->childCount() <= newRow)
        return index;
    return moveItem(parent, index.row(), newRow);
}

/*
QModelIndex GanttModel::cut(const QModelIndex &index)
{
    if (!index.isValid())
        return index;
    delete cutItem;
    cutItem = itemForIndex(index);
    Q_ASSERT(cutItem);
    GanttItem *parent = cutItem->parent();
    Q_ASSERT(parent);
    int row = parent->rowOfChild(cutItem);
    Q_ASSERT(row == index.row());
    beginRemoveRows(index.parent(), row, row);
    GanttItem *child = parent->takeChild(row);
    endRemoveRows();
    Q_ASSERT(child == cutItem);
    child = 0; // Silence compiler unused variable warning

    if (row > 0) {
        --row;
        return createIndex(row, 0, parent->childAt(row));
    }
    if (parent != rootItem) {
        GanttItem *grandParent = parent->parent();
        Q_ASSERT(grandParent);
        return createIndex(grandParent->rowOfChild(parent), 0, parent);
    }
    return QModelIndex();
}


QModelIndex GanttModel::paste(const QModelIndex &index)
{
    if (!index.isValid() || !cutItem)
        return index;
    GanttItem *sibling = itemForIndex(index);
    Q_ASSERT(sibling);
    GanttItem *parent = sibling->parent();
    Q_ASSERT(parent);
    int row = parent->rowOfChild(sibling) + 1;
    beginInsertRows(index.parent(), row, row);
    parent->insertChild(row, cutItem);
    GanttItem *child = cutItem;
    cutItem = 0;
    endInsertRows();
    return createIndex(row, 0, child);
}


QModelIndex GanttModel::promote(const QModelIndex &index)
{
    if (!index.isValid())
        return index;
    GanttItem *item = itemForIndex(index);
    Q_ASSERT(item);
    GanttItem *parent = item->parent();
    Q_ASSERT(parent);
    if (parent == rootItem)
        return index; // Already a top-level item

    int row = parent->rowOfChild(item);
    GanttItem *child = parent->takeChild(row);
    Q_ASSERT(child == item);
    GanttItem *grandParent = parent->parent();
    Q_ASSERT(grandParent);
    row = grandParent->rowOfChild(parent) + 1;
    grandParent->insertChild(row, child);
    QModelIndex newIndex = createIndex(row, 0, child);
    emit dataChanged(newIndex, newIndex);
    return newIndex;
}


QModelIndex GanttModel::demote(const QModelIndex &index)
{
    if (!index.isValid())
        return index;
    GanttItem *item = itemForIndex(index);
    Q_ASSERT(item);
    GanttItem *parent = item->parent();
    Q_ASSERT(parent);
    int row = parent->rowOfChild(item);
    if (row == 0)
        return index; // No preceding sibling to move this under
    GanttItem *child = parent->takeChild(row);
    Q_ASSERT(child == item);
    GanttItem *sibling = parent->childAt(row - 1);
    Q_ASSERT(sibling);
    sibling->addChild(child);
    QModelIndex newIndex = createIndex(sibling->childCount() - 1, 0,
                                       child);
    emit dataChanged(newIndex, newIndex);
    return newIndex;
}
*/

GanttItem *GanttModel::itemForIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (GanttItem *item = static_cast<GanttItem*>(
                index.internalPointer()))
            return item;
    }
    return NULL;
}


/*
void GanttModel::setTimedItem(const QModelIndex &index)
{
    clearTimedItem();
    timedItem = itemForIndex(index);
    Q_ASSERT(timedItem);
    announceItemChanged(timedItem);
}


void GanttModel::clearTimedItem()
{
    if (timedItem) {
        GanttItem *item = timedItem;
        timedItem = 0;
        announceItemChanged(item);
    }
}


bool GanttModel::isTimedItem(const QModelIndex &index)
{
    return timedItem && itemForIndex(index) == timedItem;
}


void GanttModel::announceItemChanged(GanttItem *item)
{
    if (item == rootItem)
        return;
    GanttItem *parent = item->parent();
    Q_ASSERT(parent);
    int row = parent->rowOfChild(item);
    QModelIndex startIndex = createIndex(row, static_cast<int>(Name),
                                         item);
    QModelIndex endIndex = createIndex(row, static_cast<int>(Total),
                                       item);
    emit dataChanged(startIndex, endIndex);
    // Update the parent & parent's parent etc too
    announceItemChanged(parent);
}


void GanttModel::addDateTimeToTimedItem(const QDateTime &start,
                                       const QDateTime &end)
{
    if (timedItem) {
        timedItem->addDateTime(start, end);
        announceItemChanged(timedItem);
    }
}


void GanttModel::setIconForTimedItem(const QIcon &icon)
{
    m_icon = icon;
    if (timedItem)
        announceItemChanged(timedItem);
}


void GanttModel::incrementEndTimeForTimedItem(int msec)
{
    if (timedItem) {
        timedItem->incrementLastEndTime(msec);
        announceItemChanged(timedItem);
    }
}
*/

void GanttModel::clear()
{
    delete cutItem;
    cutItem = 0;
    //timedItem = 0;
    reset();
}

/*
void GanttModel::load(const QString &filename)
{
    if (!filename.isEmpty())
        m_filename = filename;
    if (m_filename.isEmpty())
        throw AQP::Error(tr("no filename specified"));
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly))
        throw AQP::Error(file.errorString());

    clear();
    rootItem = new GanttItem;
    QXmlStreamReader reader(&file);
    readTasks(&reader, rootItem);
    if (reader.hasError())
        throw AQP::Error(reader.errorString());
    reset();
}
*/

//void GanttModel::readTasks(QXmlStreamReader *reader, GanttItem *task)
//{
//    while (!reader->atEnd()) {
//        reader->readNext();
//        if (reader->isStartElement()) {
//            if (reader->name() == TaskTag) {
//                const QString name = reader->attributes()
//                        .value(NameAttribute).toString();
//                bool done = reader->attributes().value(DoneAttribute)
//                            == "1";
//                task = new GanttItem(name, done, task);
//            }
//            else if (reader->name() == WhenTag) {
//                const QDateTime start = QDateTime::fromString(
//                        reader->attributes().value(StartAttribute)
//                            .toString(), Qt::ISODate);
//                const QDateTime end = QDateTime::fromString(
//                        reader->attributes().value(EndAttribute)
//                            .toString(), Qt::ISODate);
//                Q_ASSERT(task);
//                task->addDateTime(start, end);
//            }
//        }
//        else if (reader->isEndElement()) {
//            if (reader->name() == TaskTag) {
//                Q_ASSERT(task);
//                task = task->parent();
//                Q_ASSERT(task);
//            }
//        }
//    }
//}

/*
void GanttModel::save(const QString &filename)
{
    if (!filename.isEmpty())
        m_filename = filename;
    if (m_filename.isEmpty())
        throw AQP::Error(tr("no filename specified"));
    QFile file(m_filename);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        throw AQP::Error(file.errorString());

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("TIMELOG");
    writer.writeAttribute("VERSION", "2.0");
    writeTaskAndChildren(&writer, rootItem);
    writer.writeEndElement(); // TIMELOG
    writer.writeEndDocument();
}
*/

//void GanttModel::writeTaskAndChildren(QXmlStreamWriter *writer,
//                                     GanttItem *task) const
//{
//    if (task != rootItem) {
//        writer->writeStartElement(TaskTag);
//        writer->writeAttribute(NameAttribute, task->name());
//        writer->writeAttribute(DoneAttribute, task->isDone() ? "1"
//                                                             : "0");
//        QListIterator<
//                QPair<QDateTime, QDateTime> > i(task->dateTimes());
//        while (i.hasNext()) {
//            const QPair<QDateTime, QDateTime> &dateTime = i.next();
//            writer->writeStartElement(WhenTag);
//            writer->writeAttribute(StartAttribute,
//                    dateTime.first.toString(Qt::ISODate));
//            writer->writeAttribute(EndAttribute,
//                    dateTime.second.toString(Qt::ISODate));
//            writer->writeEndElement(); // WHEN
//        }
//    }
//    foreach (GanttItem *child, task->children())
//        writeTaskAndChildren(writer, child);
//    if (task != rootItem)
//        writer->writeEndElement(); // TASK
//}
//*/

QStringList GanttModel::pathForIndex(const QModelIndex &index) const
{
    QStringList path;
    QModelIndex thisIndex = index;
    while (thisIndex.isValid()) {
        path.prepend(data(thisIndex).toString());
        thisIndex = thisIndex.parent();
    }
    return path;
}


QModelIndex GanttModel::indexForPath(const QStringList &path) const
{
    return indexForPath(QModelIndex(), path);
}


QModelIndex GanttModel::indexForPath(const QModelIndex &parent,
                                    const QStringList &path) const
{
    if (path.isEmpty())
        return QModelIndex();
    for (int row = 0; row < rowCount(parent); ++row) {
        QModelIndex thisIndex = index(row, 0, parent);
        if (data(thisIndex).toString() == path.at(0)) {
            if (path.count() == 1)
                return thisIndex;
            thisIndex = indexForPath(thisIndex, path.mid(1));
            if (thisIndex.isValid())
                return thisIndex;
        }
    }
    return QModelIndex();
}
//===================================


//==========common====================
QVariant GanttModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
//---timelog2---
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == Title)
            return trUtf8("Название");
        else if (section == Begin)
            return trUtf8("Начало");
        else if (section == End)
            return trUtf8("Конец");
        else if (section == Duration)
            return trUtf8("Длительность");
    }

    if (role == Qt::SizeHintRole) {
        QSize defSize ;
        defSize.setHeight(ROW_HEIGHT+ROW_HEIGHT);
        //defSize.setWidth(200);
        return defSize ;
    }

    return QVariant();
//--------
}


QModelIndex GanttModel::index(int row, int column, const QModelIndex &parent) const
{
    //-----etm--------
//    if (parent.isValid() && parent.column() != 0)
//        return QModelIndex();

//    GanttItem *parentItem = getItem(parent);

//    GanttItem *childItem = parentItem->child(row);
//    if (childItem)
//        return createIndex(row, column, childItem);
//    else
//        return QModelIndex();
    //---------------

    //-----timelog2------
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    if (!parent.isValid()) { // запрашивают индексы корневых узлов
        return createIndex(row, column, m_itemlist.at(row));
    }

    GanttItem *parentItem = itemForIndex(parent);
    return createIndex(row, column, parentItem->childAt(row));


//    if (!rootItem || row < 0 || column < 0 || column >= ColumnCount
//        || (parent.isValid() && parent.column() != 0))
//        return QModelIndex();
//    GanttItem *parentItem = itemForIndex(parent);
//    Q_ASSERT(parentItem);
//    if (GanttItem *item = parentItem->childAt(row))
//        return createIndex(row, column, item);
//    return QModelIndex();
    //----------------
}

QModelIndex GanttModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    GanttItem* childInfo = static_cast<GanttItem*>(index.internalPointer());
    GanttItem* parentInfo = childInfo->parent();
    if (parentInfo != 0) { // parent запрашивается не у корневого элемента
        return createIndex(findRow(parentInfo), 0, parentInfo);
    }
    else {
        return QModelIndex();
    }
}


int GanttModel::rowCount(const QModelIndex &parent) const
{
    //-------etm-------
//    GanttItem *parentItem = getItem(parent);

//    return parentItem->childCount();
    //----------------

    //----timelog2--------
    if (!parent.isValid())
        return m_itemlist.size();//return 0;
    GanttItem *parentItem = itemForIndex(parent);
    return parentItem ? parentItem->childCount() : 0;
    //-----------------
}


int GanttModel::columnCount(const QModelIndex & /* parent */) const
{
    //-------etm----------
    //return rootItem->columnCount();
    //-----------------
    //------timelog2----------
    //return parent.isValid() && parent.column() != 0 ? 0 : ColumnCount;
    //-----------------
    return ColumnCount;
}


Qt::ItemFlags GanttModel::flags(const QModelIndex &index) const
{
    //----etm---------
//    if (!index.isValid())
//        return 0;

//    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    //-----------------

    //------timelog2-----
    Qt::ItemFlags theFlags = QAbstractItemModel::flags(index);
    if (index.isValid()) {
        theFlags |= Qt::ItemIsSelectable|Qt::ItemIsEnabled;
        if (index.column() == Title)
            theFlags |= Qt::ItemIsUserCheckable|Qt::ItemIsEditable|
                        Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled;
        if (index.column() == Begin)
            theFlags |= Qt::ItemIsEditable;
        if (index.column() == End)
            theFlags |= Qt::ItemIsEditable;
    }
    return theFlags;
    //-------------------
}


bool GanttModel::setData(const QModelIndex &index, const QVariant &value,
                        int role)
{
    //------etm-----------
//    if (role != Qt::EditRole)
//        return false;

//    GanttItem *item = getItem(index);
//    bool result = item->setData(index.column(), value);

//    if (result)
//        emit dataChanged(index, index);

//    return result;
    //---------------------

    //------timelog2--------

    if (!index.isValid() || index.column() == Duration )
        return false;

    if (GanttItem *item = itemForIndex(index)) {
        if (role == Qt::EditRole)
        {
            if (index.column() == Title)
                item->setName(value.toString());
            else if (index.column() == Begin)
            {
                item->setCommonBegin(value.toDateTime());
                item->duration();
            }
            else if (index.column() == End)
            {
                item->setCommonEnd(value.toDateTime());
                item->duration();
            }
        }
        else if (role == Qt::CheckStateRole)
            item->setDone(value.toBool());
        else
            return false;

//        if (index.isValid() && index.column() == Begin)
//        {

//        }

        emit dataChanged(index, index);
        return true;
    }
    return false;
    //-------------------
}

bool GanttModel::hasChildren(const QModelIndex &parent) const
{
    return rowCount(parent)>0;
}

bool GanttModel::addItem(const QString &title,const QDateTime &begin,const QDateTime &end,
                         const QModelIndex &parent)
{
    return insertRow(this->rowCount(parent), title, begin, end, parent);
}

int GanttModel::findRow(const GanttItem *nodeInfo) const
{
    if(nodeInfo == NULL)
        qDebug()<<"findRow nodeInfo = 0";
    const QList<GanttItem*> parentInfoChildren = nodeInfo->parent() != 0 ? nodeInfo->parent()->children(): m_itemlist;
    QList<GanttItem*>::const_iterator position = qFind(parentInfoChildren, nodeInfo);
    return std::distance(parentInfoChildren.begin(), position);
}

void GanttModel::onExpanded(QModelIndex index)
{
    if(index.isValid())
    {
        GanttItem * item = itemForIndex(index);
        if(!item->isExpanded())
            item->setIsExpanded(true);

        emit expanded(index);
    }
}

void GanttModel::onCollapsed(QModelIndex index)
{
    if(index.isValid())
    {
        GanttItem * item = itemForIndex(index);
        if(item->isExpanded())
            item->setIsExpanded(false);

        emit collapsed(index);
    }
}

bool GanttModel::insertRow(int row, QString title, QDateTime begin, QDateTime end, const QModelIndex &parent)
{
    GanttItem *parentItem = parent.isValid() ? itemForIndex(parent)
                                            : NULL;

    beginInsertRows(parent, row, row);
        GanttItem *item = new GanttItem(title, begin, end, false);
        if(parent.isValid())
            parentItem->insertChild(row, item);
        else
            m_itemlist.insert(row,item);
    endInsertRows();

    emit rowInserted(parent, row, row);
    return true;
}
//===============================
