#include "brick.h"

#include <QPainter>
#include <QtMath>


static const auto friction = GameConsts::brick_friction;
static const auto density1 = GameConsts::brick_density1;
static const auto density2 = GameConsts::brick_density2;
static const auto life1 = GameConsts::brick_life1;
static const auto life2 = GameConsts::brick_life2;
static const qreal sqrt_3 = qSqrt(3);

Brick::Brick(b2Body *body, int life, const QColor &color) :
    AnimateActor(body, life), m_color(color)
{
    m_body->SetLinearDamping(0.1);
    m_body->SetAngularDamping(0.1);
    m_bbox = findBodyBound();
}

Brick *Brick::createBlock(b2Body *body, qreal size, const QColor &color)
{
    Q_ASSERT(!body->GetFixtureList());
    b2PolygonShape shape;
    shape.SetAsBox(size, size);
    body->CreateFixture(&shape, density1)->SetFriction(friction);
    return new Brick(body, life1, color);
}

Brick *Brick::createCircle(b2Body *body, qreal rad, const QColor &color)
{
    Q_ASSERT(!body->GetFixtureList());
    b2CircleShape shape;
    shape.m_radius = rad;
    body->CreateFixture(&shape, density1)->SetFriction(friction);
    return new Brick(body, life1, color);
}

Brick *Brick::createTriangle(b2Body *body, qreal size, const QColor &color)
{
    Q_ASSERT(!body->GetFixtureList());
    b2PolygonShape shape;
    const b2Vec2 tri[3] = { b2Vec2(-size/2, 0), b2Vec2(size/2, 0), b2Vec2(0, 0.5*sqrt_3*size) };
    body->CreateFixture(&shape, density1)->SetFriction(friction);
    return new Brick(body, life1, color);
}

Brick *Brick::createU(b2Body *body, qreal width, qreal height, const QColor &color)
{

}

void Brick::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(m_color);
    drawFixtures(painter, m_body->GetFixtureList());
}
