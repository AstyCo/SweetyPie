#ifndef GANTTWIDGET_H
#define GANTTWIDGET_H

#include "ganttproject_global.h"
#include "gantt/info/ganttinfotree.h"
#include "extensions-gui/interfaces/abstractganttfactory.h"

#include <QModelIndex>

#include <QList>
#include <QTreeView>
#include <QWidget>


namespace Ui {
class GanttWidget;
}

class GanttScene;


class GANTTMODELSHARED_EXPORT GanttWidget : public QWidget
{
    Q_OBJECT
    void init();
public:
    explicit GanttWidget(QWidget *parent = 0);
    ~GanttWidget();

    void setCurrentDt(const UtcDateTime &dt);
    void setPlayerSpeeds(qreal minMultiply, qreal maxMultiply);///< Устанавливает минимальную и максимальную скорости воспроизведения
    bool player() const;

    void setFactory(AbstractGanttFactory *factory);
    void setView(QTreeView *view, bool inner = false);

    QAbstractItemModel *model() const;

public slots:

    /// --- Спрятать/Показать элементы
    void showInterval();
    void hideInterval();
    void setVisibleInterval(bool value);

    void showDtLine();
    void hideDtLine();
    void setVisibleDtLine(bool value);

    void showPlayer();
    void hidePlayer();
    void setVisiblePlayer(bool show);
    // ---

    void dataReset();   ///< Обновляет содержимое диаграммы по модели, вызывать если не вызывалось QAbstractItemModel::reset()


private slots:
    void onGanttViewCustomContextMenuRequested(const QPoint&);
    void onGanttViewMaximumHeightChanged();

private:
    void connectPlayer();
    void connectSceneWithInfo();
    void connectIntervals();

private:
    Ui::GanttWidget *ui;
    GanttInfoTree *_treeInfo;
    GanttScene *_scene;
    AbstractGanttFactory *_factory;

    bool m_playerShown;
};

#endif // GANTTWIDGET_H
