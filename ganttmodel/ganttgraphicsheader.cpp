#include "ganttgraphicsheader.h"


//==================konus==================
#include <QBrush>
#include <QPen>

GanttGraphicsHeader::GanttGraphicsHeader(QDateTime begin, QDateTime end, Scale scale, QGraphicsWidget *parent) :
    QGraphicsWidget(parent)
{
    setWindowFlags(Qt::Widget);
    //setVisible(false);

    QGraphicsLinearLayout *headerLayout = new QGraphicsLinearLayout(Qt::Vertical);
    QGraphicsLinearLayout *upper = new QGraphicsLinearLayout(headerLayout);
    QGraphicsLinearLayout *lower = new QGraphicsLinearLayout(headerLayout);



    m_begin = begin;
    m_end = end;

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
    delete m_lower;
    delete m_upper;
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
QDateTime GanttGraphicsHeader::end() const
{
    return m_end;
}

void GanttGraphicsHeader::setEnd(const QDateTime &end)
{
    m_end = end;
}

QDateTime GanttGraphicsHeader::begin() const
{
    return m_begin;
}

void GanttGraphicsHeader::setBegin(const QDateTime &begin)
{
    m_begin = begin;
}



