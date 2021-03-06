#include "weapon.h"
#include "defs.h"
#include "explosioneffect.h"
#include "explosioncallback.h"
#include "engine.h"

#include <QPainter>
#include <QtMath>

static const auto pi_2 = b2_pi / 2;


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
    m_state = e_LAUNCHED;
    _launch();
    emit launched();
}

void Weapon::trigger()
{
    if (m_state != e_LAUNCHED) return;  // trigger only once
    m_state = e_TRIGGERED;
    _trigger();
    emit triggered(); // the destruction of the weapon should be handled by system
}

ExplosionEffect *Weapon::createExplosionEffect() const
{
    auto effect = _createExplosionEffect();
    effect->setPos(pos());
    effect->startAnimation();
    return effect;
}

// CONTACT WEAPONS

Bazooka::Bazooka(b2Body *body, float32 power_ratio) :
    ContactWeapon(body, power_ratio)
{
    Q_ASSERT(!body->GetFixtureList());
    body->SetGravityScale(1.5);
    // a triangle shape
    b2PolygonShape shape;
    static b2Vec2 tri[3] = {b2Vec2(-0.5, -0.7), b2Vec2(0.5, -0.7), b2Vec2(0, 1.0)};
    shape.Set(tri, 3);
    body->CreateFixture(&shape, GameConsts::bazooka_density);

    m_shape = fromB2Polygon(shape);
    m_bbox = findBodyBound();
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
        auto v = m_body->GetLinearVelocity();
        auto angle = b2Atan2(v.y, v.x) - pi_2;
        // async adjust bazooka angle
        s_proxy_engine->requestTransform(new TransformRequest(m_body, m_body->GetPosition(), angle));
        painter->setBrush(QColor(220, 190, 50));
        painter->drawPolygon(m_shape);
        // draw tail
        static QLinearGradient g(QPointF(0, 0), QPointF(0, -10));
        g.setColorAt(0, QColor(220, 0, 0));
        g.setColorAt(1, QColor(240, 200, 180));
        painter->setBrush(g);
        painter->drawEllipse(QPointF(randf(-1.5, 0.5), -5), 1, randf(4, 6));
        painter->drawEllipse(QPointF(randf(-0.5, 1.5), -5), 1, randf(4, 6));
        painter->drawEllipse(QPointF(randf(-0.5, 0.5), -7), 1, randf(5, 8));
        break;
    }
    case e_TRIGGERED: {
        painter->setBrush(QColor(50, 0, 10));
        painter->drawPolygon(m_shape);
        break;
    }
    }
}

void Bazooka::_launch()  // fire cannon
{
    // give it an initial linear velocity
    m_body->SetLinearVelocity(GameConsts::bazooka_velocity * m_dir);
}

void Bazooka::_trigger() // called when cannon hits something
{
    // make an explosion
    ExplosionCallback::create(m_body, 8 * m_power_ratio, 50 * m_power_ratio);
}

ExplosionEffect *Bazooka::_createExplosionEffect() const
{
    return new BazookaExplosionEffect(50 * m_power_ratio);
}

// TIMING WEAPONS

TimingWeapon::TimingWeapon(b2Body *body, float32 power_ratio, Engine *proxy_engine) :
    Weapon(body, power_ratio)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(trigger()));
    connect(this, SIGNAL(launched()), proxy_engine, SLOT(enableAlways()));
    connect(this, SIGNAL(triggered()), proxy_engine, SLOT(disableAlways()));
}

void TimingWeapon::startTiming()
{
    m_timer.setSingleShot(true);    // trigger only once
    m_timer.start(int(threasholdT() * 1000));
}

Grenade::Grenade(b2Body *body, float32 power_ratio, qreal duration, Engine *proxy_engine) :
    TimingWeapon(body, power_ratio, proxy_engine), m_duration(duration)
{
    Q_ASSERT(!body->GetFixtureList());
    body->SetGravityScale(1.5);
    // two circle shapes
    b2CircleShape shape;//, head;
    shape.m_radius = 0.5;   // main part
//    head.m_p.Set(0, 0.4);
//    head.m_radius = 0.3;
    b2FixtureDef def;
    def.restitution = 0.2;
    def.density = GameConsts::grenade_density;
    def.friction = 0.05;
    def.shape = &shape;
    body->CreateFixture(&def);
//    def.shape = &head;
//    body->CreateFixture(&def);
    m_bbox = QRectF(-6, -6, 12, 15);
}

void Grenade::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(QColor(90, 150, 70));
    painter->drawEllipse(QPointF(), 5, 5);
    painter->setPen(Qt::black);
    painter->setBrush(QColor(130, 180, 70));
    painter->drawEllipse(QPointF(0, 4), 4, 3);
}

void Grenade::_launch()
{
    m_body->SetLinearVelocity(GameConsts::grenade_velocity * m_dir);
    m_body->SetAngularVelocity(-50 * m_dir.x);
    startTiming();
}

void Grenade::_trigger()
{
    ExplosionCallback::create(m_body, 8 * m_power_ratio, 50 * m_power_ratio);
}

ExplosionEffect *Grenade::_createExplosionEffect() const
{
    return new GrenadeExplosionEffect(50 * m_power_ratio);
}

Shotgun::Shotgun(b2Body *body, float32 power_ratio) : ContactWeapon(body, power_ratio)
{
    Q_ASSERT(!body->GetFixtureList());
    body->SetGravityScale(0.5);
    // a triangle shape
    b2PolygonShape shape;
    static b2Vec2 tri[3] = {b2Vec2(-0.1, -0.25), b2Vec2(0.1, -0.25), b2Vec2(0, 0.4)};
    shape.Set(tri, 3);
    body->CreateFixture(&shape, GameConsts::bullet_density);

    m_shape = fromB2Polygon(shape);
    m_bbox = findBodyBound();
    m_bbox += QMarginsF(1, 1, 1, 1);
}

void Shotgun::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(QColor(220, 130, 50));
    painter->drawPolygon(m_shape);
}

void Shotgun::_launch()
{
    m_dir.Normalize();
    m_body->SetLinearVelocity(GameConsts::bullet_velocity * m_dir);
}

void Shotgun::_trigger()
{
    ExplosionCallback::create(m_body, 3 * m_power_ratio, 25 * m_power_ratio);
}

ExplosionEffect *Shotgun::_createExplosionEffect() const
{
    return new SoldierExplosionEffect(15 * m_power_ratio);
}
