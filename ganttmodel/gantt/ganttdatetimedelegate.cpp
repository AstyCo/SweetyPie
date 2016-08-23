#include "ganttdatetimedelegate.h"
#include <QDateTime>
#include <QDebug>
//#include <QSpinBox>

GanttDateTimeDelegate::GanttDateTimeDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *GanttDateTimeDelegate::createEditor(QWidget *parent,
                                             const QStyleOptionViewItem &/*option*/,
                                             const QModelIndex &/*index*/) const
{

    QDateTimeEdit *editor = new QDateTimeEdit(parent);
    editor->setCalendarPopup(true);
    return editor;

}

void GanttDateTimeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

    QDateTime value = index.model()->data(index, Qt::EditRole).toDateTime();
    QDateTimeEdit *dtEdit = static_cast<QDateTimeEdit*>(editor);
    dtEdit->setDateTime(value);

}

void GanttDateTimeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

    QDateTimeEdit *dtEdit = static_cast<QDateTimeEdit*>(editor);
    QDateTime value = dtEdit->dateTime();
    model->setData(index, value, Qt::EditRole);

}

void GanttDateTimeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{

    editor->setGeometry(option.rect);

}
