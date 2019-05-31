#include "soldier.h"
#include "explosion.h"
#include <QPainter>

Soldier::Soldier(Side side, int life, qreal size, b2Body *body) :
    Actor(body),
    m_side(side), m_life(life), m_power(size),
    m_size(size), m_radius(10 * size)
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

    m_bbox = QRectF(-m_radius - 1, -m_radius - 1, 2 * (m_radius + 1), 2 * (m_radius + 1));
    m_bbox = m_bbox.united(QRectF(-11, 32, 22, 30));
}

b2Body *Soldier::getBody() const
{
    return m_body;
}

void Soldier::jump(const b2Vec2 &strength)
{
    m_body->ApplyLinearImpulse(10 * strength, m_body->GetWorldCenter(), true);
}

void Soldier::setoff()
{
    Explosion::create(m_body, 3, 30);
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

void Soldier::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(getColor());
    painter->drawEllipse(-m_radius, -m_radius, 2.0 * m_radius, 2.0 * m_radius);
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
        emit hurt(m_life);
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
