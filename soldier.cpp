#include "soldier.h"
//#include <Box2D/Box2D.h>

Soldier::Soldier(int life_, double power_, b2Body *body_) :
    life(life_), power(power_), body(body_), fixture(body->GetFixtureList())
{
    assert(life > 0);
    assert(fixture && fixture->GetNext() == nullptr);
}

b2Body *Soldier::getBody() const
{
    return body;
}

void Soldier::move(b2Vec2 delta)
{
    body->ApplyLinearImpulse(10 * delta, body->GetWorldCenter(), true);
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
