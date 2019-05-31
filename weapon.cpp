#include "weapon.h"
#include "defs.h"
#include "explosion.h"
#include <QPainter>


Weapon::Weapon(b2Body *body, float32 power_ratio) :
    Actor(body), m_state(e_COMMON), m_power_ratio(power_ratio)
{
    body->SetType(b2_dynamicBody);
    body->SetBullet(true);          // default - fast moving object
    body->SetLinearDamping(0.1);
    body->SetAngularDamping(0.1);
}

bool Weapon::isLaunched() const
{
    return m_state == e_LAUNCHED;
}

bool Weapon::isTriggered() const
{
    return m_state == e_TRIGGERED;
}

void Weapon::fire()
{
    if (m_state != e_COMMON) return;
    launch();
    emit launched();
}

void Weapon::setoff()
{
    if (m_state != e_LAUNCHED) return;
//    qDebug() << "Weapon::setoff()";
    trigger();
    emit triggered(); // the destruction of the weapon should be handled by system
}

Bazooka::Bazooka(b2Body *body, float32 power_ratio) : ContactWeapon(body, power_ratio)
{
    Q_ASSERT(!body->GetFixtureList());
    body->SetGravityScale(2.0);
    // a triangle shape
    b2PolygonShape shape;
    static b2Vec2 tri[3] = {b2Vec2(-0.3, -0.5), b2Vec2(0.3, -0.5), b2Vec2(0, 0.8)};
    shape.Set(tri, 3);
    body->CreateFixture(&shape, GameConsts::bazooka_density);

    m_shape = fromB2Polygon(shape);
    m_bbox = m_shape.boundingRect();
    m_bbox += QMargins(1, 1, 1, 1);
}

void Bazooka::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(QColor(220, 190, 50));
    painter->drawPolygon(m_shape);
    // todo switch different states
}

void Bazooka::launch()  // fire cannon
{
    Q_ASSERT(m_state == e_COMMON);
    m_state = e_LAUNCHED;
    // give it an initial linear velocity
    m_body->SetLinearVelocity(100 * m_dir);
}

void Bazooka::trigger() // called when cannon hits something
{
    if (m_state != e_LAUNCHED) return;  // trigger only once
    m_state = e_TRIGGERED;
    // make an explosion
    Explosion::create(m_body, 8 * m_power_ratio, 50 * m_power_ratio);
}
