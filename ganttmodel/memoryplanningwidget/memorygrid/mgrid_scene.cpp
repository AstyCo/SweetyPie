#include "mgrid_scene.h"
#include "memoryview.h"

#include "mgrid_item.h"
#include "mgrid_widget.h"
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
    clearLastSelected();

    setItemEdge(DEFAULT_EDGELENGTH);
    setItemBorder(DEFAULT_BORDERWIDTH);

    m_items.clear();
    m_units.clear();
    m_lastSelected = NULL;
    m_highlightMode = false;
    m_interactiveHighlight = false;

    setLengthHighlight(100);

    setHighlightStyle( bordersAround | highlightedItems);


    m_memoryWidget = new MGridWidget;
    m_interactiveUnit = NULL;
    m_interactiveUnit = new MGridInteractiveUnit(this,m_memoryWidget);

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
    // INTERACTIVE SELECTION

    if(m_highlightStyle&highlightedItems)
        setHighlightMode(true);
    setInteractiveHighlight(true);

    // ITEMS SELECT <disabled>
    MGridItem * p_mem = itemAtPos(event->scenePos());
    if(p_mem)
        setLastSelected(p_mem);

    mouseMoveEvent(event);
    return QGraphicsScene::mousePressEvent(event);

}

void MGridScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    MGridItem * p_mem = itemAtPos(event->scenePos());
    MGridUnit * p_unit = unitAtPos(event->scenePos());

    setMouseOverItem(p_mem);
    setMouseOverUnit(p_unit);

    // INTERACTIVE SELECTION
    if(m_interactiveHighlight)
    {
        if(p_mem)
        {
            setStartHighlight(p_mem->index());
        }
        showInteractiveRange(startHighlight(),finishHighlight());
    }


    // ITEMS SELECT <disabled>
    if(m_lastSelected)
    {
        if(p_mem)
        {
            clearSelection();

            long newSelectedIndex = p_mem->index();

            long min = qMin(m_lastSelectedIndex,newSelectedIndex),
                    max = qMax(m_lastSelectedIndex,newSelectedIndex);


            qDebug() << "setting selection from "<<QString::number(min)
                     <<" to "<<QString::number(max);

            for(int i = 0; i < m_items.size(); ++i)
            {
                if(m_items[i]->index()>= min && m_items[i]->index()<= max)
                    m_items[i]->setSelected(true);
            }
        }
    }

    return QGraphicsScene::mouseMoveEvent(event);
}

void MGridScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // INTERACTIVE SELECTION
    setInteractiveHighlight(false);

    // ITEMS SELECT <disabled>
    clearLastSelected();


    return QGraphicsScene::mouseReleaseEvent(event);
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
    for(int i = m_startHighlight; i<m_startHighlight+m_lengthHighlight; ++i)
    {
        MGridUnit* pmem_unit = m_items[i]->parentUnit();
        if(!pmem_unit)
            continue;
        pmem_unit->setShowBorder(true);
    }
}

void MGridScene::setMemory(const KaMemory& kaMemory)
{
    MGridUnit * p_memUnit = NULL;

    m_memory = kaMemory;

    for(int i = 0; i<memorySize(); ++i)
    {
        m_items.append(new MGridItem(i,itemEdge(),itemBorder(),m_memoryWidget));
    }

    m_memoryWidget->setupMatrix(m_items);

    const QList<KaMemoryPart>& units = kaMemory.memoryParts();

    for(int i = 0; i < units.size(); ++i)
        addUnit(units[i]);
}

MGridUnit* MGridScene::newUnit()
{

    MGridUnit* p_memUnit = new MGridUnit(m_memoryWidget);
    p_memUnit->setState(Memory::Empty);

    addUnit(p_memUnit);
    return p_memUnit;
}

//void MGridScene::setItemInfo(const QString &text)
//{
//    m_memoryWidget->setItemInfo(text);
//    memoryStatusUpdate();
//}

//void MGridScene::setUnitInfo(const QString &text)
//{
//    m_memoryWidget->setUnitInfo(text);
//    memoryStatusUpdate();
//}

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
    foreach(MGridItem* item, m_items)
    {
        item->setEdgeLength(newEdgeLength);
    }
}

void MGridScene::clearLastSelected()
{
    m_lastSelected = NULL;
    m_lastSelectedIndex = -1;
}

void MGridScene::setLastSelected(MGridItem *p_mem)
{
    qDebug() <<"setLastSelected";
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
        return true;
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
MGridUnit *MGridScene::mouseOverUnit() const
{
    return m_mouseOverUnit;
}

void MGridScene::setMouseOverUnit(MGridUnit *mouseOverUnit)
{
    m_mouseOverUnit = mouseOverUnit;
    if(m_mouseOverUnit == NULL)
    {
        emit setUnitInfo(" - ");
    }
    else
    {
        emit setUnitInfo(QString(   QObject::tr("Unit Group Id: ")
                                        +QString::number(m_mouseOverUnit->id())
                                        +QString(QObject::tr("  Unit State: "))
                                        +m_mouseOverUnit->state()
                                        +QObject::tr(" Unit Memory: ")
                                        +"0x"+fixedNumPresentation(m_mouseOverUnit->start(),16,memorySize()-1)
                                        +" - 0x"+fixedNumPresentation(m_mouseOverUnit->finish(),16,memorySize()-1)
                             ));
    }
}

MGridItem *MGridScene::mouseOverItem() const
{
    return m_mouseOverItem;
}

void MGridScene::setMouseOverItem(MGridItem *mouseOver)
{
    m_mouseOverItem = mouseOver;
    if(m_mouseOverItem == NULL)
    {
        emit setItemInfo(" - ");
    }
    else
    {
        emit setItemInfo(QString::number(mouseOver->index()));
    }
}

MGridScene::HighlightStyle MGridScene::highlightStyle() const
{
    return m_highlightStyle;
}

void MGridScene::setHighlightStyle(int highlightStyle)
{
    m_highlightStyle = static_cast<HighlightStyle>(highlightStyle);
}

KaMemory MGridScene::memory()
{
    updateMemory();
    return m_memory;
}

void MGridScene::updateMemory()
{
    QList<KaMemoryPart> parts;

    foreach(MGridUnit* unit,m_units)
    {
        KaMemoryPart part;
        part.setStart(unit->start());
        part.setFinish(unit->finish());
        part.setId(unit->id());
        part.setState(unit->state());
        parts.append(part);
    }

    m_memory.init(parts,m_memory.memorySize());
    emit memoryChanged();
}

bool MGridScene::interactiveHighlight() const
{
    return m_interactiveHighlight;
}

void MGridScene::setInteractiveHighlight(bool interactiveHighlight)
{
    if(m_interactiveHighlight&&!interactiveHighlight)
        emit intervalHasSelected();
    else if(!m_interactiveHighlight&&interactiveHighlight)
        emit intervalSelectionStarted();

    m_interactiveHighlight = interactiveHighlight;
    emit interactiveHighlightChanged(m_interactiveHighlight);
}


QString MGridScene::warning(ActionErrors id)
{

    switch (id)
    {
    case WriteToNotRead:
        return tr("WriteToNotRead");
        //        return "Запись в области ЗУ, содержащие еще не считанные данные!\
//Информация с предыдущих съемок будет потеряна!";
    case WriteToPendingRead:
        return tr("WriteToPendingRead");
//        return "Запись в области ЗУ, содержащие считанные данные, прием\
// которых не подтвержден! Информация с предыдущих съемок может быть потеряна!";
    case ReadOfReaded:
        return tr("ReadOfReaded");
//        return "Повторное считывание из области ЗУ!\
// Возможно дублирование передаваемой информации!";
    case ReadOfEmpty:
        return tr("ReadOfEmpty");
//        return "Считывание из области ЗУ,\
// не содержащей данные!";
    case WriteToNotEmpty:
        return tr("WriteToNotEmpty");
//        return "Запись в неочищенные области ЗУ! Необходима предварительная очистка!";
    case EmptyOfNotRead:
        return tr("EmptyOfNotRead");
//        return "Очистка областей ЗУ, содержащих еще не считанные данные! Информация с предыдущих съемок будет потеряна!";
    case EmptyOfPendingRead:
         return tr("EmptyOfPendingRead");
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

void MGridScene::addUnit(const KaMemoryPart &part)
{
    MGridUnit* p_memUnit = new MGridUnit(m_memoryWidget);
    p_memUnit->setState(part.state());
    p_memUnit->setId(part.id());
    p_memUnit->addItems(part.start(),part.finish());

    addUnit(p_memUnit);
}


MGridWidget *MGridScene::widget() const
{
    return m_memoryWidget;
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
    m_units.append(p_memUnit);
}

void MGridScene::removeUnit(MGridUnit *p_memUnit)
{
    if(!p_memUnit)
        return;

    m_units.removeOne(p_memUnit);

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

long MGridScene::finishHighlight() const
{
    return m_startHighlight + m_lengthHighlight - 1;
}


void MGridScene::setStartHighlight(long startHighlight)
{
    if(startHighlight==m_startHighlight)
        return;
    if(m_highlightStyle&highlightedItems)
        m_highlightMode = true;
    if(startHighlight+m_lengthHighlight>memorySize())
        return;
    m_startHighlight = startHighlight;
    update();
    emit startHighlightChanged(m_startHighlight);
}


void MGridScene::setLengthHighlight(long lengthHighlight)
{
    m_lengthHighlight = lengthHighlight;
    emit lengthHighlightChanged(m_lengthHighlight);
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
        if(m_notReadStates.contains(itemState))
        {
            errors.append(EmptyOfNotRead);
        }
        else if (itemState == Memory::PendingRead)
        {
            errors.append(EmptyOfPendingRead);
        }
    }

    if(errorHandler(errors))
    {
        setState(from,count,Memory::Empty);
    }
}

void MGridScene::setPengingRead(long from, long count)
{

    QList<ActionErrors> errors;
    // ANALYSIS
    for(int i = 0, j = from; i<count; ++i,++j)
    {
        Memory::MemoryState itemState = m_items[j]->state();
        if(itemState == Memory::PendingRead)
        {
            errors.append(ReadOfReaded);
        }
        else if (itemState == Memory::Empty)
        {
            errors.append(ReadOfEmpty);
        }
    }

    if(errorHandler(errors))
    {
        setState(from,count,Memory::PendingRead);
    }

}

void MGridScene::setFree(long from, long count)
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

void MGridScene::setPengingWrite(long from, long count)
{

    QList<ActionErrors> errors;
    // ANALYSIS
    for(int i = 0, j = from; i<count; ++i,++j)
    {
        Memory::MemoryState itemState = m_items[j]->state();
        if(m_writeStates.contains(itemState))
        {
            errors.append(WriteToNotRead);
            errors.append(WriteToNotEmpty);
        }
        else if (itemState == Memory::PendingRead)
        {
            errors.append(WriteToPendingRead);
            errors.append(WriteToNotEmpty);
        }
        else if (itemState == Memory::Free)
        {
            errors.append(WriteToNotEmpty);
        }
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

    emit memoryChanged();
}

void MGridScene::clear(long from, long count)
{
    if(from+count>=memorySize())
    {
        qDebug() <<"MemoryScene::free";
        count = memorySize()-from-1;
    }
    QList<MGridUnit*> memoryUnits = affectedUnits(from,from+count-1);
    count-= freedCount(from,from+count-1);

    foreach(MGridUnit* unit, memoryUnits)
    {
        count-=unit->removeItems(qMax(unit->start(),from),count);
    }
    qDebug() << "MemoryScene::free";
}

MGridItem *MGridScene::itemAtPos(const QPointF &pos) const
{
    QList<QGraphicsItem*> itemsAtPos = items(pos);
    MGridItem * p_mem = NULL;

    foreach(QGraphicsItem* itemAtPos,itemsAtPos)
    {
        if((p_mem=dynamic_cast<MGridItem*>(itemAtPos)))
            break;
    }
    return p_mem;
}

MGridUnit *MGridScene::unitAtPos(const QPointF &pos) const
{
    QList<QGraphicsItem*> itemsAtPos = items(pos);
    MGridUnit * p_mem = NULL;

    foreach(QGraphicsItem* itemAtPos,itemsAtPos)
    {
        if((p_mem=dynamic_cast<MGridUnit*>(itemAtPos)))
            break;
    }
    return p_mem;
}

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
    return m_memory.memorySize();
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
    foreach(MGridItem* item,m_items)
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




QList<MemoryState> MGridScene::m_notReadStates,
                    MGridScene::m_writeStates,
                    MGridScene::m_errorStates;



MGridScene::Initializer::Initializer()
{
    m_notReadStates.append(Memory::Recorded);
    m_notReadStates.append(Memory::PendingRead);
    m_notReadStates.append(Memory::PendingWrite);

    m_writeStates.append(Memory::Recorded);
    m_writeStates.append(Memory::PendingWrite);

    m_errorStates.append(Memory::ErrorRead);
    m_errorStates.append(Memory::ErrorWrite);
}
