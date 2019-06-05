#ifndef SOLDIER_H
#define SOLDIER_H

#include "defs.h"
#include "actor.h"
#include <Box2D/Box2D.h>

class GameSystem;
class ExplosionEffect;

enum Side
{
    e_RED, e_BLACK, e_NONE, e_TIE
};

struct SoldierDef
{
    Side side;
    int life;
    qreal size;    // power and mass is proposionate to size
    b2Vec2 position;    // world coord center

    SoldierDef()
    {
        side = e_NONE;
        life = 100;
        size = 1;
        position = {0, 0};
    }

    void rand()
    {
        side = randint(0, 2) == 0 ? e_RED : e_BLACK;
        life = randint(100, 300);
        size = randf(0.5, 3);
        position.Set(randf(-100, 100), randf(-100, 100));
    }
};

/** the soldier is simplified into a circle body,
 * with its origin the center of mass
 */
class Soldier : public AnimateActor
{
    Q_OBJECT
public:
    enum State
    {
        e_COMMON,
        e_CURRENT_FIRST,
        e_CURRENT
    };

    Soldier(Side side, int life, qreal size, b2Body *body);

    qreal getPower() const;    
    void setPower(qreal value);
    b2Body *getBody() const;
    qreal getRadius() const;
    qreal getSize() const;
    inline bool isCurrent() const { return m_state != e_COMMON; }
    void setCurrent(bool flag);
    ExplosionEffect *createExplosionEffect();

    /**
     * @brief move
     * move the soldier via an impulse towards (dx, dy)
     * @param strength
     */
    void jump(const b2Vec2 &strength);

    /**
     * @brief setoff
     * manually called to make explosion and kill the soldier
     */
    void setoff();

    // member variables:

    const Side m_side;

signals:
    void triggered();   // emit when setoff is called

private:
    /** attack ability
     * the greater power, the more clumsy a soldier is (larger mass)
     */
    qreal m_power;
    const qreal m_size;  // real size
    const qreal m_radius; // radius on the screen
    State m_state;
    QRectF m_bbox_org;

protected:
    virtual const QColor getColor() = 0;

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

class RedSoldier : public Soldier
{
public:
    RedSoldier(int life_, qreal size_, b2Body *body_);

    // Soldier interface
protected:
    const QColor getColor() override { return Qt::red; }
};

class BlackSoldier : public Soldier
{
public:
    BlackSoldier(int life_, qreal size_, b2Body *body_);

    // Soldier interface
protected:
    const QColor getColor() override { return Qt::black; }
};

#endif // SOLDIER_H
