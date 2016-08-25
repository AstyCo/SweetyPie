#ifndef GANTTTREEDELEGATE_H
#define GANTTTREEDELEGATE_H

#include <QItemDelegate>

class GanttTreeDelegate : public QItemDelegate
{
public:
    explicit GanttTreeDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // GANTTTREEDELEGATE_H
