#include "explosion.h"
#include "defs.h"


Explosion::Explosion(const b2Body *self, b2Vec2 center, float32 rad, float32 power) :
    m_self(self), m_center(center), m_rad(rad), m_power(power)
{
}

bool Explosion::ReportFixture(b2Fixture *fixture)
{
    auto body = fixture->GetBody();
    if (body == m_self) return true;    // exclude itself
    float32 distance;
    b2Vec2 normal;
    fixture->ComputeDistance(m_center, &distance, &normal, 0);
    float32 k = distance / m_rad;
    if (k > 1) return true;  // no effect outside rad

    b2Vec2 dir = body->GetPosition() - m_center + b2Vec2(0, randf(0, 2));
    dir.Normalize();
    float32 f = k < 0.1 ? m_power : m_power * (1.0 - k) / 0.9;

    body->ApplyLinearImpulse(dir * f, body->GetPosition(), true);   // push body outward
    return true;
}
