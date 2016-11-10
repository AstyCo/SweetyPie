#ifndef MEMORYPLANNINGWIDGET_HPP
#define MEMORYPLANNINGWIDGET_HPP

#include "ganttproject_global.h"
#include "memoryview.h"
#include "memorygrid/mgrid_scene.h"

#include <QGraphicsScene>
#include <QWidget>
#include <QVector>


namespace Ui {
class MemoryPlanningWidget;
}

class KaMemory;
class MLineScene;




class GANTTMODELSHARED_EXPORT MemoryPlanningWidget : public QWidget
{
    Q_OBJECT

public:
    MemoryPlanningWidget(QWidget *parent = 0);
    ~MemoryPlanningWidget();


    void setMemory(QSharedPointer<KaMemory> kaMemory);

    MGridScene* gridScene() const;
    MLineScene* lineScene() const;
    bool setSelected(long start,long length);

    void setShowButtons(bool flag); ///< отображение кнопок взаимодействия с выбранной областью
    void setShowViews(bool flag); ///< отображение режимов (сеткой/линейная ...)
    void setSelectionMode(MGridScene::SelectionMode mode);

public slots:
    void changeScene();
    void clear();

    void setGridView();
    void setLineView();

    void setUnitInfo(const QString& text);
    void setItemInfo(const QString& text);
    void updateParts();

private slots:
    void onSelectionChanged();
    void on_pushButtonEmpty_clicked();

    void on_pushButtonFree_clicked();

    void on_pushButtonPendingWrite_clicked();

    void on_pushButtonPendingRead_clicked();

    void hideGridWidgets();
    void showGridWidgets();

signals:
    void selectionChanged(const QList<KaMemoryPart *>);


private:
    Ui::MemoryPlanningWidget *ui;
    MGridScene* m_gridScene;
    MLineScene* m_lineScene;

    MemoryView::MemoryViewMode m_mode;
    bool m_showButtons;

    QVector<QWidget*> m_gridWidgets;
};

#endif // MEMORYPLANNINGWIDGET_HPP
