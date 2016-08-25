#ifndef GANTTWIDGET_H
#define GANTTWIDGET_H

#include "ganttproject_global.h"
#include "ganttinfoleaf.h"

#include <QModelIndex>

#include <QList>

#include <QWidget>


namespace Ui {
class GanttWidget;
}

class GanttScene;
class GanttTreeModel;
class GanttHeader;

class GANTTMODELSHARED_EXPORT GanttWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GanttWidget(QWidget *parent = 0);

    ~GanttWidget();


//    void addItems(const QString& title, const QList< std::tuple<QString,UtcDateTime,UtcDateTime> >& );
    void addItems(GanttInfoItem* item);
    void addItems(const QList<GanttInfoItem*>& items);

    UtcDateTime slidersDt() const;

    UtcDateTime minDt() const;
    const UtcDateTime &outerMinDt() const;

    UtcDateTime maxDt() const;
    const UtcDateTime &outerMaxt() const;

signals:
    void currentDtChanged(const UtcDateTime& dt);

private slots:
    void on_comboBox_mode_currentIndexChanged(int index);

    void expanded(const QModelIndex& index);
    void collapsed(const QModelIndex& index);

    void onSliderMoved();

    void on_pushButton_slider_clicked();
    void on_pushButton_header_clicked();

public slots:
//    void repaintDtHeader();
    void updateRange(const UtcDateTime& min, const UtcDateTime& max);
    void updateSliderLimits();
    // ZOOM +
    void newLimits();
    void newLimits(const UtcDateTime& min, const UtcDateTime& max);
    void prevLimits();
    // ZOOM -



private slots:
    void pushLimits();


private:
    void updatePos(GanttInfoNode* from);
    void updatePosHelper(GanttInfoItem* item);

private:
    Ui::GanttWidget *ui;

    GanttScene *m_scene;
    GanttTreeModel *m_model;

    qreal m_curSceneMax;

    UtcDateTime m_minDt, m_maxDt;

    QVector<std::pair<UtcDateTime,UtcDateTime> > m_stackLimits;

    friend class GanttScene;
    friend class MyGanttView;
    friend class GanttHeader;
};

#endif // GANTTWIDGET_H
