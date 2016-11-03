#ifndef DTINTERVALWIDGET_H
#define DTINTERVALWIDGET_H

#include "intervalslider.h"

#include <QWidget>

#include <QMenu>
#include <QDateTime>
#include "ganttproject_global.h"



namespace Ui {
class DtIntervalWidget;
}

class GANTTMODELSHARED_EXPORT DtIntervalWidget : public QWidget
{
    Q_OBJECT

public:
    enum DtIntervalWidgetEnum
    {
        ToolButtonToDt = 0x1,             ///< Включает отображение витков
        DateTime = 0x2,           ///< Включает отображение даты и времени
        IntervalSlider = 0x4,   ///< Включает отображение слайдера для временных интервалов
        AllFeatures = ToolButtonToDt|DateTime|IntervalSlider,


        DtIntervalWidgetEnumMax = IntervalSlider,      // равен наибольшему флагу
        DtIntervalWidgetEnumCount = 3       // Число элементов enum DateRoundFlags
    };

    explicit DtIntervalWidget(QWidget *parent = 0);
    ~DtIntervalWidget();


    virtual void hideAll();
    virtual void setFlags(int);

    QDateTime minimumDt() const{
        return minimumDtBegin();
    }
    QDateTime maximumDt() const{
    return maximumDtBegin();
    }

    QDateTime beginDt() const;
    QDateTime endDt() const;
    long beginHandle() const;
    long endHandle() const;

    static long long dtToSliderValue(const QDateTime &dt);

public slots:

    void setBeginDt(const QDateTime &dt);
    void setEndDt(const QDateTime &dt);

    void setLimits(const QDateTime &minDt, const QDateTime& maxDt);

    virtual void matchBegins(const QDateTime &dt);
    virtual void matchEnds(const QDateTime &dt);
    virtual void blockAllSignals(bool val);
private slots:
    void showDateTimeWidgets(bool show);
    void showIntervalSlider(bool show);
    void showToolButtonToDt(bool show);

    void onBeginHandleMoved(long long val);
    void onEndHandleMoved(long long val);
    void updateTimespanLabel();

    void setBeginHandle(long val);
    void setEndHandle(long val);
    void testDateTimeBegin();
    void testDateTimeEnd();



    void setCurrentDt();
    void setYearIntervalFuture();
    void setMounthIntervalFuture();
    void setWeekIntervalFuture();
    void set3DaysIntervalFuture();

    void setMinimum(const QDateTime &minDt);
    void setMaximum(const QDateTime &maxDt);

protected:
    virtual void createMenuSetDt();


private:
    QDateTime minimumDtBegin() const;
    QDateTime minimumDtEnd() const;
    QDateTime maximumDtBegin() const;
    QDateTime maximumDtEnd() const;

signals:
    void beginDtChanged(QDateTime);
    void endDtChanged(QDateTime);
    void dtChanged();

protected:
    QMenu * _dtMenu;
    int m_flags;
private:
    Ui::DtIntervalWidget *ui;

};

#endif // DTINTERVALWIDGET_H
