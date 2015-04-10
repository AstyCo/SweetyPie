#include "ganttgraphicsheader.h"
#include "ganttgraphicsscene.h"
#include <QDebug>

#include <qmath.h>
//#include <cmath>
//==================konus==================
#include <QBrush>
#include <QPen>

GanttGraphicsHeader::GanttGraphicsHeader(QDateTime begin, QDateTime end, Scale scale, QGraphicsWidget/*QObject*/ *parent) :
    QGraphicsWidget(parent)
{
    setWindowFlags(Qt::Widget);
    //setVisible(false);

    QGraphicsLinearLayout *headerLayout = new QGraphicsLinearLayout(Qt::Vertical);
    /*QGraphicsLinearLayout **/upper = new QGraphicsLinearLayout(headerLayout);
    /*QGraphicsLinearLayout **/lower = new QGraphicsLinearLayout(headerLayout);

    m_scale = scale;
    setBegin(begin);
    setEnd(end);
    m_lowWidth = 20; //TODO //почему при ScaleMinute и lowWidth>15 происходит наложение блоков

    //qreal m_width = m_lowWidth * lowItemsCount(m_begin, m_end, m_scale);

    m_backgroundRect.setRect(0,0,200,200);

    lowHeight = 20;
    lowCount = lowItemsCount(m_begin, m_end, m_scale);
    m_fullHeaderRect.setRect(0,0,m_lowWidth*lowCount,lowHeight*2);

    createHeader();

    upper->setSpacing(0);
    upper->addStretch(1);

    lower->setSpacing(0);
    lower->addStretch(1);

    headerLayout->addItem(upper);
    headerLayout->addItem(lower);


    headerLayout->setSpacing(0);
    headerLayout->setContentsMargins(0,0,0,0);
    setLayout(headerLayout);


}

GanttGraphicsHeader::~GanttGraphicsHeader()
{
    delete lower;
    delete upper;
    delete m_rectItem;
}

QDateTime GanttGraphicsHeader::increaseDateTime(QDateTime dateTime, Scale scale, int increment)
{
    switch (scale) {
    case ScaleSecond:
        dateTime = dateTime.addSecs(increment);
        break;
    case ScaleMinute:
        dateTime = dateTime.addSecs(increment*60);
        break;
    case ScaleHour:
        dateTime = dateTime.addSecs(increment*60*60);
        break;
    case ScaleDay:
        dateTime = dateTime.addDays(increment);
        break;
    case ScaleMonth:
        dateTime = dateTime.addMonths(increment);
        break;
    default:
        break;
    }
    return dateTime;
}

int GanttGraphicsHeader::lowItemsCount(QDateTime begin, QDateTime end, Scale scale)
{
    int count = 0;
    switch (scale) {
    case ScaleSecond:
        count = begin.secsTo(end);
        break;
    case ScaleMinute:
        count = begin.secsTo(end)/60;
        break;
    case ScaleHour:
        count = begin.secsTo(end)/(60*60);
        break;
    case ScaleDay:
        count = begin.daysTo(end);
        break;
    case ScaleMonth:
        {
        //TODO
        //считает неправильно

        int a = begin.toString("MM").toInt();
        int b = end.toString("MM").toInt();
        count = b - a;
//        if (count == 0)
//        {
//            count = 1;
//            setEnd(end);
//        }
        count++;
        break;
        }
    default:
        break;
    }
    return count;
}

QString GanttGraphicsHeader::formatOverDateTime(Scale scale)
{
    QString format;
    switch (scale) {
    case ScaleSecond:
        format = "mm";
        break;
    case ScaleMinute:
        format = "hh";
        break;
    case ScaleHour:
        format = "dd";
        break;
    case ScaleDay:
        format = "MM";
        break;
    case ScaleMonth:
        format = "yyyy";
        break;
    default:
        format = "yyyy";
        break;
    }
    return format;
}

QDateTime GanttGraphicsHeader::pix2dt(qreal pix)
{
    QDateTime dt;
    int counter = 0;
    int remain;
//    switch (m_scale) {
//    case ScaleSecond:
//        counter = qCeil(pix/lowWidth());
//        remain = fmod(pix,lowWidth());
//        dt = increaseDateTime(m_begin,m_scale, counter);
//        break;
//    case ScaleMinute:
//        counter = qCeil(pix/lowWidth());
//        //remain = fmod(pix,lowWidth());
//        break;
//    case ScaleHour:
//        counter = qCeil(pix/lowWidth());
//        //remain = fmod(pix,lowWidth());
//        break;
//    case ScaleDay:
//        counter = qCeil(pix/lowWidth());
//        //remain = fmod(pix,lowWidth());
//        break;
//    case ScaleMonth:
//        counter = qCeil(pix/lowWidth());
//        //remain = fmod(pix,lowWidth());
//        break;
//    default:
//        break;
//    }

    counter = qCeil(pix/lowWidth());
    dt = increaseDateTime(m_begin,m_scale, counter);

    return dt;
}

qreal GanttGraphicsHeader::dt2pix(QDateTime dt)
{
    qreal pix = 0;
    int counter = 0;
    switch (m_scale) {
    case ScaleSecond:
        counter = m_begin.secsTo(dt);
        break;
    case ScaleMinute:
        counter = m_begin.secsTo(dt)/60;
        break;
    case ScaleHour:
        counter = m_begin.secsTo(dt)/3600;
        break;
    default:
        break;
    }
    pix = counter * m_lowWidth;
    return pix;
}

qreal GanttGraphicsHeader::lowWidth() const
{
    return m_lowWidth;
}

void GanttGraphicsHeader::setLowWidth(const qreal &lowWidth)
{
    m_lowWidth = lowWidth;
}

Scale GanttGraphicsHeader::zoom() const
{
    return m_scale;
}

void GanttGraphicsHeader::setZoom(const Scale &scale)
{
    m_scale = scale;
    emit zoomChanged(m_scale);
}


void GanttGraphicsHeader::createHeader()
{
    clearHeader();
    


    if(m_scale == ScaleMonth)
        setBegin(m_begin);

    lowCount = lowItemsCount(m_begin, m_end, m_scale);


    QDateTime actualBegin;
    QDateTime actualEnd;
    m_fullHeaderRect.setRect(m_fullHeaderRect.x(),0,m_lowWidth*lowCount,lowHeight*2);
    int countTillBeginAHR = qFloor((m_backgroundRect.x()-m_fullHeaderRect.x())/lowWidth());
    m_actualHeaderRect.setX(m_fullHeaderRect.x() + lowWidth()*countTillBeginAHR);
    m_actualHeaderRect.setY(0);
    m_actualHeaderRect.setHeight(lowHeight*2);
    actualBegin = increaseDateTime(m_begin, m_scale, countTillBeginAHR);
    int countTillEndAHR = qCeil((m_backgroundRect.x()+m_backgroundRect.width()-m_fullHeaderRect.x())/lowWidth());
    m_actualHeaderRect.setWidth(m_fullHeaderRect.x()+lowWidth()*countTillEndAHR - m_actualHeaderRect.x());
    actualEnd = increaseDateTime(m_begin, m_scale, countTillEndAHR);
    int actualLowCount = countTillEndAHR - countTillBeginAHR;
//    qDebug()<<"BGR"<<m_backgroundRect
//    <<"FHR"<<m_fullHeaderRect
//    <<"AHR"<<m_actualHeaderRect;



    int lowPerUpCount = 0;
    QDateTime currentDT = actualBegin/*m_begin*/;
    QDateTime bufferDT;
    qreal firstLowItemPerUp = m_actualHeaderRect.x();
    for(int i = 0; i < actualLowCount/*lowCount*/; i++)
    {
        m_rectItem = new GanttGraphicsHeaderRectItem(currentDT, GanttGraphicsHeaderRectItem::Lower, m_scale,
                                                     m_actualHeaderRect.x()+lowWidth()*i/*0*/, 20, m_lowWidth, lowHeight);
        lower->addItem(m_rectItem);
        lowPerUpCount++;
        bufferDT = increaseDateTime(currentDT, m_scale, 1);
        int a = currentDT.toString(formatOverDateTime(m_scale)).toInt();
        int b = bufferDT.toString(formatOverDateTime(m_scale)).toInt();
        if ((b!=a) || (i == actualLowCount/*lowCount*/-1))
        {
            m_rectItem = new GanttGraphicsHeaderRectItem(currentDT, GanttGraphicsHeaderRectItem::Upper, m_scale,
                                                         firstLowItemPerUp/*0*/, 0, m_lowWidth * lowPerUpCount, lowHeight);
            upper->addItem(m_rectItem);
            firstLowItemPerUp = firstLowItemPerUp + lowWidth()*lowPerUpCount;
            lowPerUpCount = 0;

        }
        currentDT = bufferDT;
    }
    //update();
}

void GanttGraphicsHeader::clearHeader()
{
    for(int i = lower->count()-1; i >= 0; --i)
    {
        delete lower->itemAt(i);
    }
    for(int j = upper->count()-1; j >= 0; --j)
    {
        delete upper->itemAt(j);
    }
}


QDateTime GanttGraphicsHeader::end() const
{
    return m_end;
}

void GanttGraphicsHeader::setEnd(const QDateTime &end)
{
    m_end = increaseDateTime(end,m_scale,1);/*end.addDays(2);*/

}

QDateTime GanttGraphicsHeader::begin() const
{
    return m_begin;
}

void GanttGraphicsHeader::setBegin(const QDateTime &begin)
{
    m_begin = begin;
    QTime time;
    QDate date;
    switch (m_scale) {
    case ScaleSecond:
        break;
    case ScaleMinute:
        time.setHMS(begin.time().hour(),begin.time().minute(),0);
        m_begin.setTime(time);
        break;
    case ScaleHour:
        time.setHMS(begin.time().hour(),0,0);
        m_begin.setTime(time);
        break;
    case ScaleDay:
        time.setHMS(0,0,0);
        m_begin.setTime(time);
        break;
    case ScaleMonth:
        date.setDate(begin.date().year(),begin.date().month(),1);
        time.setHMS(0,0,0);
        m_begin.setDate(date);
        m_begin.setTime(time);
        break;
    default:
        break;
    }
}



