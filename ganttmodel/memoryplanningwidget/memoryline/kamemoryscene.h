#ifndef KAMEMORYSCENE_H
#define KAMEMORYSCENE_H


#include <ganttmodel_global.h>


#include "kamemory.h"
#include "QList"
#include <QGraphicsScene>
#include <QPainter>
#include "kamemorypicker.h"
#include "QGraphicsItemGroup"
#include "kamemorygraphicspart.h"

#define dx_border 10


class GANTTMODELSHARED_EXPORT KaMemoryScene: public QGraphicsScene
{
    Q_OBJECT
private:
    KaMemory _memory;
    bool m_isPickerEnabled;

    KaMemoryPicker * m_picker;
    QList<KaMemoryGraphicsPart *> _memoryParts;
    QList<QGraphicsItemGroup *> _memoryGroups;

    QGraphicsTextItem * _memSizeText;

public:
    KaMemoryScene(QObject * parent = 0);
    ~KaMemoryScene();
    void drawBackground(QPainter *painter, const QRectF &rect);

    void createLegend(qreal posX, qreal posY);
    void createStatistic(qreal posX, qreal posY);

    KaMemoryPicker *picker() const;

    KaMemoryGraphicsPart * selectedKaMemoryPart();

    KaMemory memory() const;

    void init(const QList<KaMemoryPart>& list, long memorySize);

    void setMemory(const KaMemory &memory);
};

#endif // KAMEMORYSCENE_H
