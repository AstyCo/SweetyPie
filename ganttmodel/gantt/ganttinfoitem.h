#ifndef GANTTINFOITEM_H
#define GANTTINFOITEM_H

#include <QString>
#include <QModelIndex>

class GanttInfoNode;

class GanttInfoItem : public QObject
{
    Q_OBJECT

public:
    GanttInfoItem(QObject *parent = NULL);

    GanttInfoNode *parent() const;

    virtual int columnCount() const = 0;

    QString title() const;

    int row() const;

    virtual qreal height() const = 0;
    virtual qreal pos() const;

    QModelIndex index() const;

signals:
    void parentChanged();
    void indexChanged();
    void titleChanged();

    void changed();

public slots:
    void setTitle(const QString &title);
    void setIndex(const QModelIndex &index);
    void setParent(GanttInfoNode *parent);

protected:
    virtual int indexOf(const GanttInfoItem* p_item) const;

private:
    GanttInfoNode* m_parent;
    QModelIndex m_index;
    QString m_title;
};

#endif // GANTTINFOITEM_H
