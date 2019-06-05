#include "frontsight.h"

#include <QPainter>


FrontSight::FrontSight() :
    m_bbox(40, -20, 20, 40)
{

}

QRectF FrontSight::boundingRect() const
{
    return m_bbox;
}

void FrontSight::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPolygonF tri;
    tri << QPointF(0, 3) << QPointF(-3, 15) << QPointF(3, 15);
    painter->save();
    painter->translate(50, 0);
    painter->setBrush(QColor(180, 190, 250));
    painter->rotate(45);
    painter->drawPolygon(tri);
    painter->rotate(90);
    painter->drawPolygon(tri);
    painter->rotate(90);
    painter->drawPolygon(tri);
    painter->rotate(90);
    painter->drawPolygon(tri);
    painter->restore();
}

ChargingFrontSight::ChargingFrontSight() :
    FrontSight(), m_anim(this, "charge")
{
    m_bbox = QRectF(0, -10, 30, 20);
    m_anim.setStartValue(0.0);
    m_anim.setEndValue(1.0);
    m_anim.setDuration(1000);   // 1s
}

void ChargingFrontSight::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPolygonF p;
    p << QPointF(0, 3) << QPointF(0, -3)
      << QPointF(30 * m_charge, -7 * m_charge - 3)
      << QPointF(30 * m_charge, 7 * m_charge + 3);
    static QLinearGradient g(0, -10, 30, 20);
    g.setColorAt(0.0, QColor(230, 230, 10));
    g.setColorAt(1.0, QColor(240, 10, 10));
    painter->setBrush(g);
    painter->drawPolygon(p);
}

qreal ChargingFrontSight::charge() const
{
    return m_charge;
}

void ChargingFrontSight::setCharge(qreal charge)
{
    m_charge = charge;
    update();
}

void ChargingFrontSight::startAnimation()
{
    m_anim.start();
}

void ChargingFrontSight::stopAnimation()
{
    m_anim.stop();
}
