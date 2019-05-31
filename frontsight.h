#ifndef FRONTSIGHT_H
#define FRONTSIGHT_H

#include <QGraphicsItem>



class FrontSight : public QGraphicsItem
{
public:
    FrontSight(const QPointF &center);

    qreal angle() const;
    void setAngle(const qreal &angle);

private:
    qreal m_angle;

    const QRectF m_bbox;

    const QPointF m_center;

    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // FRONTSIGHT_H
