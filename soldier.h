#ifndef SOLDIER_H
#define SOLDIER_H

#include "defs.h"
#include "actor.h"
#include <Box2D/Box2D.h>

class GameSystem;

enum Side
{
    e_RED, e_BLACK, e_NONE
};

struct SoldierDef
{
    Side side;
    int life;
    double size;    // power and mass is proposionate to size
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
class Soldier : public Actor
{
public:
    Soldier(Side side, int life, double size, b2Body *body);

    int getLife() const;
    double getPower() const;
    void setLife(int value);
    void setPower(double value);
    b2Body *getBody() const;
    qreal getRadius() const;
    double getSize() const;

    /**
     * @brief move
     * move the soldier via an impulse towards (dx, dy)
     * @param strength
     */
    void jump(const b2Vec2 &strength);

    // member variables:

    const Side m_side;

private:
    /**
     * HP, > 0
     * when HP == 0, the soldier BOOMS to die
     */
    int m_life;

    /** attack ability
     * the greater power, the more clumsy a soldier is (larger mass)
     */
    double m_power;

    const double m_size;  // real size

    const qreal m_radius; // radius on the screen

protected:
    virtual const QColor getColor() = 0;

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

class RedSoldier : public Soldier
{
public:
    RedSoldier(int life_, double size_, b2Body *body_);

    // Soldier interface
protected:
    const QColor getColor() override { return Qt::red; }
};

class BlackSoldier : public Soldier
{
public:
    BlackSoldier(int life_, double size_, b2Body *body_);

    // Soldier interface
protected:
    const QColor getColor() override { return Qt::black; }
};

#endif // SOLDIER_H
