#include "mgrid_scene.h"
#include "memoryview.h"

#include "mgrid_item.h"
#include "mgrid_unit.h"

#include "kamemory.h"
#include "mgrid_interactiveunit.h"

#include <QGraphicsSceneMouseEvent>

#include <QApplication>
#include <QDebug>

#include <QtCore/qmath.h>

#include <QMessageBox>
#include <QMutex>

qreal qSqrt(qreal);

MGridScene::MGridScene( QObject * parent)
    :QGraphicsScene(parent)
{
    m_highlightStyle = noHighlightStyle;
    m_selectionMode = SelectionMode_count;
    m_lastSelected = NULL;
    m_mouseOverItem = NULL;
    m_mouseOverUnit = NULL;
    m_interactiveUnit = NULL;
    m_tmpInteractiveLength = 0;

    setSelectionMode(noSelection);
    setHighlightStyle( bordersAround | highlightedArea | highlightedItems);
    setItemIndexMethod(QGraphicsScene::NoIndex);


    m_itemEdge = DEFAULT_EDGELENGTH;
    m_itemBorder = DEFAULT_BORDERWIDTH;
    m_itemPerRow = 0;

    clear();

    m_highlightMode = false;
    m_interactiveHighlight = false;

    setLengthSelection(100);

}

MGridScene::~MGridScene()
{
}

void MGridScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_leftButtonPressed = true;

    if(m_interactiveHighlight)
    {
        MGridItem * p_mem = itemAtPos(event->scenePos());
        if(p_mem)
        {
            if(m_selectionMode == areaSelection)
            {
                setLengthSelection(0);
                if(setStartSelection(p_mem->index()))
                    setLastSelected(p_mem);
            }
            if(m_highlightStyle&highlightedItems)
                setHighlightMode(true);
            setInteractiveHighlight(true);
        }
    }
    mouseMoveEvent(event);
    return QGraphicsScene::mousePressEvent(event);

}

void MGridScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    MGridItem * p_mem = itemAtPos(event->scenePos());
    MGridUnit * p_unit = (p_mem)?(p_mem->unit()):(NULL);

    setMouseOverItem(p_mem);
    setMouseOverUnit(p_unit);

    // INTERACTIVE SELECTION
    if(m_interactiveHighlight)
    {
        if(m_leftButtonPressed && (m_selectionMode == positionSelection) && p_mem)
        {
            setStartSelection(p_mem->index());
        }
        if((m_selectionMode == areaSelection) && m_lastSelected)
        {
            if(p_mem && (m_selectionMode == areaSelection) && m_lastSelected)
            {
                if(p_mem->index() < m_lastSelected->index())
                {
                    if(setStartSelection(p_mem->index()))
                    {
                        long length = m_lastSelected->index() - p_mem->index() + 1;
                        setLengthSelection(length);
                    }
                }
                else
                {
                    setStartSelection(m_lastSelected->index());
                    long length = p_mem->index() - m_lastSelected->index() + 1;
                    setLengthSelection(length);
                }
            }
        }
//        if(lengthSelection())
//            updateInteractiveRange(startSelection(),lengthSelection());
    }

    return QGraphicsScene::mouseMoveEvent(event);
}

void MGridScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_leftButtonPressed = false;
    // INTERACTIVE SELECTION
//    setInteractiveHighlight(false);

    // ITEMS SELECT <disabled>
    if(m_lastSelected)
    {
        clearLastSelected();
        emit intervalHasSelected();
    }



    return QGraphicsScene::mouseReleaseEvent(event);
}

void MGridScene::clear()
{
//    return;
    m_items.clear();
    m_units.clear();
    clearInteractiveUnit();
    clearItems();
    QGraphicsScene::clear();
//    clearUnits();

    m_lastSelected = NULL;
    clearMouseOver();
}

void MGridScene::clearInteractiveUnit()
{
    if(m_interactiveUnit)
    {
        if(m_selectionMode == positionSelection)
        {
            m_tmpInteractiveLength = m_interactiveUnit->length();
        }
        removeItem(m_interactiveUnit);
        m_interactiveUnit = NULL;
    }
}

bool MGridScene::isInteractiveUnit()
{
    return m_interactiveUnit;
}

void MGridScene::clearShownUnits()
{
    foreach(MGridUnit* unit,m_units)
        unit->setShowBorder(false);
}

void MGridScene::updateShownUnits()
{
    for(int i = startSelection(); i<startSelection()+lengthSelection(); ++i)
    {
        MGridUnit* pmem_unit = m_items[i]->unit();
        if(!pmem_unit)
            continue;
        pmem_unit->setShowBorder(true);
    }
}

void MGridScene::setMemory(QSharedPointer<KaMemory> &kaMemory)
{
    clear();
    m_memory = kaMemory;
    if(m_memory.isNull())
        return;

    for(int i = 0; i<memorySize(); ++i)
    {
        MGridItem* newItem = new MGridItem(i,this,itemEdge(),itemBorder());
        m_items.append(newItem);
    }

    setLimits(0,memorySize());

    setupMatrix(m_items);

    const QList<QSharedPointer<KaMemoryPart> >& units = kaMemory->memoryParts();

    for(int i = 0; i < units.size(); ++i)
        addUnit(units[i]);

}

MGridUnit* MGridScene::newUnit()
{
    MGridUnit* p_memUnit = new MGridUnit(QSharedPointer<KaMemoryPart>(new KaMemoryPart()),this);
    p_memUnit->setState(KaMemoryPart::Empty);

    if(!memory().isNull())
        memory()->memoryParts().append(p_memUnit->kaMemoryPart());
    addUnit(p_memUnit);
    return p_memUnit;
}

int MGridScene::itemPerRow() const
{
    return m_itemPerRow;
}

void MGridScene::setItemPerRow(int newItemPerRow)
{
    if(newItemPerRow==itemPerRow()||!newItemPerRow)
        return;

    m_itemPerRow = newItemPerRow;
    setupMatrix(m_items);

    foreach(MGridUnit* unit, m_units)
        unit->rebuildShape();
}

qreal MGridScene::itemEdge() const
{
    return m_itemEdge;
}

void MGridScene::setItemEdge(qreal newEdgeLength)
{
    if(m_itemEdge == newEdgeLength)
        return;
    m_itemEdge = newEdgeLength;

    if(!views().isEmpty())
    {
        viewResized(views()[0]->viewport()->size());
    }

    foreach(MGridItem* item, m_items)
    {
        item->setEdgeLength(m_itemEdge);
    }

    foreach(MGridUnit* unit, m_units)
    {
        unit->rebuildShape();
    }
}

void MGridScene::clearLastSelected()
{
    m_lastSelected = NULL;
}

void MGridScene::setLastSelected(MGridItem *p_mem)
{
    m_lastSelected = p_mem;
}

int MGridScene::itemIndex(QGraphicsItem *item) const
{
    return items().indexOf(item);
}


bool MGridScene::inHighlightRange(long index) const
{
    if(m_highlightStyle&highlightedArea)
    {
        return m_min <= index && index <= m_max;
    }

    if(!m_highlightMode)
        return true;
    return startSelection() <= index && index <startSelection()+lengthSelection();
}

bool MGridScene::errorHandler(QList<ActionErrors> &errors) const
{
    return true;
    if(errors.isEmpty())
        return true;

    qSort(errors);
    ActionErrors topErrorId = errors[0];


    if(errors.contains(WriteToNotEmpty))
    {
        // no need to ask user - error
        QMessageBox::critical(views()[0], tr("Write error")
                ,warning(topErrorId));
        return false;
    }

    if(errors.contains(ReadOfEmpty))
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

MGridScene::SelectionMode MGridScene::selectionMode() const
{
    return m_selectionMode;
}

void MGridScene::hoverParts(const QList<KaMemoryPart> &parts)
{
    foreach(KaMemoryPart part, parts)
        hoverPart(part);
}

void MGridScene::hoverPart(const KaMemoryPart &part)
{

    foreach(MGridUnit* unit, m_units)
    {
        if((*unit->kaMemoryPart())==part)
        {
            unit->setShowBorder(true);
            unit->rebuildShape();
        }
        else
        {
            unit->setShowBorder(false);
            unit->rebuildShape();
        }
    }
}

void MGridScene::setSelectionMode(const SelectionMode &selectionMode)
{
    if(m_selectionMode == selectionMode)
        return;

    if(selectionMode != noSelection)
        setInteractiveHighlight(true);
    else
    {
        setInteractiveHighlight(false);
    }

    m_selectionMode = selectionMode;
}
MGridUnit *MGridScene::mouseOverUnit() const
{
    return m_mouseOverUnit;
}

void MGridScene::setMouseOverUnit(MGridUnit *mouseOverUnit)
{
    if(m_mouseOverUnit == mouseOverUnit)
        return;

    if(m_mouseOverUnit)
        m_mouseOverUnit->update();

    m_mouseOverUnit = mouseOverUnit;
    if(m_mouseOverUnit == NULL)
    {
        emit setUnitInfo(" - ");
    }
    else
    {
        m_mouseOverUnit->update();
        if(!m_mouseOverUnit->length()==0)
            emit setUnitInfo(QString(   /*QObject::tr("id: ")
                                            +QString::number(m_mouseOverUnit->id())
                                            +*/QString(QObject::tr(" Состояние: "))
                                            +m_mouseOverUnit->state()
                                            +QObject::tr(" Адрес: ")
                                            +toAdress(m_mouseOverUnit->start(),m_mouseOverUnit->start()+m_mouseOverUnit->length()-1)
                                 ));
    }
}

void MGridScene::clearItems()
{
    foreach(MGridItem* item, m_items)
    {
        item->setParentItem(NULL);
        removeItem(item);
        delete item;
    }
}

void MGridScene::clearUnits()
{
    foreach(MGridUnit* unit, m_units)
    {
        unit->setParentItem(NULL);
        removeItem(unit);
        delete unit;
    }
}

MGridItem *MGridScene::mouseOverItem() const
{
    return m_mouseOverItem;
}

void MGridScene::setMouseOverItem(MGridItem *mouseOver)
{
    if(m_mouseOverItem == mouseOver)
        return;

    if(m_mouseOverItem)
        m_mouseOverItem->update();

    m_mouseOverItem = mouseOver;
    if(m_mouseOverItem == NULL)
    {
        emit setItemInfo(" - ");
    }
    else
    {
        m_mouseOverItem->update();
        emit setItemInfo(QString::number(mouseOver->index()));
    }
}

MGridScene::HighlightStyle MGridScene::highlightStyle() const
{
    return m_highlightStyle;
}

void MGridScene::setHighlightStyle(int highlightStyle)
{
    if((highlightStyle&(~(0x1|0x2|0x4))))
    {
        qWarning("highlightStyle out of range");
        return;
    }

    m_highlightStyle = static_cast<HighlightStyle>(highlightStyle);

    if(m_highlightStyle&highlightedItems)
        setHighlightMode(true);

}

QSharedPointer<KaMemory> &MGridScene::memory()
{
//    updateMemory();
    return m_memory;
}

//void MGridScene::updateMemory()
//{
//    QList<MemoryPart> parts;

//    foreach(MGridUnit* unit,m_units)
//    {
//        MemoryPart part;
//        part.setStart(unit->start());
//        part.setFinish(unit->finish());
//        part.setId(unit->id());
//        part.setState(unit->state());
//        parts.append(part);
//    }

//    m_memory.init(parts,m_memory.memorySize());
//    emit memoryChanged();
//}

bool MGridScene::interactiveHighlight() const
{
    return m_interactiveHighlight;
}

//void MGridScene::updateInteractiveRange()
//{
//    if(m_interactiveUnit==NULL)
//        return;
//    m_interactiveUnit->rebuildShape();
//}

void MGridScene::setInteractiveHighlight(bool interactiveHighlight)
{
    if(m_interactiveHighlight&&!interactiveHighlight)
        emit intervalHasSelected();
    else if(!m_interactiveHighlight&&interactiveHighlight)
        emit intervalSelectionStarted();

    m_interactiveHighlight = interactiveHighlight;
    emit interactiveHighlightChanged(m_interactiveHighlight);
}

void MGridScene::setLimits(long min, long max)
{
    if(min>max)
    {
        Q_ASSERT(false);
        return;
    }
    m_min = min;
    m_max = max;
}


QString MGridScene::warning(ActionErrors id)
{

    switch (id)
    {
    case WriteToNotRead:
        return tr("WriteToNotRead");
        //        return "Запись в области ЗУ, содержащие еще не считанные данные!
//Информация с предыдущих съемок будет потеряна!";
    case WriteToPendingRead:
        return tr("WriteToPendingRead");
//        return "Запись в области ЗУ, содержащие считанные данные, прием
// которых не подтвержден! Информация с предыдущих съемок может быть потеряна!";
    case ReadOfReaded:
        return tr("ReadOfReaded");
//        return "Повторное считывание из области ЗУ!
// Возможно дублирование передаваемой информации!";
    case ReadOfEmpty:
        return tr("ReadOfEmpty");
//        return "Считывание из области ЗУ,
// не содержащей данные!";
    case WriteToNotEmpty:
        return tr("WriteToNotEmpty");
//        return "Запись в неочищенные области ЗУ! Необходима предварительная очистка!";
    case EmptyOfNotRead:
        return tr("EmptyOfNotRead");
//        return "Очистка областей ЗУ, содержащих еще не считанные данные! Информация с предыдущих съемок будет потеряна!";
    case EmptyOfPendingRead:
         return tr("EmptyOfPendingRead");
//        return "Очистка областей ЗУ, содержащих считанные данные, прием которых не подтвержден!
// Информация с предыдущих съемок может быть потеряна!";
    default:
        qDebug() << "MemoryScene::warning id out of range";
        return QString();
    }
}

long MGridScene::lengthSelection() const
{
    if(!m_interactiveUnit)
        return 0;
    return m_interactiveUnit->length();
}

void MGridScene::addUnit(QSharedPointer<KaMemoryPart> part)
{
    MGridUnit* p_memUnit = new MGridUnit(part,this);
    addUnit(p_memUnit);

}

void MGridScene::setItemInfo(const QString &text)
{
    emit itemInfoChanged(text);
}

void MGridScene::setUnitInfo(const QString &text)
{
    emit unitInfoChanged(text);
}


void MGridScene::addUnit(MGridUnit *p_memUnit)
{
    if(!p_memUnit)
        return;


    connect(p_memUnit,SIGNAL(hoverEnter()),this,SLOT(onUnitHoverEnter()));
    connect(p_memUnit,SIGNAL(hoverLeave()),this,SLOT(onUnitHoverLeave()));

    m_units.append(p_memUnit);
}

void MGridScene::removeUnit(MGridUnit *p_memUnit)
{
    if(!p_memUnit)
        return;

    m_units.removeOne(p_memUnit);
    removeItem(p_memUnit);
}

QList<MGridUnit *> MGridScene::crossingParts(long start, long length) const
{
    QList<MGridUnit*> result;

    if(!highlightMode()
            || m_items.size()<start+length
            || m_items.isEmpty()
            || start<0)
        return result;

    for(int i = start; i < start+length; ++i)
    {
        MGridUnit* parUnit = m_items[i]->unit();
        if(!parUnit)
            continue;
        if(!result.contains(parUnit))
            result.append(parUnit);
    }
    return result;
}

void MGridScene::setupMatrix(const QVector<MGridItem *> &items)
{
    if(!m_itemPerRow)
        return;
    int row = 0, col = 0;
    for(int i=0;i<items.size();++i)
    {
        items[i]->setPos(col*itemSize(),row*itemSize());
        if(++col%m_itemPerRow == 0)
        {
            col = 0;
            row++;
        }
    }
}

void MGridScene::clearMouseOver()
{
    setMouseOverItem(NULL);
    setMouseOverUnit(NULL);
}

void MGridScene::onUnitHoverEnter()
{
    MGridUnit *p_unit = dynamic_cast<MGridUnit*>(sender());
    if(!p_unit)
        return;

    emit kaMemoryPartHoverEnter(*p_unit->kaMemoryPart());
}

void MGridScene::onUnitHoverLeave()
{
    MGridUnit *p_unit = dynamic_cast<MGridUnit*>(sender());
    if(!p_unit)
        return;


    emit kaMemoryPartHoverLeave(*p_unit->kaMemoryPart());
}

QList<QSharedPointer<KaMemoryPart> > MGridScene::crossingParts() const
{
    QList<QSharedPointer<KaMemoryPart> > result;
    QList<MGridUnit*> units;

    if(lengthSelection())
        units = crossingParts(startSelection(),lengthSelection());
    foreach(MGridUnit* unit, units)
    {
        result.append(unit->kaMemoryPart());
    }
    return result;
}

QString MGridScene::toAdress(long start, long finish)
{
    return "0x"+fixedNumPresentation(start,16,memorySize()-1)
            +" - 0x"+fixedNumPresentation(finish,16,memorySize()-1);
}

void MGridScene::setKaMemoryPart(const KaMemoryPart &part)
{
    long start = part.start(),
            length = part.length();

    switch(part.state())
    {
    case KaMemoryPart::Empty:
        setEmpty(start,length);
        break;
    case KaMemoryPart::Free:
        setFree(start,length);
        break;
    case KaMemoryPart::PendingRead:
        setPendingRead(start,length);
        break;
    case KaMemoryPart::PendingWrite:
        setPendingWrite(start,length);
        break;
    default:
        qWarning() << tr("trying to set no-user type of state");
        return;
    }
}

long MGridScene::freedCount(long from, long to) const
{
    long result = 0;
    if(m_items.size()<=to)
    {
        Q_ASSERT(false);
        return 0;
    }
    for(int i = from; i <= to; ++i)
    {
        if(!m_items[i]->unit())
        {
            result++;
            continue;
        }
    }
    return result;
}

long MGridScene::startSelection() const
{
    if(m_interactiveUnit == NULL)
        return 0;
    return m_interactiveUnit->start();
}


bool MGridScene::setStartSelection(long startHighlight)
{
    if(m_highlightStyle&highlightedItems)
        setHighlightMode(true);

    if(startHighlight==startSelection())
        return true;
    if(startHighlight+(lengthSelection()?(lengthSelection()-1):0)>m_max)
        startHighlight=m_max-lengthSelection();
    if(startHighlight<m_min)
        startHighlight=m_min;

    if(!m_interactiveUnit)
    {
        m_interactiveUnit = new MGridInteractiveUnit(this);
        addItem(m_interactiveUnit);
        if(m_selectionMode == positionSelection)
        {
            m_interactiveUnit->setLength(m_tmpInteractiveLength);
        }
    }

    if(m_interactiveUnit==NULL)
        return false;

    m_interactiveUnit->setStart(startHighlight);
    emit startHighlightChanged(startHighlight);
    return true;
}


bool MGridScene::setLengthSelection(long lengthHighlight)
{
    if(m_highlightStyle&highlightedItems)
        setHighlightMode(true);

    if(!lengthHighlight)
    {
        setHighlightMode(false);
    }

    if(lengthHighlight!=0 &&!(m_selectionMode == positionSelection) && startSelection() + lengthHighlight - 1 > m_max)
        return false;

    if(!m_interactiveUnit)
    {
        m_interactiveUnit = new MGridInteractiveUnit(this);
        addItem(m_interactiveUnit);
    }

    m_tmpInteractiveLength = lengthHighlight;
    m_interactiveUnit->setLength(lengthHighlight);
    emit lengthHighlightChanged(lengthSelection());
    return true;
}

bool MGridScene::highlightMode() const
{
    return m_highlightMode;
}

void MGridScene::setHighlightMode(bool highlightMode)
{
    if(highlightMode == m_highlightMode)
        return;
    m_highlightMode = highlightMode;
    update();
}


QSharedPointer<KaMemoryPart>  MGridScene::setEmpty(long from, long count)
{
    QList<ActionErrors> errors;
    // ANALYSIS
    if(from+count>m_items.size())
    {
        if(from>=m_items.size())
        {
            return QSharedPointer<KaMemoryPart>();
        }
        else
        {
            count = m_items.size()-from;
        }
    }

    for(int i = 0, j = from; i<count; ++i,++j)
    {
        KaMemoryPart::KaMemoryState itemState = m_items[j]->state();
        if(m_notReadStates.contains(itemState))
        {
            errors.append(EmptyOfNotRead);
        }
        if (itemState == KaMemoryPart::PendingRead)
        {
            errors.append(EmptyOfPendingRead);
        }
    }

    if(errorHandler(errors))
    {
        return setState(from,count,KaMemoryPart::Empty);
    }

    return QSharedPointer<KaMemoryPart>();
}

QSharedPointer<KaMemoryPart>  MGridScene::setPendingRead(long from, long count)
{

    QList<ActionErrors> errors;
    // ANALYSIS
    if(from+count>m_items.size())
        return QSharedPointer<KaMemoryPart>();

    for(int i = 0, j = from; i<count; ++i,++j)
    {
        KaMemoryPart::KaMemoryState itemState = m_items[j]->state();
        if(itemState == KaMemoryPart::PendingRead)
        {
            errors.append(ReadOfReaded);
        }
        else if (itemState == KaMemoryPart::Empty)
        {
            errors.append(ReadOfEmpty);
        }
    }

    if(errorHandler(errors))
    {
        return setState(from,count,KaMemoryPart::PendingRead);
    }

    return QSharedPointer<KaMemoryPart>();
}

QSharedPointer<KaMemoryPart>  MGridScene::setFree(long from, long count)
{

    QList<ActionErrors> errors;
    // ANALYSIS
    if(from+count>m_items.size())
        return QSharedPointer<KaMemoryPart>();

    for(int i = 0, j = from; i<count; ++i,++j)
    {
        // currently no possible error for change state to -> Available
    }

    if(errorHandler(errors))
    {
        return setState(from,count,KaMemoryPart::Free);
    }

    return QSharedPointer<KaMemoryPart>();
}

QSharedPointer<KaMemoryPart>  MGridScene::setPendingWrite(long from, long count)
{

    QList<ActionErrors> errors;
    // ANALYSIS
    if(from+count>m_items.size())
        return QSharedPointer<KaMemoryPart>();

    for(int i = 0, j = from; i<count; ++i,++j)
    {
        KaMemoryPart::KaMemoryState itemState = m_items[j]->state();
        if(m_writeStates.contains(itemState))
        {
            errors.append(WriteToNotRead);
            errors.append(WriteToNotEmpty);
        }
        else if (itemState == KaMemoryPart::PendingRead)
        {
            errors.append(WriteToPendingRead);
            errors.append(WriteToNotEmpty);
        }
        else if (itemState == KaMemoryPart::Free)
        {
            errors.append(WriteToNotEmpty);
        }
    }

    if(errorHandler(errors))
    {
        return setState(from,count,KaMemoryPart::PendingWrite);
    }

    return QSharedPointer<KaMemoryPart>();
}

QSharedPointer<KaMemoryPart>  MGridScene::setEmpty()
{
    if(!lengthSelection())
        return QSharedPointer<KaMemoryPart>();
    return setEmpty(startSelection(),lengthSelection());
}

QSharedPointer<KaMemoryPart>  MGridScene::setFree()
{
    if(!lengthSelection())
        return QSharedPointer<KaMemoryPart>();
    return setFree(startSelection(),lengthSelection());
}

QSharedPointer<KaMemoryPart>  MGridScene::setPendingRead()
{
    if(!lengthSelection())
        return QSharedPointer<KaMemoryPart>();
    return setPendingRead(startSelection(),lengthSelection());
}

QSharedPointer<KaMemoryPart>  MGridScene::setPendingWrite()
{
    if(!lengthSelection())
        return QSharedPointer<KaMemoryPart>();
    return setPendingWrite(startSelection(),lengthSelection());
}

QSharedPointer<KaMemoryPart> MGridScene::setState(long from, long count, KaMemoryPart::KaMemoryState state)
{
    qDebug() <<"setState "<<state;
    qDebug() <<"from "<<from <<" length "<<count;
    MGridUnit* p_mu = NULL;
    clearMemory(from,count);

    if(state != KaMemoryPart::Empty)
    {
        p_mu = newUnit();
        p_mu->setState(state);
        p_mu->addItems(from,count);
    }

    update();
    emit memoryChanged();
    return (p_mu)?(p_mu->kaMemoryPart()):(QSharedPointer<KaMemoryPart>());
}

void MGridScene::clearMemory(long from, long count)
{
    if(from+count>memorySize())
    {
        count = memorySize()-from-1;
    }
    QList<MGridUnit*> memoryUnits = crossingParts(from,count);
    count-= freedCount(from,from+count-1);

    foreach(MGridUnit* unit, memoryUnits)
    {
        count-=unit->removeItems(qMax(unit->start(),from),count);
    }
}

MGridItem *MGridScene::itemAtPos(const QPointF &pos) const
{
    int col = pos.x()/itemSize(),
        row = pos.y()/itemSize();

    long index = row*itemPerRow()+col;

    if(index<0)
    {
        if(m_items.isEmpty())
            return NULL;
        return m_items[0];
    }

    if(index>=m_items.size())
    {
        if(m_items.isEmpty())
            return NULL;
        return m_items[m_items.size()-1];
    }

    return m_items[index];
}

//MGridUnit *MGridScene::unitAtPos(const QPointF &pos) const
//{
//    QList<QGraphicsItem*> itemsAtPos = items(pos);
//    MGridUnit * p_mem = NULL;

//    foreach(QGraphicsItem* itemAtPos,itemsAtPos)
//    {
//        if((p_mem=dynamic_cast<MGridUnit*>(itemAtPos)))
//            break;
//    }
//    return p_mem;
//}

bool MGridScene::isMouseOverUnit(MGridUnit *p_unit) const
{
    return m_mouseOverUnit == p_unit;
}

bool MGridScene::isMouseOverItem(MGridItem* p_item) const
{
    if(m_interactiveHighlight)
        return false;
    return m_mouseOverItem==p_item;
}
long MGridScene::memorySize() const
{
    if(m_memory.isNull())
        return 0;
    return m_memory->memorySize();
}

void MGridScene::viewResized(QSizeF viewSize)
{
    qreal viewWidth = viewSize.width();

    int newItemPerRow = (viewWidth)
                              /(itemSize());

    if(!newItemPerRow)
        return;

    if(itemPerRow()!=newItemPerRow)
    {
        setSceneRect(0,0,viewWidth,(memorySize()/newItemPerRow + 1)*itemSize());
        setItemPerRow(newItemPerRow);
        if(m_interactiveUnit)
            m_interactiveUnit->rebuildShape();
    }
}

qreal MGridScene::itemSize() const
{
    return /*spacing()+*/itemEdge()+2*itemBorder();
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
    return m_itemBorder;
}

void MGridScene::setItemBorder(qreal itemBorder)
{
    if(m_itemBorder == itemBorder)
        return;
    m_itemBorder = itemBorder;
    foreach(MGridItem* item,m_items)
        item->setBorderWidth(m_itemBorder);

    foreach(MGridUnit* unit, m_units)
        unit->rebuildShape();
}

bool MGridScene::setSelected(long start, long length)
{
    long lastStart = startSelection();
    if(!setStartSelection(start))
        return false;
    if(!setLengthSelection(length))
    {
        setStartSelection(lastStart);
        return false;
    }
    return true;
}






QList<KaMemoryPart::KaMemoryState> MGridScene::m_notReadStates,
                    MGridScene::m_writeStates,
                    MGridScene::m_errorStates;


MGridScene::Initializer MGridScene::initializerGuard;

MGridScene::Initializer::Initializer()
{
    m_notReadStates.append(KaMemoryPart::Recorded);
    m_notReadStates.append(KaMemoryPart::PendingRead);
    m_notReadStates.append(KaMemoryPart::PendingWrite);

    m_writeStates.append(KaMemoryPart::Recorded);
    m_writeStates.append(KaMemoryPart::PendingWrite);

    m_errorStates.append(KaMemoryPart::ErrorRead);
    m_errorStates.append(KaMemoryPart::ErrorWrite);
}
