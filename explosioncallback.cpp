#include "explosioncallback.h"
#include "defs.h"
#include "soldier.h"


ExplosionCallback::ExplosionCallback(const b2Body *self, const b2Vec2 &center, float32 rad, float32 power) :
    m_self(self), m_center(center), m_rad(rad), m_power(power)
{
}

bool ExplosionCallback::ReportFixture(b2Fixture *fixture)
{
    auto body = fixture->GetBody();
    if (body == m_self)
        return true;  // exclude itself

    float32 distance;
    b2Vec2 normal;
    fixture->ComputeDistance(m_center, &distance, &normal, 0);
    distance -= fixture->GetShape()->m_radius;
    float32 k = distance / m_rad;
    if (k > 1) return true;  // no effect outside rad

    b2Vec2 dir = -normal + b2Vec2(randf(-0.1, 0.1), randf(-0.1, 0.1));
    dir.Normalize();
    float32 f = k < 0.1 ? m_power : m_power * (1.0 - k) / 0.9;

    if (body->GetType() == b2_dynamicBody)     // exert impulse to dynamic bodies
        body->ApplyLinearImpulse(dir * f * 1.8, body->GetPosition(), true);

    // do some damage to AnimateActor
    auto *animate = dynamic_cast<AnimateActor*>((Actor*)body->GetUserData());
    if (!animate) return true; // not an animate actor
    animate->takeDamage(f);

    return true;
}

void ExplosionCallback::create(b2Body *self, float32 rad, float32 power)
{
    auto center = self->GetPosition();
    ExplosionCallback callback(self, center, rad, power);
    b2AABB aabb;
    aabb.lowerBound = center - b2Vec2(rad, rad);
    aabb.upperBound = center + b2Vec2(rad, rad);
    self->GetWorld()->QueryAABB(&callback, aabb);
}
