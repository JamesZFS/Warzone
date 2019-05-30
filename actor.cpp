#include "actor.h"
#include <Box2D/Box2D.h>
#include "defs.h"

Actor::Actor(b2Body *body_, qreal damping, qreal friction) :
    body(body_), b_box(-10, -10, 20, 20)
{
    Q_ASSERT(body->GetFixtureList());
    body->SetLinearDamping(damping);
    body->SetAngularDamping(damping);
    for (auto fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        fixture->SetFriction(friction);
}

void Actor::setB2Pos(const b2Vec2 &b_pos)
{
    setPos(mapB2Pos(b_pos));
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
