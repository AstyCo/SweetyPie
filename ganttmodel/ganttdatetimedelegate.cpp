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
    //dtEdit->dateTimeChanged();
    QDateTime value = dtEdit->dateTime();
    model->setData(index, value, Qt::EditRole);

}

void GanttDateTimeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{

    editor->setGeometry(option.rect);

}


//====================================


////! [1]
//QWidget *GanttDateTimeDelegate::createEditor(QWidget *parent,
//    const QStyleOptionViewItem &/* option */,
//    const QModelIndex &/* index */) const
//{
//    QSpinBox *editor = new QSpinBox(parent);
//    editor->setMinimum(0);
//    editor->setMaximum(100);

//    return editor;
//}
////! [1]

////! [2]
//void GanttDateTimeDelegate::setEditorData(QWidget *editor,
//                                    const QModelIndex &index) const
//{
//    int value = index.model()->data(index, Qt::EditRole).toInt();

//    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
//    spinBox->setValue(value);
//}
////! [2]

////! [3]
//void GanttDateTimeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
//                                   const QModelIndex &index) const
//{
//    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
//    spinBox->interpretText();
//    int value = spinBox->value();

//    model->setData(index, value, Qt::EditRole);
//}
////! [3]

////! [4]
//void GanttDateTimeDelegate::updateEditorGeometry(QWidget *editor,
//    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
//{
//    editor->setGeometry(option.rect);
//}
////! [4]
