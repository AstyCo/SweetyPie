#include "gantttreedelegate.h"

#include "utcdatetime.h"


#include <QDateTimeEdit> //

#include <QDebug>

GanttTreeDelegate::GanttTreeDelegate(QObject *parent) :
    QItemDelegate(parent)
{

}

QWidget *GanttTreeDelegate::createEditor(QWidget *parent,
                                             const QStyleOptionViewItem &/*option*/,
                                             const QModelIndex &/*index*/) const
{

    QDateTimeEdit *editor = new QDateTimeEdit(parent);
    editor->setCalendarPopup(true);
    return editor;

}

void GanttTreeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

    QDateTime value = index.model()->data(index, Qt::EditRole).toDateTime();
    QDateTimeEdit *dtEdit = static_cast<QDateTimeEdit*>(editor);
    dtEdit->setDateTime(value);

}

void GanttTreeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

    QDateTimeEdit *dtEdit = static_cast<QDateTimeEdit*>(editor);
    QDateTime value = dtEdit->dateTime();
    model->setData(index, value, Qt::EditRole);

}

void GanttTreeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
