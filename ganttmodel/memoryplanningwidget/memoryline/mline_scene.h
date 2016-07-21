#ifndef MLINE_SCENE_H
#define MLINE_SCENE_H


#include <ganttmodel_global.h>


#include "kamemory.h"
#include "QList"
#include <QGraphicsScene>
#include <QPainter>
#include "mline_picker.h"
#include "QGraphicsItemGroup"
#include "mline_graphicspart.h"

#define dx_border 10


class GANTTMODELSHARED_EXPORT MLineScene: public QGraphicsScene
{
    Q_OBJECT
private:
    KaMemory _memory;
    bool m_isPickerEnabled;

    MLinePicker * m_picker;
    QList<MLineGraphicsPart *> _memoryParts;
    QList<QGraphicsItemGroup *> _memoryGroups;

    QGraphicsTextItem * _memSizeText;

public:
    MLineScene(QObject * parent = 0);
    ~MLineScene();
    void drawBackground(QPainter *painter, const QRectF &rect);

    void createLegend(qreal posX, qreal posY);
    void createStatistic(qreal posX, qreal posY);

    MLinePicker *picker() const;

    MLineGraphicsPart * selectedKaMemoryPart();

    KaMemory memory() const;

    void init(const QList<KaMemoryPart>& list, long memorySize);

    void setMemory(const KaMemory &memory);
};

#endif // MLINE_SCENE_H