#ifndef SOLDIER_H
#define SOLDIER_H

#include "defs.h"
#include <Box2D/Box2D.h>

class GameSystem;

struct SoldierDef
{
    int life;
    double size;    // power and mass is proposionate to size
    b2Vec2 position;    // world coord center

    SoldierDef()
    {
        life = 100;
        size = 1;
        position = {0, 0};
    }

    void rand()
    {
        life = randint(100, 300);
        size = randf(0.5, 3);
        position.Set(randf(-100, 100), randf(-100, 100));
    }
};

class Soldier
{
    friend class GameSystem;

    int getLife() const;

    double getPower() const;

protected:
    Soldier(int life_, double power_, b2Body *body_);
    ~Soldier() = default;

    void setLife(int value);

    void setPower(double value);

    b2Body *getBody() const;

    /**
     * @brief move
     * move the soldier via an impulse towards (dx, dy)
     * @param delta
     */
    void move(b2Vec2 delta);

    /**
     * HP, > 0
     * when HP == 0, the soldier BOOMS to die
     */
    int life;

    /** attack ability
     * the greater power, the more clumsy a soldier is (larger mass)
     */
    double power;

    /**
     * Physical body of the soldier, enpowered by LiquidFun™ backend
     * the soldier is simplified into a circle body,
     * with its origin the center of mass
     */
    b2Body* const body;

    /**
     * geometric fixture of the body
     */
    b2Fixture* const fixture;
};

#endif // SOLDIER_H
