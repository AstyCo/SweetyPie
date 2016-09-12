#include "ganttheaderview.h"
#include "ganttglobalvalues.h"

#include "ganttwidget.h"
#include "gantttreeview.h"

#include <QPaintEvent>
#include <QPainter>

#include <QFontMetrics>

#include <QDebug>


#define HEADERVIEW_MARGIN 5


GanttHeaderView::GanttHeaderView(Qt::Orientation orientation, QWidget * parent) :
    QHeaderView(orientation,parent)
{
    m_widget = NULL;
    QWidget *p_parent = parentWidget();
    while(!m_widget)
    {
        if(!p_parent)
            break;

        m_widget = dynamic_cast<GanttWidget*>(p_parent);
        p_parent = p_parent->parentWidget();
    }
}

void GanttHeaderView::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());

    QPen headerPen(QBrush(Qt::white)
                   ,1,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
    QFont dtFont("Goudy Old Style", 10,QFont::DemiBold),
            utcFont("Goudy Old Style", 12,QFont::DemiBold);

    QFontMetrics fmetrics(utcFont,painter.device());


    QRect utcRect = fmetrics.tightBoundingRect("UTC").adjusted(-3,-3,3,3);

    QRect dtRect = rect().adjusted(0,0, - utcRect.width() - HEADERVIEW_MARGIN , 0);

    utcRect.moveTop((dtRect.bottom() - utcRect.height())/2);
    utcRect.moveLeft(dtRect.right() - HEADERVIEW_MARGIN);

    painter.fillRect(rect(),QBrush(HEADER_COLOR));

    painter.setPen(headerPen);

    painter.setFont(dtFont);
    if(m_widget && m_widget->player())
    {
        painter.drawText(dtRect,m_widget->slidersDt().toString("dd.MM.yyyy HH:mm:ss")
                           ,QTextOption(Qt::AlignCenter));
    }


    painter.drawRect(utcRect);  
    painter.setFont(utcFont);
    painter.drawText(utcRect,"UTC",QTextOption(Qt::AlignCenter));

    e->accept();

}



