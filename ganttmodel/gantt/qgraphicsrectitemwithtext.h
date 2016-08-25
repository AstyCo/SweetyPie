#ifndef QGRAPHICSRECTITEMWITHTEXT_H
#define QGRAPHICSRECTITEMWITHTEXT_H

#include <QGraphicsRectItem>
#include <QString>

class QGraphicsRectItemWithText : public QGraphicsRectItem
{
public:
    QGraphicsRectItemWithText(const QString& text,const QRectF & rect, QGraphicsItem * parent = 0,QGraphicsScene *scene = 0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QColor textColor() const;
    void setTextColor(const QColor &textColor);

private:
    QString m_text;
    QColor m_textColor;
};

#endif // QGRAPHICSRECTITEMWITHTEXT_H
