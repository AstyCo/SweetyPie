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

    Q_PROPERTY(long startHighlight READ startHighlight WRITE setStartHighlight
                NOTIFY startHighlightChanged )
    Q_PROPERTY(long lengthHighlight READ lengthHighlight WRITE setLengthHighlight
                NOTIFY lengthHighlightChanged )
    Q_PROPERTY(bool interactiveHighlight READ interactiveHighlight WRITE setInteractiveHighlight
                NOTIFY interactiveHighlightChanged )


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

    void updateParenthesis();

    friend bool MGridtem::isHighlighted() const;
    friend void MGridtem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    friend void MGridUnit::setItems();

    bool highlightMode() const;
    void setHighlightMode(bool highlightMode);

    void setLengthHighlight(long lengthHighlight);
    void setStartHighlight(long startHighlight);

    long startHighlight() const;
    long lengthHighlight() const;

    MGridWidget *widget() const;

    void addUnit(MGridUnit* p_memUnit);
    void removeUnit(MGridUnit* p_memUnit);

    QList<MGridUnit*> affectedUnits(long from, long to) const;
    long freedCount(long from, long to) const;

    bool interactiveHighlight() const;
    void setInteractiveHighlight(bool interactiveHighlight);

    KaMemory memory() const;

public slots:
    void transformChanged(const QTransform& transform);

    // +ACTIONS

    void setEmpty(long from, long count);
    void setRead(long from, long count);
    void setAvailable(long from, long count);
    void setWritten(long from, long count);
    
    // -ACTIONS
    
private:
    void setState(long from, long count, MemoryState state);
    void clear(long from,long count);

signals:
    void startHighlightChanged(long val);
    void lengthHighlightChanged(long val);
    void interactiveHighlightChanged(bool val);

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
    void setLastSelected(MGridtem* p_mem);
    int  itemIndex(QGraphicsItem* item) const;
    void memoryStatusUpdate(const QRectF& rect = QRectF());
    bool inHighlightRange(long index) const;

    // +WARNINGS
private:
    enum ActionErrors
    {
        WriteToWritten = 1,
        WriteToRead = 2,
        ReadFromRead = 3,
        ReadFromFreed = 4,
        WriteToNotFreed = 5,
        FreeOfWritten = 6,
        FreeOfRead = 7,
        ActionErrors_count
    };


    static QString warning(ActionErrors id);
    bool errorHandler(QList<ActionErrors>& errors) const;

    // -WARNINGS

private:
    int m_lastSelectedIndex;
    MGridtem* m_lastSelected;
    MGridWidget* m_memoryWidget;

    MGridInteractiveUnit* m_interactiveUnit;


    QList<MGridUnit*> m_units;
    QList<MGridtem*> m_items;

    bool m_interactiveHighlight;

    QMap<int , MGridUnit*> m_unit_by_unitId;

    long m_memorySize;

    bool m_highlightMode;
    long m_startHighlight;
    long m_lengthHighlight;

    KaMemory m_memory;

    friend class MGridInteractiveUnit;
};

#endif // MGRID_SCENE_H