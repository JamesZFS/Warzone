#include "frontsight.h"

#include <QPainter>

FrontSight::FrontSight(const QPointF &center) :
    m_angle(0), m_bbox(20, -10, 20, 20), m_center(center)
{

}

qreal FrontSight::angle() const
{
    return m_angle;
}

void FrontSight::setAngle(const qreal &angle)
{
    m_angle = angle;
    setRotation(angle);
}

QRectF FrontSight::boundingRect() const
{
    return m_bbox;
}

void FrontSight::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPolygonF tri;
    painter->save();
    painter->translate(30, 0);
    tri << QPointF(0, 3) << QPointF(-2, 10) << QPointF(2, 10);
    painter->setBrush(QColor(180, 190, 250));
    painter->drawPolygon(tri);
    painter->rotate(90);
    painter->drawPolygon(tri);
    painter->rotate(90);
    painter->drawPolygon(tri);
    painter->rotate(90);
    painter->drawPolygon(tri);
    painter->restore();
    setPos(m_center);
}
