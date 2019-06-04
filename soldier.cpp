#include "soldier.h"
#include "explosioncallback.h"
#include "explosioneffect.h"
#include <QPainter>

Soldier::Soldier(Side side, int life, qreal size, b2Body *body) :
    Actor(body),
    m_side(side), m_life(life), m_power(size),
    m_size(size), m_radius(10 * size), m_state(e_COMMON)
{
    Q_ASSERT(m_life > 0);
    m_body->SetType(b2_dynamicBody);
    m_body->SetLinearDamping(0.1);
    m_body->SetFixedRotation(true);
//    m_body->SetAngularDamping(0.8);

    // fixture created after body
    b2CircleShape shape;
    shape.m_radius = size;
    b2FixtureDef fix_def;
    fix_def.shape = &shape;
    fix_def.density = GameConsts::soldier_density;
    fix_def.friction = 0.4;
    fix_def.restitution = 0.2;
    m_body->CreateFixture(&fix_def);

    m_bbox = QRectF(-m_radius - 2, -m_radius - 2, 2 * (m_radius + 2), 2 * (m_radius + 2));
    m_bbox = m_bbox.united(QRectF(-11, 32, 22, 30));
}

b2Body *Soldier::getBody() const
{
    return m_body;
}

void Soldier::jump(const b2Vec2 &strength)
{
    m_body->ApplyLinearImpulse(30 * strength, m_body->GetWorldCenter(), true);
}

void Soldier::setoff()
{
    ExplosionCallback::create(m_body, 3 * m_power, 30 * m_power);
    emit triggered();
}

qreal Soldier::getRadius() const
{
    return m_radius;
}

qreal Soldier::getSize() const
{
    return m_size;
}

void Soldier::setCurrent(bool flag)
{
    m_state = flag ? e_CURRENT_FIRST : e_COMMON;
}

ExplosionEffect *Soldier::createExplosionEffect()
{
    auto effect = new SoldierExplosionEffect(30 * m_size);
    effect->setPos(pos());
    effect->startAnimation();
    return effect;
}

void Soldier::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (isCurrent()) {
        if (m_state == e_CURRENT_FIRST) {
            QPolygonF tri;
            tri << QPointF(-6, 30) << QPointF(6, 30) << QPointF(0, 20);
            painter->setBrush(Qt::yellow);
            painter->drawPolygon(tri);
            m_state = e_CURRENT;
        }
        painter->setPen(QPen(Qt::yellow, 2));
    }
    else
        painter->setPen(Qt::NoPen);
    painter->setBrush(getColor());
    painter->drawEllipse(-m_radius, -m_radius, 2.0 * m_radius, 2.0 * m_radius);
    if (isCurrent()) return;
    // life info:
    painter->scale(1, -1);
    painter->setPen(Qt::black);
    painter->drawText(-11, -25, QString::number(m_life));
}

qreal Soldier::getPower() const
{
    return m_power;
}

void Soldier::setPower(qreal value)
{
    m_power = value;
}

int Soldier::getLife() const
{
    return m_life;
}

void Soldier::setLife(int value)
{
    m_life = value;
}

void Soldier::takeDamage(int damage)
{
    Q_ASSERT(damage >= 0);
    if (m_life > damage) {
        m_life -= damage;
        emit hurt(damage);
    }
    else {
        m_life = 0;
        emit hurt(damage);
        emit died();    // handled by system
    }
}

RedSoldier::RedSoldier(int life_, qreal size_, b2Body *body_) :
    Soldier(e_RED, life_, size_, body_)
{
}

BlackSoldier::BlackSoldier(int life_, qreal size_, b2Body *body_) :
    Soldier(e_BLACK, life_, size_, body_)
{
}
