#ifndef MGRID_SCENE_H
#define MGRID_SCENE_H

#include <ganttmodel_global.h>

#include "mgrid_item.h"
#include "mgrid_widget.h"
#include "mgrid_unit.h"

#include "kamemory.h"

#include <QGraphicsScene>

#include <QList>
#include <QMap>

#include <QLabel>

QT_FORWARD_DECLARE_CLASS(QStatusBar);

class MGridInteractiveUnit;
class KaMemory;


class GANTTMODELSHARED_EXPORT MGridScene : public QGraphicsScene
{
    Q_OBJECT

public:
    MGridScene( QObject * parent = 0 );
    ~MGridScene();

    qreal spacing() const;
    void setSpacing(const qreal &spacing);
    void setMemory(const KaMemory& kaMemory/*const QList<MemoryItemPresentation>& mem_it_list,long memorySize*/);
    MGridUnit* newUnit(int unitId = -1);
    MGridUnit* unit(int unitId) const;

    void setItemInfo(const QString& text);
    void setUnitInfo(const QString& text);

    int itemPerRow() const;
    void setItemPerRow(int newItemPerRow);

    qreal itemEdge() const;
    void setItemEdge(qreal newEdgeLength);

    qreal itemBorder() const;
    void setItemBorder(qreal itemBorder);

    long memorySize() const;
    void setMemorySize(long memorySize);

    void viewResized(QSizeF viewSize);

    void showInteractiveRange(long start, long finish);
    void hideInteractiveRange();

    qreal itemSize() const;

    bool highlightMode() const;
    void setHighlightMode(bool highlightMode);

    void setLengthHighlight(long lengthHighlight);
    void setStartHighlight(long startHighlight);

    long startHighlight() const;
    long finishHighlight() const;
    long lengthHighlight() const;

    bool interactiveHighlight() const;
    void setInteractiveHighlight(bool interactiveHighlight);

    KaMemory memory();

public slots:
    void transformChanged(const QTransform& transform);

    // +ACTIONS
    /*
     *  Empty,          ///< Пустая память - белый
     *  Free,           ///< Свободно - серый
     *  Recorded,       ///< Записано - зелёный
     *  PendingWrite,   ///< Ожидание съёмки - оранжевый
     *  PendingRead,    ///< Ожидание сброса - жёлтый
     *  ErrorWrite,     ///< Ошибка записи - сиреневый
     *  ErrorRead,      ///< Ошибка сброса - красный
    */
    void setEmpty(long from, long count);
    void setFree(long from, long count);
    void setPengingRead(long from, long count);
    void setPengingWrite(long from, long count);
    
    // -ACTIONS
    
private:
    void setState(long from, long count, MemoryState state);
    void clear(long from,long count);

signals:
    void startHighlightChanged(long val);
    void lengthHighlightChanged(long val);
    void interactiveHighlightChanged(bool val);
    void intervalHasSelected();
    void intervalSelectionStarted();
    void memoryChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

//    void drawForeground(QPainter *painter, const QRectF &rect);

private:
    void clearShownUnits();
    void updateShownUnits();
    void clearLastSelected();
    void setLastSelected(MGridItem* p_mem);
    int  itemIndex(QGraphicsItem* item) const;
    void memoryStatusUpdate(const QRectF& rect = QRectF());
    bool inHighlightRange(long index) const;
    QList<MGridUnit*> affectedUnits(long from, long to) const;
    long freedCount(long from, long to) const;
    void updateMemory();
    void updateParenthesis();
    void addUnit(MGridUnit* p_memUnit);
    void removeUnit(MGridUnit* p_memUnit);
    MGridWidget *widget() const;

    // +WARNINGS
private:
    enum ActionErrors
    {
        WriteToNotRead = 1,
        WriteToPendingRead = 2,
        ReadOfReaded = 3,
        ReadOfEmpty = 4,
        WriteToNotEmpty = 5,
        EmptyOfNotRead = 6,
        EmptyOfPendingRead = 7,
        ActionErrors_count
    };

    static QList<MemoryState> m_notReadStates,
                              m_writeStates,
                              m_errorStates;
    struct Initializer
    {
        Initializer();
    };
    static Initializer initializerGuard;

    static QString warning(ActionErrors id);
    bool errorHandler(QList<ActionErrors>& errors) const;

    // -WARNINGS

private:

    bool m_highlightMode;
    bool m_interactiveHighlight;

    long m_memorySize;

    long m_startHighlight;
    long m_lengthHighlight;

    KaMemory m_memory;
private:

    int m_lastSelectedIndex;
    MGridItem* m_lastSelected;
    MGridWidget* m_memoryWidget;

    MGridInteractiveUnit* m_interactiveUnit;


    QList<MGridUnit*> m_units;
    QList<MGridItem*> m_items;
    QMap<int , MGridUnit*> m_unit_by_unitId;

    friend class MGridInteractiveUnit;

    friend bool MGridItem::isHighlighted() const;
    friend void MGridItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    friend void MGridItem::setParentUnit(MGridUnit*);
    friend void MGridUnit::setItems();
    friend long MGridUnit::removeItems(long,long);
};

#endif // MGRID_SCENE_H
