#ifndef GANTTMODEL_H
#define GANTTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "ganttitem.h"

#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>
#include "ganttproject_global.h"

class GANTTMODELSHARED_EXPORT  GanttModel : public QAbstractItemModel
{
    Q_OBJECT
public:
//=======editabletreemodel===========
//    explicit GanttModel(const QStringList &headers, const QString &data,
//                        QObject *parent = 0);
//    ~GanttModel();

//    QVariant data(const QModelIndex &index, int role) const;
//    bool setHeaderData(int section, Qt::Orientation orientation,
//                       const QVariant &value, int role = Qt::EditRole);
//    bool insertRows(int position, int rows,
//                    const QModelIndex &parent = QModelIndex());
//    bool removeRows(int position, int rows,
//                    const QModelIndex &parent = QModelIndex());


//    bool insertColumns(int position, int columns,
//                       const QModelIndex &parent = QModelIndex());
//    bool removeColumns(int position, int columns,
//                       const QModelIndex &parent = QModelIndex());
//=======================
//======timelog2=========
    explicit GanttModel(QObject *parent=0)
        : QAbstractItemModel(parent), cutItem(0) {}
    ~GanttModel() { delete cutItem; }

    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    bool setHeaderData(int, Qt::Orientation, const QVariant&,
                       int=Qt::EditRole) { return false; }
//    bool insertRows(int row, int count,
//                    const QModelIndex &parent=QModelIndex());
    bool removeRows(int row, int count,
                    const QModelIndex &parent=QModelIndex());



    Qt::DropActions supportedDragActions() const
        { return Qt::MoveAction; }
    Qt::DropActions supportedDropActions() const
        { return Qt::MoveAction; }

//    QStringList mimeTypes() const;
//    QMimeData *mimeData(const QModelIndexList &indexes) const;
//    bool dropMimeData(const QMimeData *mimeData,
//            Qt::DropAction action, int row, int column,
//            const QModelIndex &parent);

    bool isChecked(const QModelIndex &index) const;
    bool hasCutItem() const { return cutItem; }

    QModelIndex moveUp(const QModelIndex &index);
    QModelIndex moveDown(const QModelIndex &index);
//    QModelIndex cut(const QModelIndex &index);
//    QModelIndex paste(const QModelIndex &index);
//    QModelIndex promote(const QModelIndex &index);
//    QModelIndex demote(const QModelIndex &index);

//    void setTimedItem(const QModelIndex &index);
//    void clearTimedItem();
//    bool isTimedItem(const QModelIndex &index);
//    void addDateTimeToTimedItem(const QDateTime &start,
//                                const QDateTime &end);
//    void setIconForTimedItem(const QIcon &icon=QIcon());
//    void incrementEndTimeForTimedItem(int msec);

    void clear();
//    QString filename() const { return m_filename; }
//    void setFilename(const QString &filename)
//        { m_filename = filename; }
//    void load(const QString &filename=QString());
//    void save(const QString &filename=QString());

    QStringList pathForIndex(const QModelIndex &index) const;
    QModelIndex indexForPath(const QStringList &path) const;
//=====================

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);

    bool hasChildren(const QModelIndex &parent) const;


//    void addItem(QDateTime begin, QDateTime end, GanttItem::ItemType type);
//    void addItem(QDateTime begin, QDateTime duration, GanttItem::ItemType type);
//    void addItem(QDateTime end, QDateTime duration, GanttItem::ItemType type);
    //void addItem(QString title, QDateTime begin, QDateTime end, GanttItem::ItemType type);

    bool addItem(const QString &title,const QDateTime &begin,const QDateTime &end,
                 const QModelIndex &parent = QModelIndex());

    bool insertRow(int row, QString title, QDateTime begin, QDateTime end,
                  const QModelIndex &parent = QModelIndex());

    QList<GanttItem *> itemlist() const;
    void setItemlist(const QList<GanttItem *> &itemlist);

    GanttItem *itemForIndex(const QModelIndex &index) const;

private:
    //=======editabletreemodel===========
    //    void setupModelData(const QStringList &lines, GanttItem *parent);
//    GanttItem *getItem(const QModelIndex &index) const;

//=====================
//======timelog2=======
    

    void readTasks(QXmlStreamReader *reader, GanttItem *task);
    void writeTaskAndChildren(QXmlStreamWriter *writer,
                              GanttItem *task) const;
//    void announceItemChanged(GanttItem *item);

    QModelIndex indexForPath(const QModelIndex &parent,
                             const QStringList &path) const;
    QModelIndex moveItem(GanttItem *parent, int oldRow, int newRow);

//    QString m_filename;
//    QIcon m_icon;
    GanttItem *cutItem;
    //GanttItem *timedItem;
//=====================
    QList<GanttItem*> m_itemlist;

    int findRow(const GanttItem *nodeInfo) const;
signals:
    void expanded(QModelIndex);
    void collapsed(QModelIndex);
    void rowInserted(QModelIndex, int, int);

public slots:
    void onExpanded(QModelIndex index);
    void onCollapsed(QModelIndex index);

};

#endif // GANTTMODEL_H
