#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <Box2D/Box2D.h>

// this is an internal class
class Explosion : public b2QueryCallback
{
protected:
    Explosion(const b2Body *self, const b2Vec2 &center, float32 rad, float32 power);
    ~Explosion() = default;

public:
    // b2QueryCallback interface
    bool ReportFixture(b2Fixture *fixture) override;    // explosion

    // helper function
    static void create(b2Body *self, float32 rad, float32 power);

private:
    const b2Body* const m_self;
    const b2Vec2 m_center;
    const float32 m_rad;
    const float32 m_power;  // max damage
};


#endif // EXPLOSION_H
