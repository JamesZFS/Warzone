#include "actor.h"
#include "defs.h"
#include <Box2D/Box2D.h>
#include <QtMath>

Actor::Actor(b2Body *body) :
    m_body(body), m_bbox(-10, -10, 20, 20), m_world(body->GetWorld())
{
    setB2Pos(m_body->GetPosition());
    setB2Angle(m_body->GetAngle());
    m_body->SetUserData(this);            // link b2body to Actor
}

void Actor::setB2Pos(const b2Vec2 &b_pos)
{
    setPos(mapB2Pos(b_pos));
}

void Actor::setB2Angle(const float32 angle)
{
    setRotation(qRadiansToDegrees(angle));   // notice liquidfun uses rad angle
}

QPointF Actor::mapB2Pos(const b2Vec2 &b_pos) const   // zoom x10
{
    return QPointF(10 * b_pos.x, 10 * b_pos.y);
}

QPolygonF Actor::fromB2Polygon(const b2PolygonShape &b2_poly) const
{
    QPolygonF polygon;
    for (int i = 0; i < b2_poly.GetVertexCount(); ++i)
        polygon << mapB2Pos(b2_poly.GetVertex(i));
    return polygon;
}

b2Body *Actor::body() const
{
    return m_body;
}

void Actor::advance(int phase)
{
    if (phase == 0) return;
    setB2Pos(m_body->GetPosition());
    setB2Angle(m_body->GetAngle());
}
