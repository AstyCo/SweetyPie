#ifndef MGRID_SCENE_H
#define MGRID_SCENE_H

#include <ganttmodel_global.h>

#include "kamemory.h"

#include <QGraphicsScene>

#include <QList>
#include <QMap>

#include <QLabel>

QT_FORWARD_DECLARE_CLASS(QStatusBar);

class MGridInteractiveUnit;
class MGridItem;
class MGridUnit;
class MGridWidget;


class GANTTMODELSHARED_EXPORT MGridScene : public QGraphicsScene
{
    Q_OBJECT

public:
    MGridScene( QObject * parent = 0 );
    ~MGridScene();

    /// Инициализирует память
    void setMemory(const KaMemory& kaMemory);

    /// Расстояние между блоками
    qreal spacing() const;
    void setSpacing(const qreal &spacing);

    /// Устанавливает размер блоков
    qreal itemEdge() const;
    void setItemEdge(qreal newEdgeLength);

    /// Рамка блоков
    qreal itemBorder() const;
    void setItemBorder(qreal itemBorder);

    enum HighlightStyle
    {
        bordersAround = 0x1,
        highlightedItems = 0x2,
    };

    void setLengthHighlight(long lengthHighlight);
    void setStartHighlight(long startHighlight);

    long startHighlight() const;
    long finishHighlight() const;
    long lengthHighlight() const;

    KaMemory memory();

    HighlightStyle highlightStyle() const;
    void setHighlightStyle(int highlightStyle);

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


signals:
    void startHighlightChanged(long val);
    void lengthHighlightChanged(long val);
    void interactiveHighlightChanged(bool val);
    void intervalHasSelected();
    void intervalSelectionStarted();
    void memoryChanged();

    void itemInfoChanged(const QString& text);
    void unitInfoChanged(const QString& text);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);


private:
    void setState(long from, long count, MemoryState state);
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
    MGridUnit* newUnit();
    void removeUnit(MGridUnit* p_memUnit);
    void addUnit(const KaMemoryPart &part);
    MGridWidget *widget() const;
    void setItemInfo(const QString& text);
    void setUnitInfo(const QString& text);
    void viewResized(QSizeF viewSize);
    bool interactiveHighlight() const;
    void setInteractiveHighlight(bool interactiveHighlight);
    void showInteractiveRange(long start, long finish);
    void hideInteractiveRange();
    qreal itemSize() const;
    bool highlightMode() const;
    void setHighlightMode(bool highlightMode);
    long memorySize() const;
    int itemPerRow() const;
    void setItemPerRow(int newItemPerRow);
    void clear(long from,long count);
    MGridItem *itemAtPos(const QPointF& pos) const;
    MGridUnit *unitAtPos(const QPointF& pos) const;
    bool isMouseOverUnit(MGridUnit* p_unit) const;
    bool isMouseOverItem(MGridItem* p_item) const;
    MGridItem *mouseOverItem() const;
    void setMouseOverItem(MGridItem *mouseOverItem);
    MGridUnit *mouseOverUnit() const;
    void setMouseOverUnit(MGridUnit *mouseOverUnit);

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

    HighlightStyle m_highlightStyle;

    bool m_highlightMode;
    bool m_interactiveHighlight;

    long m_startHighlight;
    long m_lengthHighlight;

    KaMemory m_memory;
private:

    MGridItem *m_mouseOverItem;
    MGridUnit *m_mouseOverUnit;
    long m_lastSelectedIndex;
    MGridItem* m_lastSelected;
    MGridWidget* m_memoryWidget;

    MGridInteractiveUnit* m_interactiveUnit;


    QList<MGridUnit*> m_units;
    QList<MGridItem*> m_items;

    friend class MGridInteractiveUnit;
    friend class MGridItem;
    friend class MGridUnit;
    friend class MGridWidget;

    friend class MemoryView;
};

#endif // MGRID_SCENE_H
