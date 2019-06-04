#ifndef EXPLOSIONCALLBACL_H
#define EXPLOSIONCALLBACL_H

#include <Box2D/Box2D.h>
#include <QGraphicsObject>
#include <QPropertyAnimation>

// this is an internal class
class ExplosionCallback : public b2QueryCallback
{
protected:
    ExplosionCallback(const b2Body *self, const b2Vec2 &center, float32 rad, float32 power);
    ~ExplosionCallback() = default;

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

#endif // EXPLOSIONCALLBACL_H
