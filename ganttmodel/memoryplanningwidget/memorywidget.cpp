#include "memorywidget.hpp"
#include "memoryscene.hpp"

#include "memorystatus.hpp"

#include "memoryitem.hpp"
#include "labelitem.hpp"

#include <QGraphicsGridLayout>
#include <QSizePolicy>

#include <QGraphicsSceneMouseEvent>

#include <QGraphicsProxyWidget>

#include <QPainter>
#include <QGraphicsItemGroup>


#include <QDebug>


MemoryWidget::MemoryWidget(QGraphicsWidget *parent) :
    QGraphicsWidget(parent)
{
    setItemPerRow(64);
    m_spacing = DEFAULT_SPACING;
    m_memoryStatus = NULL;


}

MemoryWidget::~MemoryWidget()
{
}


int MemoryWidget::toColumn(int index) const
{
    return (labels()?1:0)+index % itemPerRow();
}

int MemoryWidget::toRow(int index) const
{
    return (labels()?1:0)+index / itemPerRow();
}

int MemoryWidget::fromPos(int row, int column) const
{
    return row*(itemPerRow()+(labels()?1:0))+column;
}

int MemoryWidget::toMemoryAdress(int row, int column) const
{
    return (row-(labels()?1:0))*itemPerRow()+(column-(labels()?1:0));
}

int MemoryWidget::toMemoryAdress(int pos) const
{
    return toMemoryAdress(toRow(pos),toColumn(pos));
}



void MemoryWidget::setupMatrix(QList<MemoryItem*> mem_items)
{

    removeLabels();
    QGraphicsGridLayout * gridLayout = new QGraphicsGridLayout;

    gridLayout->setContentsMargins(-0, -0, 0, 0);
    gridLayout->setSpacing(m_spacing);

    if(labels())
    {
        LabelItem* emptyLabelItem = new LabelItem(QString(),this);
        gridLayout->addItem(emptyLabelItem,0,0);
        m_labelItems.append(emptyLabelItem);

        for(int j=0;j<itemPerRow();++j)
        {
            LabelItem* labelItem = new LabelItem(QString::number(j),this);
            gridLayout->addItem(labelItem,0,j+1);
            m_labelItems.append(labelItem);
        }
    }
    for(int i=0;i<mem_items.size();++i)
    {
        if(labels()&&!(i%itemPerRow()))
        {
            LabelItem* labelItem = new LabelItem(QString::number(i/itemPerRow()),this);
            gridLayout->addItem(labelItem,i/itemPerRow()+1,0);
            m_labelItems.append(labelItem);
        }
        gridLayout->addItem(mem_items[i], toRow(i), toColumn(i));
    }
    {
        adjustSize();
        // MemoryStatus
        if(!m_memoryStatus)
        {
            m_memoryStatus = new MemoryStatus(this);
            m_memoryStatus->setStatusLabel(tr("Info:"));
            m_memoryStatus->setItemInfo(tr("Item Info"));
            m_memoryStatus->setUnitInfo(tr("Unit Info"));
        }

        gridLayout->addItem(m_memoryStatus,toRow(mem_items.size()-1)+1,0,1,itemPerRow()+(labels()?1:0));
        gridLayout->setRowFixedHeight(toRow(mem_items.size()-1)+1,20);
    }

    qDebug() <<"MemoryStatus sz: ";
    qDebug() << m_memoryStatus->boundingRect();
    setLayout(gridLayout);

    adjustSize();
}

QGraphicsLayout *MemoryWidget::layout()
{
    return QGraphicsWidget::layout();
}

qreal MemoryWidget::margins() const
{
    return m_margins;
}

void MemoryWidget::setMargins(const qreal &margins)
{
    m_margins = margins;
}

qreal MemoryWidget::spacing() const
{
    return m_spacing;
}

void MemoryWidget::setSpacing(const qreal &spacing)
{
    m_spacing = spacing;
}

void MemoryWidget::setStatusLabel(const QString &text)
{
    m_memoryStatus->setStatusLabel(text);
}

QString MemoryWidget::statusLabel() const
{
    return m_memoryStatus->statusLabel();
}

void MemoryWidget::setItemInfo(const QString &text)
{
    m_memoryStatus->setItemInfo(text);
}

QString MemoryWidget::itemInfo() const
{
    return m_memoryStatus->itemInfo();
}

void MemoryWidget::setUnitInfo(const QString &text)
{
    m_memoryStatus->setUnitInfo(text);
}

QString MemoryWidget::unitInfo() const
{
    return m_memoryStatus->unitInfo();
}
bool MemoryWidget::labels() const
{
    return m_labels;
}

void MemoryWidget::setLabels(bool labels)
{
    m_labels = labels;
}

void MemoryWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    // empty
}

void MemoryWidget::memoryStatusUpdate(const QRectF &rect)
{
    m_memoryStatus->update(rect);
}

QVariant MemoryWidget::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemTransformHasChanged)
    {
        qDebug() << "MemoryWidget itemTransf";
    }
    return QGraphicsItem::itemChange(change,value);
}

void MemoryWidget::removeLabels()
{
    foreach(LabelItem* item, m_labelItems)
    {
        scene()->removeItem(item);
        item->setParentItem(NULL);
        m_labelItems.removeOne(item);
    }
}

void MemoryWidget::transformChanged(const QTransform &transform)
{
    m_memoryStatus->transformChanged(transform);
}

int MemoryWidget::itemPerRow() const
{
    return m_itemPerRow;
}

void MemoryWidget::setItemPerRow(int itemPerRow)
{
    m_itemPerRow = itemPerRow;

}
