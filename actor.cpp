#include "actor.h"
#include "defs.h"
#include <Box2D/Box2D.h>
#include <QPainter>
#include <QtMath>

// Actor:
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

QRectF Actor::fromB2AABB(const b2AABB &aabb) const
{
    return QRectF(mapB2Pos(aabb.lowerBound), mapB2Pos(aabb.upperBound));
}

QRectF Actor::findBodyBound() const
{
    QRectF bound;
    for (auto fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        QRectF fix_bound;
        switch (fixture->GetType()) {
        case b2Shape::e_circle: {
            auto circle = (b2CircleShape*)fixture->GetShape();
            qreal r = 10 * circle->m_radius;
            fix_bound = QRectF(-r, -r, 2*r, 2*r);
            break;
        }
        case b2Shape::e_polygon: {
            auto polygon = (b2PolygonShape*)fixture->GetShape();
            fix_bound = fromB2Polygon(*polygon).boundingRect();
            break;
        }
        default:
            qFatal("unsupported shape in findBodyBound!");
            break;
        }
        bound = bound.united(fix_bound);
    }
    return bound + QMargins(1, 1, 1, 1);
}

void Actor::setCollisionCategory(Side side)
{
    b2Filter ft;
    ft.categoryBits = side == e_RED ? GameConsts::red_category : GameConsts::black_category;
    for (auto fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        fixture->SetFilterData(ft);
    }
}

void Actor::setCollisionFilter(Side side)
{
    b2Filter ft;
    ft.maskBits &= side == e_RED ? ~GameConsts::red_category : ~GameConsts::black_category;
    for (auto fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        fixture->SetFilterData(ft);
    }
}

void Actor::drawFixtures(QPainter *painter, const b2Fixture *fixture) const
{
    while (fixture) {
        switch (fixture->GetType()) {
        case b2Shape::e_circle: {
            auto circle = (b2CircleShape*)fixture->GetShape();
            qreal r = 10 * circle->m_radius;
            painter->drawEllipse(mapB2Pos(circle->m_p), r, r);
            break;
        }
        case b2Shape::e_polygon: {
            auto polygon = (b2PolygonShape*)fixture->GetShape();
            painter->drawPolygon(fromB2Polygon(*polygon));
            break;
        }
        default:
            qFatal("unsupported shape in drawFixtures!");
            break;
        }
        fixture = fixture->GetNext();
    }
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

// AnimateActor:
AnimateActor::AnimateActor(b2Body *body, int life) : Actor(body), m_life(life)
{
    Q_ASSERT(life > 0);
}

void AnimateActor::takeDamage(int damage)
{
    Q_ASSERT(damage >= 0);
    if (m_life > damage) {
        m_life -= damage;
        emit hurt(damage);
    }
    else {
        m_life = 0;
        emit hurt(damage);
        emit died();    // handled by system
    }
}

