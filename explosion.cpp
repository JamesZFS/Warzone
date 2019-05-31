#include "explosion.h"
#include "defs.h"
#include "soldier.h"


Explosion::Explosion(const b2Body *self, const b2Vec2 &center, float32 rad, float32 power) :
    m_self(self), m_center(center), m_rad(rad), m_power(power)
{
}

bool Explosion::ReportFixture(b2Fixture *fixture)
{
    auto body = fixture->GetBody();
    if (body == m_self || body->GetType() != b2_dynamicBody)
        return true;  // exclude itself and statics
    float32 distance;
    b2Vec2 normal;
    fixture->ComputeDistance(m_center, &distance, &normal, 0);
    float32 k = distance / m_rad;
    if (k > 1) return true;  // no effect outside rad

    b2Vec2 dir = body->GetPosition() - m_center + b2Vec2(0, randf(0, 2));
    dir.Normalize();
    float32 f = k < 0.1 ? m_power : m_power * (1.0 - k) / 0.9;

    body->ApplyLinearImpulse(dir * f * 1.8, body->GetPosition(), true);   // push body outward

    // do some damage
    Soldier *unit = dynamic_cast<Soldier*>((Actor*)body->GetUserData());
    if (!unit) return true; // not a soldier
    unit->takeDamage(f);

    return true;
}

void Explosion::create(b2Body *self, float32 rad, float32 power)
{
    auto center = self->GetPosition();
    Explosion callback(self, center, rad, power);
    b2AABB aabb;
    aabb.lowerBound = center - b2Vec2(rad, rad);
    aabb.upperBound = center + b2Vec2(rad, rad);
    self->GetWorld()->QueryAABB(&callback, aabb);
}
