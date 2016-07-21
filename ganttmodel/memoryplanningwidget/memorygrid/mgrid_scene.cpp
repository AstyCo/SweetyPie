#include "mgrid_scene.h"
#include "memoryview.h"

#include "kamemory.h"
#include "mgrid_interactiveunit.h"

#include <QGraphicsSceneMouseEvent>

#include <QApplication>
#include <QDebug>

#include <QtCore/qmath.h>

#include <QMessageBox>

qreal qSqrt(qreal);




MGridScene::MGridScene( QObject * parent)
    :QGraphicsScene(parent)
{
    clearLastSelected();

    setItemEdge(DEFAULT_EDGELENGTH);
    setItemBorder(DEFAULT_BORDERWIDTH);

    m_items.clear();
    m_units.clear();
    m_lastSelected = NULL;
    m_interactiveUnit = NULL;
    m_highlightMode = true;
    m_interactiveHighlight = true;
    setLengthHighlight(100);


    m_memoryWidget = new MGridWidget;

//    m_memoryWidget->setPos(5,5);

    m_memoryWidget->setLabels(false);

    addItem(m_memoryWidget);

    m_memoryWidget->setSpacing(DEFAULT_SPACING);
    m_memoryWidget->setMargins(DEFAULT_MARGINS);
    m_memoryWidget->setItemPerRow(64);

}

MGridScene::~MGridScene()
{
    delete m_memoryWidget;
}

void MGridScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QList<QGraphicsItem*> itemsAtPos = items(event->scenePos());


    MGridtem* p_mem = NULL;

    foreach(QGraphicsItem* itemAtPos,itemsAtPos)
    {
        if((p_mem=dynamic_cast<MGridtem*>(itemAtPos)))
            break;
    }
    if(!p_mem)
    {
        return;
    }
    setLastSelected(p_mem);
    return QGraphicsScene::mousePressEvent(event);

}

void MGridScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_lastSelected
//       && (QApplication::keyboardModifiers()&Qt::ShiftModifier)
            )
    {

        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos());
        MGridtem * p_mem = NULL;

        foreach(QGraphicsItem* itemAtPos,itemsAtPos)
        {
            if((p_mem=dynamic_cast<MGridtem*>(itemAtPos)))
                break;
        }

        if(!p_mem)
            return;

        clearSelection();

        int newSelectedIndex = p_mem->index();

        QList<QGraphicsItem*> sceneItems = items();

        int min = qMin(m_lastSelectedIndex,newSelectedIndex),
            max = qMax(m_lastSelectedIndex,newSelectedIndex);

        for(int i = 0; i < sceneItems.size(); ++i)
        {
            MGridtem* p_itemAt = dynamic_cast<MGridtem*>(sceneItems[i]);
            if(!p_itemAt)
            {
                continue;
            }

            if(p_itemAt->index()>= min && p_itemAt->index()<= max)
                p_itemAt->setSelected(true);
        }
    }

    return QGraphicsScene::mouseMoveEvent(event);
}

void MGridScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    clearLastSelected();

    return QGraphicsScene::mouseReleaseEvent(event);
}

void MGridScene::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<"MemoryScene::keyPressEvent";

    setInteractiveHighlight(true);

    return QGraphicsScene::keyPressEvent(event);
}

void MGridScene::keyReleaseEvent(QKeyEvent *event)
{
    return QGraphicsScene::keyReleaseEvent(event);
}

//void MemoryScene::drawForeground(QPainter *painter, const QRectF &rect)
//{
//    if(m_interactiveUnit)
//        m_interactiveUnit->update();
//    foreach(MemoryUnit* unit, m_units)
//        unit->update();
//    return QGraphicsScene::drawForeground(painter,rect);
//}

void MGridScene::clearShownUnits()
{
    foreach(MGridUnit* unit,m_units)
        unit->setShowBorder(false);
}

void MGridScene::updateShownUnits()
{
    if(!m_highlightMode)
        return;
    for(int i = m_startHighlight; i<m_startHighlight+m_lengthHighlight; ++i)
    {
        MGridUnit* pmem_unit = m_items[i]->parentUnit();
        if(!pmem_unit)
            continue;
        pmem_unit->setShowBorder(true);
    }
}

void MGridScene::setMemory(const KaMemory& kaMemory/*const QList<MemoryItemPresentation> &mem_it_list,long memSize*/)
{
    MGridUnit * p_memUnit = NULL;

    m_memory = kaMemory;
    setMemorySize(kaMemory.memorySize());

    for(int i = 0; i<memorySize(); ++i)
    {
        m_items.append(new MGridtem(i,itemEdge(),itemBorder(),m_memoryWidget));
    }

    m_memoryWidget->setupMatrix(m_items);

    const QList<KaMemoryPart>& units = kaMemory.memoryParts();

    for(int i = 0; i < units.size(); ++i)
    {
        p_memUnit = new MGridUnit(m_memoryWidget);
        p_memUnit->setState(units[i].state());
        p_memUnit->setUnitId(units[i].id());
        p_memUnit->addItems(units[i].start(),units[i].finish());


        addUnit(p_memUnit);
    }
}

MGridUnit* MGridScene::newUnit(int unitId)
{
    MGridUnit* p_memUnit = NULL;
    if(unitId == -1)
    {
        int vacantUnitId = 1;
        for(;;++vacantUnitId)
        {
            if(!m_unit_by_unitId.contains(vacantUnitId))
                break;
        }
        unitId = vacantUnitId;
    }
    else if(m_unit_by_unitId.contains(unitId))
        return m_unit_by_unitId[unitId];

    p_memUnit = new MGridUnit(m_memoryWidget);
    p_memUnit->setState(Memory::Empty);
    p_memUnit->setUnitId(unitId);

    addUnit(p_memUnit);

    return p_memUnit;
}

MGridUnit *MGridScene::unit(int unitId) const
{
    return m_unit_by_unitId.value(unitId);
}

void MGridScene::setItemInfo(const QString &text)
{
    m_memoryWidget->setItemInfo(text);
    memoryStatusUpdate();
}

void MGridScene::setUnitInfo(const QString &text)
{
    m_memoryWidget->setUnitInfo(text);
    memoryStatusUpdate();
}

//QStatusBar *MemoryScene::statusBar()
//{
//    return m_memoryWidget->m_statusBar;
//}

int MGridScene::itemPerRow() const
{
    return m_memoryWidget->itemPerRow();
}

void MGridScene::setItemPerRow(int newItemPerRow)
{
    if(newItemPerRow==itemPerRow())
        return;

    m_memoryWidget->setItemPerRow(newItemPerRow);
    m_memoryWidget->setupMatrix(m_items);
    updateParenthesis();
}

qreal MGridScene::itemEdge() const
{
    if(m_items.isEmpty())
        return DEFAULT_EDGELENGTH;
    return m_items[0]->edgeLength();
}

void MGridScene::setItemEdge(qreal newEdgeLength)
{
    foreach(MGridtem* item, m_items)
    {
        item->setEdgeLength(newEdgeLength);
    }
}

void MGridScene::clearLastSelected()
{
    m_lastSelected = NULL;
    m_lastSelectedIndex = -1;
}

void MGridScene::setLastSelected(MGridtem *p_mem)
{
    m_lastSelected = p_mem;
    if(!p_mem)
    {
        m_lastSelectedIndex = -1;
    }
    else
    {
        m_lastSelectedIndex = p_mem->index();
    }
}

int MGridScene::itemIndex(QGraphicsItem *item) const
{
    return items().indexOf(item);
}

void MGridScene::memoryStatusUpdate(const QRectF &rect)
{
    m_memoryWidget->memoryStatusUpdate(rect);
}

bool MGridScene::inHighlightRange(long index) const
{
    if(!m_highlightMode)
        return false;
    return m_startHighlight <= index && index < m_startHighlight+m_lengthHighlight;
}

bool MGridScene::errorHandler(QList<ActionErrors> &errors) const
{
    qDebug() <<"MemoryScene::errorHandler";

    if(errors.isEmpty())
        return true;

    qSort(errors);
    ActionErrors topErrorId = errors[0];
    qDebug() << QString::number(topErrorId);


    if(errors.contains(WriteToNotFreed))
    {
        // no need to ask user - error
        QMessageBox::critical(views()[0], tr("Write error")
                ,warning(topErrorId));
        return false;
    }

    if(errors.contains(ReadFromFreed))
    {
        // no need to ask user - error
        QMessageBox::critical(views()[0], tr("Read error")
                ,warning(topErrorId));
        return false;
    }

    int ret = QMessageBox::warning(views()[0], tr("Must be confirmed")
                                   ,warning(topErrorId)+tr("\n Are you sure?")
                                   ,QMessageBox::Apply|QMessageBox::Abort, QMessageBox::Apply);
    switch(ret)
    {
    case QMessageBox::Apply:
        return true;
    case QMessageBox::Abort:
        return false;
    default:
        qDebug() << "MemoryScene::errorHandler default behavior";
        return false;
    }
}
KaMemory MGridScene::memory() const
{
    return m_memory;
}

bool MGridScene::interactiveHighlight() const
{
    if(!m_highlightMode)
        return false;
    return m_interactiveHighlight;
}

void MGridScene::setInteractiveHighlight(bool interactiveHighlight)
{
    m_interactiveHighlight = interactiveHighlight;
}


QString MGridScene::warning(ActionErrors id)
{

    switch (id)
    {
    case WriteToWritten:
        return tr("WriteToWritten");
        //        return "Запись в области ЗУ, содержащие еще не считанные данные!\
//Информация с предыдущих съемок будет потеряна!";
    case WriteToRead:
        return tr("WriteToRead");
//        return "Запись в области ЗУ, содержащие считанные данные, прием\
// которых не подтвержден! Информация с предыдущих съемок может быть потеряна!";
    case ReadFromRead:
        return tr("ReadFromRead");
//        return "Повторное считывание из области ЗУ!\
// Возможно дублирование передаваемой информации!";
    case ReadFromFreed:
        return tr("ReadFromFreed");
//        return "Считывание из области ЗУ,\
// не содержащей данные!";
    case WriteToNotFreed:
        return tr("WriteToAvailable");
//        return "Запись в неочищенные области ЗУ! Необходима предварительная очистка!";
    case FreeOfWritten:
        return tr("FreeOfWritten");
//        return "Очистка областей ЗУ, содержащих еще не считанные данные! Информация с предыдущих съемок будет потеряна!";
    case FreeOfRead:
         return tr("FreeOfRead");
//        return "Очистка областей ЗУ, содержащих считанные данные, прием которых не подтвержден!\
// Информация с предыдущих съемок может быть потеряна!";
    default:
        qDebug() << "MemoryScene::warning id out of range";
        return QString();
    }
}

long MGridScene::lengthHighlight() const
{
        return m_lengthHighlight;
        }

MGridWidget *MGridScene::widget() const
{
        return m_memoryWidget;
}

void MGridScene::addUnit(MGridUnit *p_memUnit)
{
    if(!p_memUnit)
        return;
    m_unit_by_unitId.insert(p_memUnit->unitId(),p_memUnit);
    m_units.append(p_memUnit);
}

void MGridScene::removeUnit(MGridUnit *p_memUnit)
{
    if(!p_memUnit)
        return;

    m_units.removeOne(p_memUnit);
    m_unit_by_unitId.remove(p_memUnit->unitId());

    removeItem(p_memUnit);
}

QList<MGridUnit *> MGridScene::affectedUnits(long from, long to) const
{
    QList<MGridUnit*> result;
    for(int i = from; i <= to; ++i)
    {
        MGridUnit* parUnit = m_items[i]->parentUnit();
        if(!parUnit)
            continue;
        if(!result.contains(parUnit))
            result.append(parUnit);
    }
    return result;
}

long MGridScene::freedCount(long from, long to) const
{
    long result = 0;
    for(int i = from; i <= to; ++i)
    {
        if(!m_items[i]->parentUnit())
        {
            result++;
            continue;
        }
    }
    return result;
}

long MGridScene::startHighlight() const
{
    return m_startHighlight;
}


void MGridScene::setStartHighlight(long startHighlight)
{
    m_highlightMode = true;
    if(startHighlight+m_lengthHighlight>m_memorySize)
        return;
    m_startHighlight = startHighlight;
//    updateShownUnits();
    update();
}


void MGridScene::setLengthHighlight(long lengthHighlight)
{
    m_lengthHighlight = lengthHighlight;
}

bool MGridScene::highlightMode() const
{
    return m_highlightMode;
}

void MGridScene::setHighlightMode(bool highlightMode)
{
    m_highlightMode = highlightMode;
}

void MGridScene::transformChanged(const QTransform &transform)
{
    m_memoryWidget->transformChanged(transform);
}

void MGridScene::setEmpty(long from, long count)
{
    QList<ActionErrors> errors;
    // ANALYSIS
    for(int i = 0, j = from; i<count; ++i,++j)
    {
        Memory::MemoryState itemState = m_items[j]->state();
//        if(itemState == Memory::Written)
//        {
//            errors.append(FreeOfWritten);
//        }
//        else if (itemState == Memory::Read)
//        {
//            errors.append(FreeOfRead);
//        }
    }

    if(errorHandler(errors))
    {
        setState(from,count,Memory::Empty);
    }
}

void MGridScene::setRead(long from, long count)
{

    QList<ActionErrors> errors;
    // ANALYSIS
    for(int i = 0, j = from; i<count; ++i,++j)
    {
        Memory::MemoryState itemState = m_items[j]->state();
//        if(itemState == Memory::Read)
//        {
//            errors.append(ReadFromRead);
//        }
//        else if (itemState == Memory::Freed)
//        {
//            errors.append(ReadFromFreed);
//        }
    }

    if(errorHandler(errors))
    {
        setState(from,count,Memory::PendingRead);
    }

}

void MGridScene::setAvailable(long from, long count)
{

    QList<ActionErrors> errors;
    // ANALYSIS
    for(int i = 0, j = from; i<count; ++i,++j)
    {
        // currently no possible error for change state to -> Available
    }

    if(errorHandler(errors))
    {
        setState(from,count,Memory::Free);
    }

}

void MGridScene::setWritten(long from, long count)
{

    QList<ActionErrors> errors;
    // ANALYSIS
    for(int i = 0, j = from; i<count; ++i,++j)
    {
        Memory::MemoryState itemState = m_items[j]->state();
//        if(itemState == Memory::Written)
//        {
//            errors.append(WriteToWritten);
//            errors.append(WriteToNotFreed);
//        }
//        else if (itemState == Memory::Read)
//        {
//            errors.append(WriteToRead);
//            errors.append(WriteToNotFreed);
//        }
//        else if (itemState == Memory::Available)
//        {
//            errors.append(WriteToNotFreed);
//        }
    }

    if(errorHandler(errors))
    {
        setState(from,count,Memory::PendingWrite);
    }


}

void MGridScene::setState(long from, long count, MemoryState state)
{

    clear(from,count);
    if(state == Memory::Empty)
        return;



    MGridUnit* p_mu = newUnit();
    p_mu->setState(state);
    p_mu->addItems(from,from+count-1);

//    updateParenthesis();
//    update();
}

void MGridScene::clear(long from, long count)
{
    if(from+count>=m_memorySize)
    {
        qDebug() <<"MemoryScene::free";
        count = m_memorySize-from-1;
    }
    QList<MGridUnit*> memoryUnits = affectedUnits(from,from+count-1);
    count-= freedCount(from,from+count-1);

    foreach(MGridUnit* unit, memoryUnits)
    {
        count-=unit->removeItems(qMax(unit->start(),from),count);
    }
    qDebug() << "MemoryScene::free";
}
long MGridScene::memorySize() const
{
    return m_memorySize;
}

void MGridScene::setMemorySize(long memorySize)
{
    m_memorySize = memorySize;
}

void MGridScene::viewResized(QSizeF viewSize)
{
    qDebug()<<"viewResized";

    qreal viewWidth = viewSize.width();

    int newItemPerRow = (viewWidth)
                              /(itemSize())
                        - ((m_memoryWidget->labels())?1:0);

    if(itemPerRow()!=newItemPerRow)
        setItemPerRow(newItemPerRow);

    setSceneRect(itemsBoundingRect());
}

void MGridScene::showInteractiveRange(long start, long finish)
{
    if(!m_interactiveUnit)
        return;
    m_interactiveUnit->setRange(start,finish);
}

void MGridScene::hideInteractiveRange()
{
    if(!m_interactiveUnit)
        return;
    m_interactiveUnit->setShowBorders(false);
}

qreal MGridScene::itemSize() const
{
    return spacing()+itemEdge()+2*itemBorder();
}

void MGridScene::updateParenthesis()
{
    for(int i=0;i<m_units.size();++i)
    {
        m_units[i]->updateParenthesis();
    }
}

qreal MGridScene::itemBorder() const
{
    if(m_items.isEmpty())
        return DEFAULT_BORDERWIDTH;
    return m_items[0]->borderWidth();
}

void MGridScene::setItemBorder(qreal itemBorder)
{
    foreach(MGridtem* item,m_items)
        item->setBorderWidth(itemBorder);
}


qreal MGridScene::spacing() const
{
    return m_memoryWidget->spacing();
}

void MGridScene::setSpacing(const qreal &spacing)
{
    m_memoryWidget->setSpacing(spacing);
}




