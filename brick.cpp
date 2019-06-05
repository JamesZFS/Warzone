#include "brick.h"

#include <QPainter>
#include <QtMath>


static const auto friction = GameConsts::brick_friction;
static const auto density1 = GameConsts::brick_density1;
static const auto density2 = GameConsts::brick_density2;
static const auto life1 = GameConsts::brick_life1;
static const auto life2 = GameConsts::brick_life2;
static const qreal sqrt_3_2 = qSqrt(3) / 2;
static const QBrush default_brush(Qt::gray);

Brick::Brick(b2Body *body, int life, const QBrush *brush) :
    AnimateActor(body, life), m_brush(brush)
{
    if (brush == nullptr) m_brush = &default_brush;
    m_body->SetLinearDamping(0.2);
    m_body->SetAngularDamping(0.2);
    m_bbox = findBodyBound();
}

Brick *Brick::createRect(b2Body *body, qreal w, qreal h, const QBrush *brush)
{
    Q_ASSERT(!body->GetFixtureList());
    b2PolygonShape shape;
    shape.SetAsBox(w/2, h/2);
    body->CreateFixture(&shape, density1)->SetFriction(friction);
    return new Brick(body, life1, brush);
}

Brick *Brick::createBlock(b2Body *body, qreal size, const QBrush *brush)
{
    return createRect(body, size, size, brush);
}

Brick *Brick::createCircle(b2Body *body, qreal rad, const QBrush *brush)
{
    Q_ASSERT(!body->GetFixtureList());
    b2CircleShape shape;
    shape.m_radius = rad;
    body->CreateFixture(&shape, density1)->SetFriction(friction);
    return new Brick(body, life1, brush);
}

Brick *Brick::createTriangle(b2Body *body, qreal size, const QBrush *brush)
{
    Q_ASSERT(!body->GetFixtureList());
    b2PolygonShape shape;
    const b2Vec2 tri[3] = { b2Vec2(-size/2, 0), b2Vec2(+size/2, 0), b2Vec2(0, sqrt_3_2*size) };
    body->CreateFixture(&shape, density1)->SetFriction(friction);
    return new Brick(body, life1, brush);
}

Brick *Brick::createU(b2Body *body, qreal width, qreal height, qreal thickness, const QBrush *brush)
{
    Q_ASSERT(!body->GetFixtureList());
    b2PolygonShape shape;
    shape.SetAsBox(width/2, thickness/2);
    body->CreateFixture(&shape, density2)->SetFriction(friction);
    shape.SetAsBox(thickness/2, height/2, b2Vec2(-width/2, height/2), 0);
    body->CreateFixture(&shape, density2)->SetFriction(friction);
    shape.SetAsBox(thickness/2, height/2, b2Vec2(+width/2, height/2), 0);
    body->CreateFixture(&shape, density2)->SetFriction(friction);
    return new Brick(body, life2, brush);
}

void Brick::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(*m_brush);
    drawFixtures(painter, m_body->GetFixtureList());
}
