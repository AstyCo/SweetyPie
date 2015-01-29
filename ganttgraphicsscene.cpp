#include "ganttgraphicsscene.h"

#include <QPainter>
#include <QGraphicsTextItem>
#include <QRect>
#include <QStyle>
#include <QLabel>

#include "QDebug"

static const int GRID_STEP = 20;

inline qreal round(qreal val, int step) {
   int tmp = int(val) + step /2;
   tmp -= tmp % step;
   return qreal(tmp);
}

GanttGraphicsScene::GanttGraphicsScene(GanttModel *model, /*QDateTime begin, QDateTime end,*/ QObject *parent) :
    QGraphicsScene(parent)
{
    QGraphicsWidget * test = new QGraphicsWidget;
    m_layout = new QGraphicsLinearLayout(Qt::Vertical);

//    QGraphicsTextItem * pText = new QGraphicsTextItem("Test");
//    pText->setFlags(QGraphicsItem::ItemIsMovable);
//    addItem(pText);


    //QDateTime begin = QDateTime::currentDateTime();
    QDateTime begin = QDateTime::fromString("29.01.2015", "dd.MM.yyyy");
    QDateTime end = begin.addMonths(1);
    m_begin = begin;
    m_end = end;
    m_header = new GanttGraphicsHeader(begin, end, ScaleDay);
    m_header->setPos(0,0);

    m_layout->addItem(m_header);

    /*
    GanttGraphicsHeaderRectItem * gRect = new GanttGraphicsHeaderRectItem(QDateTime::currentDateTime(), GanttGraphicsHeaderRectItem::Lower, ScaleDay, 100,0,100,15);
    m_layout->addItem(gRect);
    */

    setItems(model);

    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(5);
    test->setLayout(m_layout);
    this->addItem(test);

    qDebug()<< "count:"<<m_layout->count();
}

GanttGraphicsScene::~GanttGraphicsScene()
{
    delete m_header;
    delete m_model;
    delete m_layout;
    delete m_item;
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
   for (qreal y = start - step; y < rect.bottom(); ) {
      y += step;
      painter->drawLine(rect.left(), y, rect.right(), y);
   }
   // now draw vertical grid
   int stepV = 20;
   start = round(rect.left(), stepV);
   if (start > rect.left()) {
      start -= stepV;
   }
   for (qreal x = start - stepV; x < rect.right(); ) {
      x += stepV;
      painter->drawLine(x, rect.top(), x, rect.bottom());
   }

}

void GanttGraphicsScene::setInterval(QDateTime begin, QDateTime end)
{
    m_begin = begin;
    m_end = end;
}


void GanttGraphicsScene::setItems(GanttModel *model)
{

    //m_index = new QModelIndex(1,5);
    //m_item = model->data(m_index,Qt::UserRole);
    //m_layout->addItem(m_item);
    GanttItem * newItem;
    Scale graphicsScale = ScaleDay;
    foreach (newItem, model->itemlist()) {
        m_item = new GanttGraphicsItem(newItem, graphicsScale, m_begin, m_end);
        //m_item->
        m_layout->addItem(m_item);
    }
}
