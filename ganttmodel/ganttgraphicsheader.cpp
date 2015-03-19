#include "ganttgraphicsheader.h"
#include <QDebug>

//==================konus==================
#include <QBrush>
#include <QPen>

GanttGraphicsHeader::GanttGraphicsHeader(QDateTime begin, QDateTime end, Scale scale, QGraphicsWidget *parent) :
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


    createHeader();

/*
    int lowHeight = 20;
    int lowWidth = 20; //TODO //почему при ScaleMinute и lowWidth>15 происходит наложение блоков
    int lowCount = lowItemsCount(begin, end, scale);

//    QRectF lowRect(0,0,lowWidth*lowCount, lowHeight);
//    lower->setGeometry(lowRect);
//    upper->setGeometry(lowRect);

    //int upCount = 0;
    int lowPerUpCount = 0;
    QDateTime currentDT = begin;
    QDateTime bufferDT;
    for(int i = 0; i < lowCount; i++)
    {
        m_rectItem = new GanttGraphicsHeaderRectItem(currentDT, GanttGraphicsHeaderRectItem::Lower, scale,
                                                     0, 0, lowWidth, lowHeight);
        lower->addItem(m_rectItem);
        lowPerUpCount++;
        bufferDT = increaseDateTime(currentDT, scale, 1);
        int a = currentDT.toString(formatOverDateTime(scale)).toInt();
        int b = bufferDT.toString(formatOverDateTime(scale)).toInt();
        if ((b!=a) || (i == lowCount-1))
        {
            m_rectItem = new GanttGraphicsHeaderRectItem(currentDT, GanttGraphicsHeaderRectItem::Upper, scale,
                                                         0, 0, lowWidth * lowPerUpCount, lowHeight);
            upper->addItem(m_rectItem);
            lowPerUpCount = 0;
        }
        currentDT = bufferDT;
    }
*/
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
        if (count == 0)
            count = 1;
        break;
        }
    default:
        break;
    }
    //count++;
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

void GanttGraphicsHeader::createHeader()
{
    clearHeader();

    int lowHeight = 20;
    int lowWidth = 20; //TODO //почему при ScaleMinute и lowWidth>15 происходит наложение блоков
    int lowCount = lowItemsCount(m_begin, m_end, m_scale);


    //int upCount = 0;
    int lowPerUpCount = 0;
    QDateTime currentDT = m_begin;
    QDateTime bufferDT;
    for(int i = 0; i < lowCount; i++)
    {
        m_rectItem = new GanttGraphicsHeaderRectItem(currentDT, GanttGraphicsHeaderRectItem::Lower, m_scale,
                                                     0, 0, lowWidth, lowHeight);
        lower->addItem(m_rectItem);
        lowPerUpCount++;
        bufferDT = increaseDateTime(currentDT, m_scale, 1);
        int a = currentDT.toString(formatOverDateTime(m_scale)).toInt();
        int b = bufferDT.toString(formatOverDateTime(m_scale)).toInt();
        if ((b!=a) || (i == lowCount-1))
        {
            m_rectItem = new GanttGraphicsHeaderRectItem(currentDT, GanttGraphicsHeaderRectItem::Upper, m_scale,
                                                         0, 0, lowWidth * lowPerUpCount, lowHeight);
            upper->addItem(m_rectItem);
            lowPerUpCount = 0;
        }
        currentDT = bufferDT;
    }
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

void GanttGraphicsHeader::zoom(Scale scale)
{
    m_scale = scale;
    createHeader();
}

QDateTime GanttGraphicsHeader::end() const
{
    return m_end;
}

void GanttGraphicsHeader::setEnd(const QDateTime &end)
{
    m_end = end.addDays(2);
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
        qDebug()<<"wtf"<<m_begin;
        break;
    case ScaleMonth:
        date.setDate(begin.date().year(),begin.date().month(),1);
        m_begin.setDate(date);
        break;
    default:
        break;
    }
}



