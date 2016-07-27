
#include "kamemory.h"
#include "mline_scene.h"

#include <QDebug>
#include <QCursor>
#include "QGraphicsTextItem"

KaMemory MLineScene::memory() const
{
    return _memory;
}

void MLineScene::init(const QList<KaMemoryPart> &records, long memorySize)
{
    KaMemory kaMemory;
    kaMemory.init(records,memorySize);
    setMemory(kaMemory);
}

void MLineScene::setMemory(const KaMemory &memory)
{
    _memory = memory;

    for(int i=0; i<_memory.memoryParts().count(); i++)
    {
        MLineGraphicsPart * part = new MLineGraphicsPart();
        addItem(part);
        part->setVisible(true);
        _memoryParts.append(part);

        part->setKaMemoryPart(_memory.memoryParts()[i]);

        QGraphicsItemGroup * group = new QGraphicsItemGroup();
        _memoryGroups.append(group);
        addItem(group);
        {
            MLineGraphicsPart * part = new MLineGraphicsPart(group);
            group->addToGroup(part);
            part->setStatus(_memory.memoryParts()[i].state());
            part->setBegin(_memory.memoryParts()[i].start());
            part->setEnd(_memory.memoryParts()[i].finish());

        }
        group->setVisible(true);
    }
}


MLineScene::MLineScene(QObject *parent):
    QGraphicsScene(parent)
{
    m_picker = new MLinePicker();
    m_picker->setVisible(true);
    addItem(m_picker);

    _memSizeText = new QGraphicsTextItem();

    addItem(_memSizeText);

    createLegend(10, 70);
}

MLineScene::~MLineScene()
{

}

void MLineScene::drawBackground(QPainter *painter, const QRectF &rect)
{
//    painter->setPen(QColor(Qt::black));
//    qreal metrY = 40;
//    painter->drawLine(rect.left()+dx_border, sceneRect().top()+metrY, rect.left()+dx_border, sceneRect().top()+metrY+10);
//    painter->drawLine(rect.left()+dx_border, sceneRect().top()+metrY+10, rect.right()-dx_border, sceneRect().top()+metrY+10);
//    painter->drawLine(rect.right()-dx_border, sceneRect().top()+metrY+10, rect.right()-dx_border, sceneRect().top()+metrY);
//    QRectF textLeft(rect.left(), sceneRect().top()+metrY+10, 20, 15);
//    painter->drawText(textLeft, Qt::AlignCenter, "0");
//    QRectF textRight(rect.right()-20, sceneRect().top()+metrY+10, 20, 15);

//    {
//        painter->drawText(textRight, Qt::AlignCenter,QString::number(_memory.memorySize()));
//        painter->setPen(Qt::DashLine);
//        {
//            painter->drawLine(rect.left()/*+i*inputChannelSize()*(width()-2*dx_border)/_memory.memorySize()*/+dx_border,
//                              sceneRect().top()+metrY+20,
//                              rect.left()/*+i*inputChannelSize()*(width()-2*dx_border)/_memory.memorySize()*/+dx_border,
//                              sceneRect().top()+metrY-5);
//        }
//    }
}

void MLineScene::createLegend(qreal posX, qreal posY)
{
    qreal row0 = posY;
    qreal row1 = row0 + 20;
    //qreal row2 = row1 + 20;

    qreal col0 = posX;
    qreal col1 = col0 + 20;
    qreal col2 = col1 + 80;
    qreal col3 = col2 + 20;
    qreal col4 = col3 + 120;
    qreal col5 = col4 + 20;
    qreal col6 = col5 + 120;
    qreal col7 = col6 + 20;


    QGraphicsTextItem * empText = new QGraphicsTextItem(tr("Пусто"));
    QGraphicsTextItem * freText = new QGraphicsTextItem(tr("Свободно"));
    QGraphicsTextItem * recText = new QGraphicsTextItem(tr("Записано"));
    QGraphicsTextItem * penWrText = new QGraphicsTextItem(tr("Ожидание съёмки"));
    QGraphicsTextItem * penReText = new QGraphicsTextItem(tr("Ожидание сброса"));
    QGraphicsTextItem * errWrText = new QGraphicsTextItem(tr("Ошибка записи"));
    QGraphicsTextItem * errReText = new QGraphicsTextItem(tr("Ошибка сброса"));
    QGraphicsTextItem * inputChanelsText = new QGraphicsTextItem();
    QGraphicsTextItem * memSizeText = new QGraphicsTextItem();

    empText->setPos(col1, row0);
    freText->setPos(col1, row1);

    penWrText->setPos(col3, row0);
    penReText->setPos(col3, row1);

    errWrText->setPos(col5, row0);
    errReText->setPos(col5, row1);

    recText->setPos(col7, row0);



    QGraphicsRectItem * empRect = new QGraphicsRectItem(col0,row0,15,15);
    QGraphicsRectItem * freRect = new QGraphicsRectItem(col0,row1,15,15);

    QGraphicsRectItem * penWrRect = new QGraphicsRectItem(col2,row0,15,15);
    QGraphicsRectItem * penReRect = new QGraphicsRectItem(col2,row1,15,15);

    QGraphicsRectItem * errWrRect = new QGraphicsRectItem(col4,row0,15,15);
    QGraphicsRectItem * errReRect = new QGraphicsRectItem(col4,row1,15,15);

    QGraphicsRectItem * recRect = new QGraphicsRectItem(col6,row0,15,15);

    empRect->setBrush(QBrush(QColor(Qt::white)));
    freRect->setBrush(QBrush(QColor(Qt::gray)));
    recRect->setBrush(QBrush(QColor(Qt::green)));
    penWrRect->setBrush(QBrush(QColor(255, 165, 0)));
    penReRect->setBrush(QBrush(QColor(Qt::yellow)));
    errWrRect->setBrush(QBrush(QColor(Qt::magenta)));
    errReRect->setBrush(QBrush(QColor(Qt::red)));

    addItem(empRect);
    addItem(freRect);
    addItem(recRect);
    addItem(penWrRect);
    addItem(penReRect);
    addItem(errWrRect);
    addItem(errReRect);

    addItem(empText);
    addItem(freText);
    addItem(recText);
    addItem(penWrText);
    addItem(penReText);
    addItem(errWrText);
    addItem(errReText);

}

void MLineScene::createStatistic(qreal posX, qreal posY)
{
//    if(!isInputChannelMode())
//    {
//        _inputChanelsText->setPlainText(tr("все %1 каналов").arg(
//                                      QString::number(_memory.inputChannelCount())));
//        _memSizeText->setPlainText(tr("объем памяти %1 Мб по %2 Мб на канал").arg(
//                                      QString::number(_memory.memorySize()),
//                                      QString::number(inputChannelSize())));
//    }
//    else
    {
//        _inputChanelsText->setPlainText(tr("один канал из %1").arg(
//                                      QString::number(_memory.inputChannelCount())));
        _memSizeText->setPlainText(tr("объем памяти %1 Мб").arg(
                                      QString::number(_memory.memorySize())));
    }
    _memSizeText->setPos(posX, posY+20);
}

MLinePicker *MLineScene::picker() const
{
    return m_picker;
}


MLineGraphicsPart *MLineScene::selectedKaMemoryPart()
{
    QGraphicsItem * selItem = selectedItems().first();
    for(int i=0; i<_memoryParts.count(); i++)
    {
        if(_memoryParts[i]== selItem)
            return _memoryParts[i];
    }
    return 0;
}



