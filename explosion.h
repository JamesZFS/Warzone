#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <Box2D/Box2D.h>


// this is an inside class
class Explosion : public b2QueryCallback
{
public:
    Explosion(const b2Body *self, b2Vec2 center, float32 rad, float32 power);

    // b2QueryCallback interface
    bool ReportFixture(b2Fixture *fixture) override;

private:
    const b2Body* const m_self;
    const b2Vec2 m_center;
    const float32 m_rad;
    const float32 m_power;
};

#endif // EXPLOSION_H
