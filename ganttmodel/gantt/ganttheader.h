#ifndef GANTTHEADER_H
#define GANTTHEADER_H

#include "ganttglobalvalues.h"
#include "qgraphicsrectitemwithtext.h"

#include "utcdatetime.h"

#include <QDateTime>

#include <QList>

class GanttScene;
class GanttInfoItem;
class GanttInfoLeaf;
class GanttInfoNode;
class GanttWidget;

class GanttHeader : public QGraphicsItem
{
public:
    enum GanttHeaderMode
    {
        GanttDiagramMode,
        TimelineMode,

        GanttHeaderMode_count
    };

    enum GanttPrecisionMode
    {
        years1,
        months1,
        days1,

        hours12,
        hours6,
        hours1,

        minutes30,
        minutes15,
        minutes5,
        minutes1,

        seconds30,
        seconds15,
        seconds5,
        seconds1,

//        miliseconds30,
//        miliseconds15,
//        miliseconds1,

//        microseconds30,
//        microseconds15,
//        microseconds1,

        GanttPrecisionMode_count
    };


public:
    GanttHeader(QGraphicsItem *parent = 0);

    void setScene(QGraphicsScene* scene);

    QRectF boundingRect() const;

    void updateHeader();

    const GanttPrecisionMode &mode() const;
    bool setMode(const GanttPrecisionMode &mode);

    void onItemsAddition(const QList<GanttInfoItem*>& items);
    void onItemsAddition(GanttInfoItem* items);

    static QString topHeaderFormat(GanttPrecisionMode mode);
    static QString secondHeaderFormat(GanttPrecisionMode mode);

    static UtcDateTime nextTopHeaderDt(const UtcDateTime& curDt,GanttHeader::GanttPrecisionMode mode);

    static UtcDateTime nextSecondHeaderDt(const UtcDateTime& curDt,GanttHeader::GanttPrecisionMode mode);

    qreal secondHeaderWidth() const;

    void clear();

    qreal dtToX(const UtcDateTime& dt) const;
    UtcDateTime xToDt(qreal x) const;

    bool setHeaderMode(const GanttHeaderMode &headerMode);
    void init();
    GanttHeaderMode headerMode() const;

    void setCurrentWidth(int currentWidth);

    void setLengthInMicroseconds(long long lengthInMicroseconds);

    const UtcDateTime &startDt() const;
    const UtcDateTime &finishDt() const;
    const UtcDateTime &minDt() const;
    const UtcDateTime &maxDt() const;

    void setRange(UtcDateTime min, UtcDateTime max);

    bool verifyBoundsByLeaf(const GanttInfoLeaf* leaf);
    bool verifyBoundsByNode(const GanttInfoNode* node);


    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    GanttPrecisionMode calculateTimeMode(const UtcDateTime &min, const UtcDateTime &max);
    void initRange();
    UtcDateTime startByDt(const UtcDateTime& dt,GanttPrecisionMode mode) const;
    UtcDateTime startByDt(const UtcDateTime& dt) const;

    UtcDateTime finishByDt(const UtcDateTime& dt,GanttPrecisionMode mode) const;
    UtcDateTime finishByDt(const UtcDateTime& dt) const;

    UtcDateTime nextStart(const UtcDateTime& start,GanttPrecisionMode mode) const;
    UtcDateTime nextStart(const UtcDateTime& start) const;

    UtcDateTime prevFinish(const UtcDateTime& finish,GanttPrecisionMode mode) const;
    UtcDateTime prevFinish(const UtcDateTime& finish) const;


    static long long modeToMicrosecond(GanttPrecisionMode mode, const QDate& date = QDate());
    static long long modeToSecond(GanttPrecisionMode mode, const QDate& date = QDate());
    static int modeToSegmentCount(GanttPrecisionMode mode, const QDate& date = QDate());
    static bool isDrawn(const UtcDateTime& dt, GanttPrecisionMode mode);
    static QString formatForMode(GanttPrecisionMode mode);


    void setVisItemCount(int visItemCount);


    void setEmpty(bool isEmpty);

private:

    bool onItemsAdditionHelper(GanttInfoItem* item);
    QString textForDtStep(int step_segment) const;
    void updateWidget();

//    void updateVisItemCount(qreal item_width);
    qreal calculateVisItemCount(qreal item_width) const;
    qreal getCoef(const UtcDateTime &min, const UtcDateTime &max) const;
    void updateStretchFactor();


private:

    GanttHeaderMode m_headerMode;
    // Gantt mode
    UtcDateTime   m_startDt,
                  m_finishDt;
    QList<QGraphicsRectItemWithText*> m_items;
    // line mode
    qreal m_stretchFactor;
    int m_currentWidth;

    // COMMON

    UtcDateTime   m_minDt,
                  m_maxDt;

    long long m_lengthInMicroseconds;
    int m_visItemCount;

    GanttPrecisionMode m_mode;

    GanttScene * m_scene;
    GanttWidget *m_widget;

    bool m_isEmpty;

    friend class GanttWidget;
    friend class GanttScene;

};

#endif // GANTTHEADER_H
