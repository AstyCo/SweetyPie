#ifndef MGRID_SCENE_H
#define MGRID_SCENE_H

#include <ganttproject_global.h>

#include "memory.h"

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
    void setMemory(QSharedPointer<KaMemory>& kaMemory);
    QSharedPointer<KaMemory> &memory();
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
        noHighlightStyle = 0,
        bordersAround = 0x1,
        highlightedItems = 0x2,
        highlightedArea = 0x4
    };

    /// Устанавливает выделенный участок с началом start и длиной length
    bool setSelected(long start, long length);
    /// Устанавливает длину выделяемого участка
    bool setLengthSelection(long lengthSelection);

    /// Начало выделяемого участка
    long startSelection() const;
    /// Длина выделяемого участка
    long lengthSelection() const;

    /// Устанавливает стиль выделения (с рамкой/ с подсвечиванием/ комбинированный)
    void setHighlightStyle(int highlightStyle);

    /// Возвращает список блоков, пересекающих выбранный интервал
    QList<QSharedPointer<MemoryPart> > crossingParts() const;

    /// Возвращает строку, представляющую блока памяти в унифицированном виде (ex. 0x0000-0xFFFF ...)
    QString toAdress(long start,long finish);

    /// Устанавливает режим выделения области памяти
    void setHighlightMode(bool highlightMode);

    /// Отвечает за возможность изменения выбранной области ( false - конец выделения )
    void setInteractiveHighlight(bool interactiveHighlight);

    /// Устанавливает какую часть памяти можно модифицировать
    void setLimits(long min, long max);

    enum SelectionMode
    {
        noSelection, ///< выделение недоступно
        areaSelection, ///< выделение области памяти, определяемой пользователем
        positionSelection, ///< выделение области памяти фиксированного размера

        SelectionMode_count
    };

    /// Устанавливает режим выделения (см. SelectionMode)
    void setSelectionMode(const SelectionMode &selectionMode);

    SelectionMode selectionMode() const;

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

    void setKaMemoryPart(const MemoryPart& part);

    /// Устанавливает статус по адресам
    QSharedPointer<MemoryPart>  setEmpty(long from, long count);
    QSharedPointer<MemoryPart>  setFree(long from, long count);
    QSharedPointer<MemoryPart>  setPendingRead(long from, long count);
    QSharedPointer<MemoryPart>  setPendingWrite(long from, long count);


    /// Устанавливает статус по текущему выделению
    QSharedPointer<MemoryPart>  setEmpty();
    QSharedPointer<MemoryPart>  setFree();
    QSharedPointer<MemoryPart>  setPendingRead();
    QSharedPointer<MemoryPart>  setPendingWrite();
    void clear();

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
    /// Устанавливает начало выделяемого участка
    bool setStartSelection(long startSelection);
    QSharedPointer<MemoryPart> setState(long from, long count, MemoryPart::MemoryState state);
    void clearShownUnits();
    void updateShownUnits();
    void clearLastSelected();
    void setLastSelected(MGridItem* p_mem);
    int  itemIndex(QGraphicsItem* item) const;
    void memoryStatusUpdate(const QRectF& rect = QRectF());
    bool inHighlightRange(long index) const;
    long freedCount(long from, long to) const;
//    void updateMemory();
    void updateParenthesis();
    void addUnit(MGridUnit* p_memUnit);
    MGridUnit* newUnit();
    void removeUnit(MGridUnit* p_memUnit);
    void addUnit(QSharedPointer<MemoryPart> part);

    void setItemInfo(const QString& text);
    void setUnitInfo(const QString& text);
    void viewResized(QSizeF viewSize);
    bool interactiveHighlight() const;

//    void updateInteractiveRange();
    void updateInteractiveRange(long start, long length);
    qreal itemSize() const;
    bool highlightMode() const;

    int itemPerRow() const;
    void setItemPerRow(int newItemPerRow);
    void clearMemory(long from,long count);
    MGridItem *itemAtPos(const QPointF& pos) const;
    bool isMouseOverUnit(MGridUnit* p_unit) const;
    bool isMouseOverItem(MGridItem* p_item) const;
    MGridItem *mouseOverItem() const;
    void setMouseOverItem(MGridItem *mouseOverItem);
    MGridUnit *mouseOverUnit() const;
    void setMouseOverUnit(MGridUnit *mouseOverUnit);
    void clearItems();
    void clearUnits();
    HighlightStyle highlightStyle() const;
    QList<MGridUnit*> crossingParts(long start, long length) const;
    void setupMatrix(const QVector<MGridItem*>& items);
    void clearMouseOver();

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

    static QList<MemoryPart::MemoryState> m_notReadStates,
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

    QSharedPointer<KaMemory> m_memory;

    qreal m_itemEdge;
    qreal m_itemBorder;
    int m_itemPerRow;
    SelectionMode m_selectionMode;
    long m_min,m_max;
private:

    bool m_leftButtonPressed;
    MGridItem *m_mouseOverItem;
    MGridUnit *m_mouseOverUnit;
    MGridItem* m_lastSelected;
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
