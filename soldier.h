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
    Soldier(Side side_, int life_, double power_, b2Body *body_);

    int getLife() const;

    double getPower() const;

    void setLife(int value);

    void setPower(double value);

    b2Body *getBody() const;

    qreal getRadius() const;

    /**
     * @brief move
     * move the soldier via an impulse towards (dx, dy)
     * @param strength
     */
    void move(const b2Vec2 &strength);

    // member variables:

    const Side side;

private:

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
     * geometric fixture of the body
     */
    b2Fixture* const fixture;

    const qreal radius; // radius on the screen

    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void advance(int phase) override;
};

#endif // SOLDIER_H
