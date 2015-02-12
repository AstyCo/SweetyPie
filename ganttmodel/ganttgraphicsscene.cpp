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
    //m_layout->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //test->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //m_layout->setSpacing(0);


    QDateTime begin = QDateTime::fromString("29.01.2015", "dd.MM.yyyy");
    QDateTime end = begin.addMonths(2);
    m_begin = begin;
    m_end = end;
    m_scale = ScaleDay;
    m_header = new GanttGraphicsHeader(begin, end, m_scale);
    m_header->setPos(0,0);
    m_header->setZValue(1.0);

    m_layout->addItem(m_header);

    m_model = model;


    setItems(m_model);

    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(5);

    test->setLayout(m_layout);
    this->addItem(test);

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

void GanttGraphicsScene::updateItems(QModelIndex index)
{

//    for(int i=1; i<=m_layout->count()-1; i++)
//    {
//        delete m_layout->itemAt(index.row());
//        //m_layout->removeAt(i);
//    }

    m_layout->addStretch(1);
    qDebug()<<m_layout->itemAt(index.row()+1)->contentsRect();

    delete m_layout->itemAt(index.row()+1);
    //qDebug()<<m_layout->itemAt(index.row()+1)->contentsRect();

    //setItems(m_model);

}


void GanttGraphicsScene::setItems(GanttModel *model)
{
    GanttItem * newItem;
    foreach (newItem, model->itemlist()) {
        m_item = new GanttGraphicsItem(newItem, m_scale, m_begin, m_end);
        m_layout->addItem(m_item);
    }
}
