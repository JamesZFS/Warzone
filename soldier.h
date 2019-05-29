#ifndef SOLDIER_H
#define SOLDIER_H

#include "defs.h"
#include <Box2D/Box2D.h>
#include <QGraphicsItem>

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

class Soldier : public QGraphicsItem
{
    friend class GameSystem;

    int getLife() const;

    double getPower() const;

protected:
    Soldier(Side side_, int life_, double power_, b2Body *body_);
    ~Soldier() = default;

    void setLife(int value);

    void setPower(double value);

    b2Body *getBody() const;

    /**
     * @brief move
     * move the soldier via an impulse towards (dx, dy)
     * @param strength
     */
    void move(const b2Vec2 &strength);

    // member variables:
    Side side;

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

    const qreal radius; // radius on the screen

public:
    void setB2Pos(const b2Vec2 &b_pos);

    // QGraphicsItem interface
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void advance(int phase) override;
};

#endif // SOLDIER_H
