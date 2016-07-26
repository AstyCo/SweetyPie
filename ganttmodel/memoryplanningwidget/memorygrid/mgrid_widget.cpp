#include "mgrid_widget.h"
#include "mgrid_scene.h"

#include "mgrid_item.h"
#include "mgrid_labelitem.h"

#include <QGraphicsGridLayout>
#include <QSizePolicy>

#include <QGraphicsSceneMouseEvent>

#include <QGraphicsProxyWidget>

#include <QPainter>
#include <QGraphicsItemGroup>


#include <QDebug>


MGridWidget::MGridWidget(QGraphicsWidget *parent) :
    QGraphicsWidget(parent)
{
    setItemPerRow(64);
    m_spacing = DEFAULT_SPACING;

    setAcceptHoverEvents(true);
}

MGridWidget::~MGridWidget()
{
}


int MGridWidget::toColumn(int index) const
{
    return (labels()?1:0)+index % itemPerRow();
}

int MGridWidget::toRow(int index) const
{
    return (labels()?1:0)+index / itemPerRow();
}

int MGridWidget::fromPos(int row, int column) const
{
    return row*(itemPerRow()+(labels()?1:0))+column;
}

int MGridWidget::toMemoryAdress(int row, int column) const
{
    return (row-(labels()?1:0))*itemPerRow()+(column-(labels()?1:0));
}

int MGridWidget::toMemoryAdress(int pos) const
{
    return toMemoryAdress(toRow(pos),toColumn(pos));
}



void MGridWidget::setupMatrix(QList<MGridItem*> mem_items)
{

    removeLabels();
    QGraphicsGridLayout * gridLayout = new QGraphicsGridLayout;

    gridLayout->setContentsMargins(-0, -0, 0, 0);
    gridLayout->setSpacing(m_spacing);

    if(labels())
    {
        MGridLabelItem* emptyLabelItem = new MGridLabelItem(QString(),this);
        gridLayout->addItem(emptyLabelItem,0,0);
        m_labelItems.append(emptyLabelItem);

        for(int j=0;j<itemPerRow();++j)
        {
            MGridLabelItem* labelItem = new MGridLabelItem(QString::number(j),this);
            gridLayout->addItem(labelItem,0,j+1);
            m_labelItems.append(labelItem);
        }
    }
    for(int i=0;i<mem_items.size();++i)
    {
        if(labels()&&!(i%itemPerRow()))
        {
            MGridLabelItem* labelItem = new MGridLabelItem(QString::number(i/itemPerRow()),this);
            gridLayout->addItem(labelItem,i/itemPerRow()+1,0);
            m_labelItems.append(labelItem);
        }
        gridLayout->addItem(mem_items[i], toRow(i), toColumn(i));
    }

    setLayout(gridLayout);

    adjustSize();
}

bool MGridWidget::labels() const
{
    return m_labels;
}

qreal MGridWidget::margins() const
{
    return m_margins;
}

void MGridWidget::setMargins(const qreal &margins)
{
    m_margins = margins;
}


// reaches mouse over's property control.
void MGridWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    MGridScene* p_scene = dynamic_cast<MGridScene*>(scene());
    if(!p_scene)
        return;
    p_scene->m_mouseOverItem = NULL;
    p_scene->m_mouseOverUnit = NULL;
}

qreal MGridWidget::spacing() const
{
    return m_spacing;
}

void MGridWidget::setSpacing(const qreal &spacing)
{
    m_spacing = spacing;
}



void MGridWidget::setLabels(bool labels)
{
    m_labels = labels;
}

void MGridWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    // empty
}


QVariant MGridWidget::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemTransformHasChanged)
    {
        qDebug() << "MemoryWidget itemTransf";
    }
    return QGraphicsItem::itemChange(change,value);
}

void MGridWidget::removeLabels()
{
    foreach(MGridLabelItem* item, m_labelItems)
    {
        scene()->removeItem(item);
        item->setParentItem(NULL);
        m_labelItems.removeOne(item);
    }
}


int MGridWidget::itemPerRow() const
{
    return m_itemPerRow;
}

void MGridWidget::setItemPerRow(int itemPerRow)
{
    m_itemPerRow = itemPerRow;

}
