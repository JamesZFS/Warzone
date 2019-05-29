#include "soldier.h"
//#include <Box2D/Box2D.h>
#include <QPainter>

inline static qreal toDegrees(qreal rad)
{
    return 180.0 * rad / M_PI;
}

Soldier::Soldier(Side side_, int life_, double power_, b2Body *body_) :
    side(side_), life(life_), power(power_), body(body_),
    fixture(body->GetFixtureList()), radius(fixture->GetShape()->m_radius)
{
    Q_ASSERT(life > 0);
    Q_ASSERT(fixture && fixture->GetNext() == nullptr);
}

b2Body *Soldier::getBody() const
{
    return body;
}

void Soldier::move(const b2Vec2 &strength)
{
    body->ApplyLinearImpulse(10 * strength, body->GetWorldCenter(), true);
}

QRectF Soldier::boundingRect() const
{
    return QRectF(-radius, -radius, 2 * radius, 2 * radius);
}

void Soldier::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_ASSERT(side != e_NONE);
    painter->setPen(Qt::NoPen);
    painter->setBrush(side == e_RED ? Qt::red : Qt::black);
    painter->drawEllipse(-0.8 * radius, -1.0 * radius, 1.6 * radius, 2.0 * radius);
}

void Soldier::advance(int phase)
{
    if (phase == 0) return;
    setB2Pos(body->GetPosition());
    setRotation(toDegrees(body->GetAngle()));   // notice liquidfun uses rad angle
}

double Soldier::getPower() const
{
    return power;
}

void Soldier::setPower(double value)
{
    power = value;
}

int Soldier::getLife() const
{
    return life;
}

void Soldier::setLife(int value)
{
    life = value;
}
