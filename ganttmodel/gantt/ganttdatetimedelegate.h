#ifndef GANTTDATETIMEDELEGATE_H
#define GANTTDATETIMEDELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QDateTimeEdit>

class GanttDateTimeDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit GanttDateTimeDelegate(QObject *parent = 0);


    //-spinboxdelegate example-
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
    //--

signals:

public slots:

};

#endif // GANTTDATETIMEDELEGATE_H
