#ifndef KAMEMORYABSTRACTPART_H
#define KAMEMORYABSTRACTPART_H

#include <QGraphicsObject>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>



class KaMemoryAbstractPart : public QGraphicsObject
{
    Q_OBJECT
protected:
    int _begin;
    int _end;

    int y_pos;
    int height;


    qreal xByVal(int val) const;
    int valByX(qreal x) const;
    qreal width() const;

public:
    explicit KaMemoryAbstractPart(QGraphicsItem *parent = 0);
    KaMemoryAbstractPart(const KaMemoryAbstractPart &part);
    ~KaMemoryAbstractPart();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)=0;
    QRectF boundingRect() const;

    int begin() const;  
    int end() const;
    int size() const;


    int memorySize() const;


public slots:
    void setBegin(int begin);
    void setEnd(int end);

signals:
    void valueChanged(int,int);
    void beginChanged(int);
    void endChanged(int);
};

#endif // KAMEMORYPICKER_H
