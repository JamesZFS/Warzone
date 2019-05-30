#include "soldier.h"
//#include <Box2D/Box2D.h>
#include <QPainter>

inline static qreal toDegrees(qreal rad)
{
    return 180.0 * rad / M_PI;
}

Soldier::Soldier(Side side_, int life_, double power_, b2Body *body_) :
    Actor(body_, 0.5, 1.0),
    side(side_), life(life_), power(power_),
    fixture(body->GetFixtureList()), radius(10 * fixture->GetShape()->m_radius)
{
    Q_ASSERT(life > 0);
    b_box = QRectF(-radius - 1, -radius - 1, 2 * (radius + 1), 2 * (radius + 1));
}

b2Body *Soldier::getBody() const
{
    return body;
}

void Soldier::move(const b2Vec2 &strength)
{
    body->ApplyLinearImpulse(5 * strength, body->GetWorldCenter(), true);
}

qreal Soldier::getRadius() const
{
    return radius;
}

QRectF Soldier::boundingRect() const
{
    return b_box;
}

void Soldier::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_ASSERT(side != e_NONE);
    painter->setPen(Qt::NoPen);
    painter->setBrush(side == e_RED ? Qt::red : Qt::black);
//    painter->drawEllipse(-0.8 * radius, -1.0 * radius, 1.6 * radius, 2.0 * radius);
//    painter->drawRect(-radius, -radius, 2 * radius, 2 * radius);
    painter->drawEllipse(-radius, -radius, 2.0 * radius, 2.0 * radius);
//    painter->drawEllipse(0, 0, 2.0 * radius, 2.0 * radius);
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
