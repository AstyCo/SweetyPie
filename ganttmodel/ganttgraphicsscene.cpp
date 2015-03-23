#include "ganttgraphicsscene.h"

#include "ganttgraphicsview.h"

#include <QPainter>
#include <QGraphicsTextItem>
#include <QRect>
#include <QStyle>
#include <QLabel>

#include "QDebug"
#include "QTimer"

static const int GRID_STEP = 20;

inline qreal round(qreal val, int step) {
   int tmp = int(val) + step /2;
   tmp -= tmp % step;
   return qreal(tmp);
}

GanttGraphicsScene::GanttGraphicsScene(GanttModel *model, /*QDateTime begin, QDateTime end,*/ QObject *parent) :
    QGraphicsScene(parent)
{
    /*QGraphicsWidget * */test = new QGraphicsWidget;
    m_layout = new QGraphicsLinearLayout(Qt::Vertical);
    m_layout->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //test->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_layout->setSpacing(0);

    test->setPos(0,0);

    QDateTime begin, end;
    QModelIndex seed = model->index(0,0);
    if (seed.isValid())
    {
        begin = model->itemForIndex(model->index(0,0))->begin();
        end = model->itemForIndex(model->index(0,0))->end();
    }
    else
    {
        begin = QDateTime::currentDateTime();
        end = begin.addDays(2);
    }
//    m_begin = begin;
//    m_end = end;
    m_scale = ScaleDay;

    m_zoom = ZoomDay;

    m_header = new GanttGraphicsHeader(begin, end, m_scale);
    m_header->setPos(0,0);
    m_header->setZValue(1.1);
    //m_header->setFlags(QGraphicsItem::ItemIsMovable);
    this->addItem(m_header);


    m_headerItem = new GanttGraphicsHeaderItem(begin, begin.addDays(2), m_zoom);
    m_headerItem->setPos(0,0);
    m_headerItem->setZValue(1.0);
    //m_headerItem->setFlags(QGraphicsItem::ItemIsMovable);
    m_layout->addItem(m_headerItem);

    m_itemLayout = new QGraphicsLinearLayout(Qt::Vertical);
    m_itemLayout->setContentsMargins(0,0,0,0);
    m_itemLayout->setSpacing(10);
    m_model = model;

//    for(int i = 0; i<m_model->rowCount(); ++i)
//    {
//        GanttItem * newItem = m_model->itemForIndex(m_model->index(i,0));
//        if (newItem->begin() < m_begin)
//            m_begin = newItem->begin();

//        if (newItem->end() > m_end)
//            m_end = newItem->end();

//        m_header->setBegin(m_begin);
//        m_header->setEnd(m_end);
//        m_item = new GanttGraphicsItem(newItem, m_scale, m_begin, m_end);
//        m_itemLayout->addItem(m_item);

//        QPersistentModelIndex persIndex(m_model->index(i,0));
//        m_proxyList.append(index2row(persIndex, m_itemLayout->count()-1));

//        setItems(m_model->index(i,0));
//    }

    setItems(QModelIndex(), 0);

    m_itemLayout->addStretch(1);
    m_layout->addItem(m_itemLayout);

    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(5);
    //m_layout->addStretch(1);

    test->setLayout(m_layout);
    this->addItem(test);

    //connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateItems(m_model)));

    m_cursor = 0;
    QTimer * timer; timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(10);
}

GanttGraphicsScene::~GanttGraphicsScene()
{
    delete m_header;
    delete m_model;
    delete m_layout;
    delete m_item;
    delete m_itemLayout;
    delete test;
}

void GanttGraphicsScene::drawBackground(QPainter *painter, const QRectF &rect)
{
   int step = GRID_STEP;
   QPen newPen(Qt::DashLine);
   newPen.setColor(QColor(Qt::gray));
   //newPen.setColor(QColor(200, 200, 255, 125));
   painter->setPen(newPen);
   // draw horizontal grid
   qreal start = round(rect.top(), step);
   if (start > rect.top()) {
      start -= step;
   }

   qreal y = start - step;
   //for (qreal y = start - step; y < rect.bottom(); ) {
   for (int i = 0; i < m_proxyList.count()+3; ++i ) {
      y += step;
      painter->drawLine(rect.left(), y, rect.right(), y);
   }
   // now draw vertical grid
   int stepV = GRID_STEP;
   start = round(rect.left(), stepV);
   if (start > rect.left()) {
      start -= stepV;
   }
   for (qreal x = start - stepV; x < rect.right(); ) {
      x += stepV;
      painter->drawLine(x, rect.top(), x, rect.bottom());
   }

   //qDebug()<<"header Left"<<m_header->rect().left()<<"header Right"<<m_header->rect().right()<<"  scene Left"<<rect.left()<<"scene Right"<<rect.right();
   //qDebug()<<"header Left"<<m_header->pos().x()<<"header Right"<<m_header->rect().right()<<"  scene Left"<<rect.left()<<"scene Right"<<rect.right();

//   if(m_header->pos().x() < 0)
//   {
//       if(m_header->rect().width()+m_header->pos().x() == rect.right())
//       {
//           m_header->setEnd(m_header->end().addDays(2));
//           m_header->createHeader();
//       }
//   }

   //оси координат сцены
//   QPen zeroPen(Qt::SolidLine);
//   zeroPen.setColor(QColor(Qt::red));
//   painter->setPen(zeroPen);

//   painter->drawLine(0,rect.top(),0,rect.bottom());
//   painter->drawLine(rect.left(),0,rect.right(),0);

}

void GanttGraphicsScene::onTimer()
{
    m_cursor += 1;
    update();
}

void GanttGraphicsScene::setInterval(QDateTime begin, QDateTime end)
{
    //m_begin = begin;
    //m_end = end;
}

void GanttGraphicsScene::calcRowFromIndex()
{
    //m_model->
}

void GanttGraphicsScene::deleteGraphicsItem(QModelIndex index)
{
    delete m_itemLayout->itemAt(index.row());
}

void GanttGraphicsScene::updateItems(GanttModel *model)
{
    for(int row = /*model->rowCount()*/m_itemLayout->count()-1; row >= 0; --row)
    {
        //qDebug()<<"row"<<row;
        delete m_itemLayout->itemAt(row);
    }
    //setItems(model);
    m_itemLayout->addStretch(1);
}

void GanttGraphicsScene::editAdd(QModelIndex index)
{
    qDebug()<<"parent"<<index;

    GanttItem * newItem;
    QModelIndex itemIndex = m_model->index(0,0, index);
    int row = itemIndex.row();
    newItem = m_model->itemForIndex(itemIndex);
    m_item = new GanttGraphicsItem(newItem, m_scale, m_header);
//TODO index.parent
    while(itemIndex.parent().isValid())
    {
        row += itemIndex.parent().row()+1;
        itemIndex = itemIndex.parent();
        qDebug()<<"row"<<row;
    }
    m_itemLayout->insertItem(/*index.parent().row()+1*/row, m_item);
}

//void GanttGraphicsScene::editDelete(QModelIndex index)
//{
//    int row = m_proxyList.indexOf(index);
//    if(m_model->hasChildren(index))
//    {
//        for(int i = m_model->rowCount(index)-1; i>=0; --i)
//        {
//            QModelIndex childIndex = index.child(i,0);
//            editDelete(childIndex);
//            //delete m_itemLayout->itemAt(row+i);
//        }
//    }
//    delete m_itemLayout->itemAt(row);
//}

void GanttGraphicsScene::editMoveUp(QModelIndex index)
{
    m_itemLayout->removeAt(index.row());
    // m_itemLayout->insertItem(index.row()-1, m_itemLayout->itemAt(index.row()));
}

void GanttGraphicsScene::onExpanded(QModelIndex index)
{

    if(!index.isValid())
        return;


    int rowIndex = -1;
    for(int i = 0; i < m_proxyList.size(); ++i)
    {
        if(m_proxyList.at(i) == index)
        {
            rowIndex = i+1;
            break;
        }
        else
        {
            rowIndex = m_proxyList.size();
        }
    }
    if(rowIndex == -1)
        return;

    setItems(index, rowIndex);

    update();
}

void GanttGraphicsScene::onCollapsed(QModelIndex index)
{

    if(!index.isValid())
        return;

    int rowIndex = m_proxyList.size();
    int rowSibling = 0;

    for(int i = 0; i < m_proxyList.size(); ++i)
    {
        if(index == m_proxyList.at(i))
        {
            rowIndex = i;
            break;
        }
    }


    QModelIndex sibIndex = index.sibling(index.row()+1,0);
    if(sibIndex.isValid())
    {
        rowSibling = m_proxyList.indexOf(sibIndex, rowIndex);
    }
    else
    {
        while(index.parent().isValid()&& (!sibIndex.isValid()))
        {
            index = index.parent();
            sibIndex = index.sibling(index.row()+1,0);
        }
        if(sibIndex.isValid())
            rowSibling = m_proxyList.indexOf(sibIndex, rowIndex);
        else
            rowSibling = m_proxyList.size()/*-1*/;
    }

    for(int i = rowSibling-1; i > rowIndex; --i)
    {
        //use this someday to show stacking items
        //m_itemLayout->itemAt(i)->graphicsItem()->setY(m_itemLayout->itemAt(rowIndex)->graphicsItem()->y());

        delete m_itemLayout->itemAt(i);
        m_proxyList.removeAt(i);
    }
    update();
}

void GanttGraphicsScene::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{


    GanttItem * newItem;
    int rowIndex;
    QModelIndex treeIndex = m_model->index(topLeft.row(),0, topLeft.parent());

    newItem = m_model->itemForIndex(treeIndex);
    //qDebug()<<"Changed!"<<topLeft<<bottomRight<<newItem->begin()<<newItem->end();
    rowIndex = m_proxyList.indexOf(treeIndex, 0);
    delete m_itemLayout->itemAt(rowIndex);

    qreal durationTillBegin;

    bool flag = true;
    if (flag == (newItem->begin() < m_header->begin()))
    {
        //==something for header pos======

        durationTillBegin = newItem->begin().secsTo(m_header->begin());
        qreal secWidth = 20;
        m_header->setBegin(newItem->begin());
        qreal newDuration = m_header->begin().secsTo(newItem->begin());
        durationTillBegin = (durationTillBegin+newDuration)*secWidth;

        switch (m_scale) {
        case ScaleSecond:
            break;
        case ScaleMinute:
            durationTillBegin = durationTillBegin/60;
            break;
        case ScaleHour:
            durationTillBegin = durationTillBegin/3600;
            break;
        case ScaleDay:
            durationTillBegin = durationTillBegin/86400;
            break;
        case ScaleMonth:
            //m_width = m_width/86400;
            break;
        default:
            break;
        }

    }
    else if (flag == (newItem->end() > m_header->end()))
    {
        //m_end = newItem->end();
        m_header->setEnd(newItem->end());
    }
    else flag = false;

    if(flag)
    {
        m_header->createHeader();
        m_header->moveBy(-durationTillBegin,0);
    }

    m_item = new GanttGraphicsItem(newItem, m_scale, m_header);
    m_itemLayout->insertItem(rowIndex,m_item);

}

void GanttGraphicsScene::onRowsInserted(const QModelIndex &parent, int start, int end)
{
    GanttItem * newItem;
    int rowIndex;

    if(parent.isValid())
    {
        newItem = m_model->itemForIndex(parent);
        if(!newItem->isExpanded())
            return;
        else
        {
            rowIndex = m_proxyList.indexOf(parent, 0);
            rowIndex +=start+1;
        }
    }
    else
    {
        rowIndex = start;
    }

    for(int row = start; row <= end; ++row)
    {

        QModelIndex childIndex = m_model->index(row,0,parent);


        newItem = m_model->itemForIndex(childIndex);

        if (newItem->begin() < m_header->begin())
            m_header->setBegin(newItem->begin());

        if (newItem->end() > m_header->end())
            m_header->setEnd(newItem->end());

//        m_header->setBegin(m_begin);
//        m_header->setEnd(m_end);
        m_item = new GanttGraphicsItem(newItem, m_scale, m_header);
        m_itemLayout->insertItem(rowIndex,m_item);

        //qDebug()<<"insert item at:"<<rowIndex;

        QPersistentModelIndex persIndex(childIndex);
        m_proxyList.insert(rowIndex, persIndex);
        rowIndex += 1;

        //if(m_model->hasChildren(childIndex))
        //    setItems(childIndex, rowIndex);
    }
    m_header->createHeader();
    m_header->update();
    update();
}

void GanttGraphicsScene::onRowsRemoved(const QModelIndex &parent, int start, int end)
{
    for(int i = m_endDeleteRow-1; i >= m_beginDeleteRow; --i)
    {
        delete m_itemLayout->itemAt(i);
        m_proxyList.removeAt(i);
    }
    update();
}

void GanttGraphicsScene::onRowsAboutToBeRemoved(const QModelIndex & parent, int start, int end)
{
    //QModelIndex index = parent;

    if(!parent.isValid())
    {

        QModelIndex startIndex = m_model->index(start,0);
        m_beginDeleteRow = m_proxyList.indexOf(startIndex, 0);
        if(startIndex.sibling(startIndex.row()+1,0).isValid())
        {
            m_endDeleteRow = m_proxyList.indexOf(startIndex.sibling(startIndex.row()+1,0), m_beginDeleteRow);
        }
        else
        {
            m_endDeleteRow = m_proxyList.size();
        }
        return;
    }

    QModelIndex index = parent.child(start,0);
    for(int i = 0; i < m_proxyList.size(); ++i)
    {
        if(index == m_proxyList.at(i))
        {
            m_beginDeleteRow = i;
            break;
        }
    }

    QModelIndex sibIndex = index.sibling(index.row()+1,0);
    if(sibIndex.isValid())
    {
        m_endDeleteRow = m_proxyList.indexOf(sibIndex, m_beginDeleteRow);
    }
    else
    {
        while(index.parent().isValid()&& (!sibIndex.isValid()))
        {
            index = index.parent();
            sibIndex = index.sibling(index.row()+1,0);
        }
        if(sibIndex.isValid())
            m_endDeleteRow = m_proxyList.indexOf(sibIndex, m_beginDeleteRow);
        else
            m_endDeleteRow = m_proxyList.size()/*-1*/;
    }
}

void GanttGraphicsScene::onMoveUp(QModelIndex index)
{

}






int GanttGraphicsScene::setItems(QModelIndex parent, int rowIndex)
{
    GanttItem * newItem;

    if(parent.isValid())
    {
        newItem = m_model->itemForIndex(parent);
        if(!newItem->isExpanded())
            return rowIndex;
    }
    else
    {
        rowIndex = m_proxyList.size();
    }

    //int rowIndex1 = rowIndex;
    for(int row = 0; row < m_model->rowCount(parent); ++row)
    {

        QModelIndex childIndex = m_model->index(row,0,parent);
        newItem = m_model->itemForIndex(childIndex);

        if (newItem->begin() < m_header->begin())
        {

            //==something for header pos======
            qreal durationTillBegin = newItem->begin().secsTo(m_header->begin());
            qreal secWidth = 20;

            m_header->setBegin(newItem->begin());
            qreal newDuration = m_header->begin().secsTo(newItem->begin());
            durationTillBegin = (durationTillBegin+newDuration)*secWidth;



            //durationTillBegin = durationTillBegin*secWidth;

            switch (m_scale) {
            case ScaleSecond:
                break;
            case ScaleMinute:
                durationTillBegin = durationTillBegin/60;
                break;
            case ScaleHour:
                durationTillBegin = durationTillBegin/3600;
                break;
            case ScaleDay:
                durationTillBegin = durationTillBegin/86400;
                break;
            case ScaleMonth:
                //m_width = m_width/86400;
                break;
            default:
                break;
            }
            m_header->setX(m_header->x()-durationTillBegin);
            //=====================
            //m_begin = newItem->begin();
            m_header->setBegin(newItem->begin());
        }

        if (newItem->end() > m_header->end())
            //m_end = newItem->end();
            m_header->setEnd(newItem->end());

        //m_header->setBegin(m_begin);
        //m_header->setEnd(m_end);

        m_item = new GanttGraphicsItem(newItem, m_scale, m_header);
        m_itemLayout->insertItem(rowIndex,m_item);

        QPersistentModelIndex persIndex(childIndex);
        m_proxyList.insert(rowIndex, persIndex);
        rowIndex/*1*/ += 1;

        if(m_model->hasChildren(childIndex))
            rowIndex/*1*/ = setItems(childIndex, rowIndex/*1*/);
    }

    m_header->createHeader();
    m_header->update();

    return rowIndex/*2*/;
}
