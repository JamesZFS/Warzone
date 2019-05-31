#ifndef FRONTSIGHT_H
#define FRONTSIGHT_H

#include <QGraphicsItem>
#include <QPropertyAnimation>


class QPropertyAnimation;

class FrontSight : public QGraphicsItem
{
public:
    FrontSight();

protected:
    QRectF m_bbox;
    const QPointF m_center;

    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

class ChargingFrontSight : public QObject, public FrontSight
{
    Q_OBJECT
    Q_PROPERTY(qreal charge READ charge WRITE setCharge)
public:
    ChargingFrontSight();

private:
    qreal m_charge;
    QPropertyAnimation m_anim;

public:
    // QGraphicsItem interface
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    qreal charge() const;
    void setCharge(qreal charge);

    void startAnimation();
    void stopAnimation();
};

#endif // FRONTSIGHT_H
