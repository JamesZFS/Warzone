#ifndef WEAPON_H
#define WEAPON_H

#include "actor.h"

#include <Box2D/Box2D.h>

#include <QTimer>

class ExplosionEffect;
class Engine;

class Weapon : public Actor
{
    Q_OBJECT
public:
    enum Type
    {
        e_BAZOOKA,
        e_GRENADE,
        e_SHOTGUN,
        e_NONE,
    };
    enum State { e_COMMON, e_LAUNCHED, e_TRIGGERED };

    Weapon(b2Body *body, float32 power_ratio);
    ~Weapon() = default;

    bool isLaunched() const;
    bool isTriggered() const;

    void aim(const b2Vec2 &dir)
    { m_dir = dir; }

public slots:
    void launch();
    void trigger();

public:
    ExplosionEffect *createExplosionEffect() const;

protected:
    virtual void _launch() = 0;
    virtual void _trigger() = 0;
    virtual ExplosionEffect* _createExplosionEffect() const = 0;

    State m_state;
    b2Vec2 m_dir;   // both direction and strength is stored
    const float32 m_power_ratio;  // adjust power according to soldier's size

signals:
    void launched();    // emitted right after launching
    void triggered();   // emitted after taking effect

};

// will trigger when hit something, implemented mainly by contactlistener
class ContactWeapon : public Weapon
{
    Q_OBJECT
public:
    ContactWeapon(b2Body *body, float32 power_ratio) : Weapon(body, power_ratio) {}
    virtual qreal threasholdV() const = 0;
};

class TimingWeapon : public Weapon  // will trigger after certain time
{
    Q_OBJECT
public:
    TimingWeapon(b2Body *body, float32 power_ratio, Engine *proxy_engine);
    virtual qreal threasholdT() const = 0;  // trigger time limit, in sec
    void startTiming();

private:
    QTimer m_timer;
};

class Bazooka : public ContactWeapon
{
    Q_OBJECT
public:
    Bazooka(b2Body *body, float32 power_ratio);

    // QGraphicsItem interface
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // ContactWeapon interface
    qreal threasholdV() const override
    { return 0.1; }

    // Weapon interface
protected:
    void _launch() override;
    void _trigger() override;
    ExplosionEffect *_createExplosionEffect() const override;

private:
    QPolygonF m_shape;

};

class Grenade : public TimingWeapon
{
public:
    Grenade(b2Body *body, float32 power_ratio, qreal duration, Engine *proxy_engine); // duration: in sec

    // QGraphicsItem interface
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // Weapon interface
protected:
    void _launch() override;
    void _trigger() override;
    ExplosionEffect *_createExplosionEffect() const override;

    // TimingWeapon interface
public:
    qreal threasholdT() const override
    { return m_duration; }

private:
    const qreal m_duration;
};

class Shotgun : public ContactWeapon
{
public:
    Shotgun(b2Body *body, float32 power_ratio);

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // Weapon interface
protected:
    void _launch() override;
    void _trigger() override;
    ExplosionEffect *_createExplosionEffect() const override;

    // ContactWeapon interface
public:
    qreal threasholdV() const override
    { return 0.01; }

private:
    QPolygonF m_shape;
};

#endif // WEAPON_H
