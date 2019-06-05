#ifndef INITIALIZER_H
#define INITIALIZER_H

#include <QBrush>
#include <QPixmap>
#include <QSet>

class b2World;
class b2Body;
class Land;
class Soldier;
class RedSoldier;
class BlackSoldier;
class Brick;

// can initialize the world: land, bricks, soldiers
class Initializer
{
public:
    Initializer(b2World *world);
    virtual ~Initializer() = default;

    virtual void initLand(Land *&land) = 0;
    virtual void initBricks(QSet<Brick *> &bricks) = 0;
    virtual void initUnits(QSet<RedSoldier *> &R_units, QSet<BlackSoldier *> &B_units) = 0;

protected:
    b2World *m_world;
};


class FortressInitializer : public Initializer
{
public:
    FortressInitializer(b2World *world) : Initializer(world) {}

    // Initializer interface
    void initLand(Land *&land) override;
    void initBricks(QSet<Brick *> &bricks) override;
    void initUnits(QSet<RedSoldier *> &R_units, QSet<BlackSoldier *> &B_units) override;
};


#endif // INITIALIZER_H
