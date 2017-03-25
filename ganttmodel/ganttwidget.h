/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление GanttWidget class.
 * \~englist
 * \brief File contains definition of GanttWidget class.
 */
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

/*!
 * \~russian
 * \brief Класс GanttWidget class предоставляет функции для работы с диаграммой Ганта.
 * \~english
 * \brief The GanttWidget class сlass let the functionality to work with Gantt chart.
 */
class GANTTMODELSHARED_EXPORT GanttWidget : public QWidget
{
    Q_OBJECT
    void init();
public:
    explicit GanttWidget(QWidget *parent = 0);
    ~GanttWidget();

    void setCurrentDt(const UtcDateTime &dt);
    void setPlayerSpeeds(qreal minMultiply, qreal maxMultiply);///< Устанавливает минимальную и максимальную скорости воспроизведения
    /// \return Плеер отображен?
    bool player() const;
    /// Устанавливает AbstractGanttFactory class
    void setFactory(AbstractGanttFactory *factory);
    /// Устанавливает QTreeView class связанную с диаграммой Ганта
    void setView(QTreeView *view, bool inner = false);

    QAbstractItemModel *model() const;

    /// \return UtcDateTime class время для QPointF class точки в QWidget class координатах
    UtcDateTime dtForPos(const QPoint &pos) const;

    /// \return QModelIndex class индекс элемента текущей модели(GanttWidget::model) для QPointF class точки в QWidget class координатах
    QModelIndex indexForPos(const QPoint &pos) const;

    QPoint mapPosToGlobal(const QPoint &pos) const;


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
