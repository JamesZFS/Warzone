#ifndef ACTOR_H
#define ACTOR_H

#include "defs.h"

#include <QGraphicsObject>

struct b2Vec2;

class b2World;
class b2Body;
class b2PolygonShape;
class b2Shape;
class b2Fixture;
struct b2AABB;
typedef float float32;

class Actor : public QGraphicsObject
{
public:
    Actor(b2Body *body);

    ~Actor() = default;

    void setB2Pos(const b2Vec2 &b_pos);
    void setB2Angle(const float32 angle);

    QPointF mapB2Pos(const b2Vec2 &b_pos) const;  // map b2Vec to screen coord
    QPolygonF fromB2Polygon(const b2PolygonShape &b2_poly) const;
    QRectF fromB2AABB(const b2AABB &aabb) const;    // be careful to use this
    QRectF findBodyBound() const;

protected:
    void drawFixtures(QPainter *painter, const b2Fixture *fixture) const;

    /**
     * Physical body, enpowered by LiquidFun™ backend
     * the ownership is taken by world
     */
    b2Body* const m_body;
    QRectF m_bbox; // bounding box
    const b2World* const m_world;

    // QGraphicsItem interface
public:
    QRectF boundingRect() const override
    { return m_bbox; }

    virtual void advance(int phase) override;

    b2Body *body() const;
};

class ContactListener;
class ExplosionCallback;

// actor that has life, and can be killed by weapons or collision
class AnimateActor : public Actor
{
    Q_OBJECT
    // those two classes can modify the life
    friend class ContactListener;
    friend class ExplosionCallback;
public:
    AnimateActor(b2Body *body, int life);

    int life() const { return m_life; }
    bool isAlive() const { return m_life > 0; }

signals:
    void died();    // emit when life == 0
    void hurt(int damage);

private:
    void setLife(int life) { m_life = life; }
    void takeDamage(int damage);

    /**
     * HP, > 0
     * when HP == 0, the soldier BOOMS to die
     */
    int m_life;
};

#endif // ACTOR_H
