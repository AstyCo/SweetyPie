#ifndef MGRID_SCENE_H
#define MGRID_SCENE_H

#include <ganttmodel_global.h>

#include "kamemory.h"

#include <QGraphicsScene>

#include <QList>
#include <QVector>

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
    KaMemory memory();
    /// Возвращает размер памяти
    long memorySize() const;

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

    /// Устанавливает длину выделяемого участка
    void setLengthHighlight(long lengthHighlight);
    /// Устанавливает начало выделяемого участка
    void setStartHighlight(long startHighlight);

    /// Начало выделяемого участка
    long startHighlight() const;
    /// Конец выделяемого участка
    long finishHighlight() const;
    /// Длина выделяемого участка
    long lengthHighlight() const;

    /// Устанавливает стиль выделения (с рамкой/ с подсвечиванием/ комбинированный)
    void setHighlightStyle(int highlightStyle);

    /// Возвращает список блоков, пересекающих выбранный интервал
    QList<KaMemoryPart> crossingParts() const;

    /// Возвращает строку, представляющую блока памяти в унифицированном виде (ex. 0x0000-0xFFFF ...)
    QString toAdress(long start,long finish);


public slots:

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

    void setKaMemoryPart(const KaMemoryPart& part);

    /// Устанавливает статус по адресам
    KaMemoryPart setEmpty(long from, long count);
    KaMemoryPart setFree(long from, long count);
    KaMemoryPart setPendingRead(long from, long count);
    KaMemoryPart setPendingWrite(long from, long count);


    /// Устанавливает статус по текущему выделению
    KaMemoryPart setEmpty();
    KaMemoryPart setFree();
    KaMemoryPart setPendingRead();
    KaMemoryPart setPendingWrite();

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
    void clear();
    KaMemoryPart setState(long from, long count, MemoryState state);
    void clearShownUnits();
    void updateShownUnits();
    void clearLastSelected();
    void setLastSelected(MGridItem* p_mem);
    int  itemIndex(QGraphicsItem* item) const;
    void memoryStatusUpdate(const QRectF& rect = QRectF());
    bool inHighlightRange(long index) const;
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
    int itemPerRow() const;
    void setItemPerRow(int newItemPerRow);
    void clearMemory(long from,long count);
    MGridItem *itemAtPos(const QPointF& pos) const;
    MGridUnit *unitAtPos(const QPointF& pos) const; // Its better to not use this function
    bool isMouseOverUnit(MGridUnit* p_unit) const;
    bool isMouseOverItem(MGridItem* p_item) const;
    MGridItem *mouseOverItem() const;
    void setMouseOverItem(MGridItem *mouseOverItem);
    MGridUnit *mouseOverUnit() const;
    void setMouseOverUnit(MGridUnit *mouseOverUnit);
    void clearItems();
    void clearUnits();
    HighlightStyle highlightStyle() const;
    QList<MGridUnit*> crossingParts(long from, long to) const;

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

    qreal m_itemEdge;
    qreal m_itemBorder;
private:

    MGridItem *m_mouseOverItem;
    MGridUnit *m_mouseOverUnit;
    long m_lastSelectedIndex;
    MGridItem* m_lastSelected;
    MGridWidget* m_mGridWidget;

    MGridInteractiveUnit* m_interactiveUnit;


    QList<MGridUnit*> m_units;
    QVector<MGridItem*> m_items;

    friend class MGridInteractiveUnit;
    friend class MGridItem;
    friend class MGridUnit;
    friend class MGridWidget;

    friend class MemoryView;
};

#endif // MGRID_SCENE_H
