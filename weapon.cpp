#include "weapon.h"
#include "defs.h"
#include "explosioneffect.h"
#include "explosioncallback.h"

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

void Weapon::launch()
{
    if (m_state != e_COMMON) return;
    _launch();
    emit launched();
}

void Weapon::trigger()
{
    if (m_state != e_LAUNCHED) return;
//    qDebug() << "Weapon::setoff()";
    _trigger();
    emit triggered(); // the destruction of the weapon should be handled by system
}

ExplosionEffect *Weapon::createExplosionEffect()
{
    auto effect = _createExplosionEffect();
    effect->setPos(pos());
    effect->startAnimation();
    return effect;
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
    m_bbox += QMarginsF(1, 20, 1, 1);
}

void Bazooka::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    switch (m_state) {
    case e_COMMON: {
        painter->setBrush(QColor(220, 190, 50));
        painter->drawPolygon(m_shape);
        break;
    }
    case e_LAUNCHED: {
        painter->setBrush(QColor(220, 190, 50));
        painter->drawPolygon(m_shape);
        QLinearGradient g(QPointF(0, 0), QPointF(0, -10));
        g.setColorAt(0, QColor(220, 0, 0));
        g.setColorAt(1, QColor(240, 200, 180));
        painter->setBrush(g);
        painter->drawEllipse(QPointF(randf(-1.5, 0.5), -5), 1, randf(4, 6));
        painter->drawEllipse(QPointF(randf(-0.5, 1.5), -5), 1, randf(4, 6));
        painter->drawEllipse(QPointF(randf(-0.5, 0.5), -7), 1, randf(5, 8));
        break;
    }
    case e_TRIGGERED:
        painter->setBrush(QColor(50, 0, 10));
        painter->drawPolygon(m_shape);
        break;
    }

    // todo switch different states
}

void Bazooka::_launch()  // fire cannon
{
    Q_ASSERT(m_state == e_COMMON);
    m_state = e_LAUNCHED;
    // give it an initial linear velocity
    m_body->SetLinearVelocity(100 * m_dir);
}

void Bazooka::_trigger() // called when cannon hits something
{
    if (m_state != e_LAUNCHED) return;  // trigger only once
    m_state = e_TRIGGERED;
    // make an explosion
    ExplosionCallback::create(m_body, 8 * m_power_ratio, 50 * m_power_ratio);
}

ExplosionEffect *Bazooka::_createExplosionEffect()
{
    return new BazookaExplosionEffect(50 * m_power_ratio);
}
