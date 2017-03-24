/*!
 * \file
 * \~russian
 * \brief Файл содержит объявление HFitScene class.
 * \~englist
 * \brief File contains definition of HFitScene class.
 */
#ifndef HFITSCENE_H
#define HFITSCENE_H

#include <QGraphicsScene>
#include "ganttgraphicsview.h"

/*!
 * \~russian
 * \brief Класс HFitScene class представляет сцену с фиксированной шириной
 * \~english
 * \brief The HFitScene class is the QGraphicsScene class with fixed width
 */
class HFitScene : public QGraphicsScene
{
    Q_OBJECT

    void init();
public:
    HFitScene(GanttGraphicsView *view, QObject *parent = 0);
    virtual ~HFitScene(){}
    GanttGraphicsView *view() const{
        return _view;
    }

signals:
    void sceneRectAboutToBeChanged(const QRectF &newRect);

public slots:
    virtual void onViewResized(const QSize &newSize);

protected:
    void updateWidth(int w);
    void updateHeight(int h);

protected:
    GanttGraphicsView *_view;
};

#endif // HFITSCENE_H
