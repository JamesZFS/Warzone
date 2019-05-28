#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include "defs.h"
#include "soldier.h"



class GameSystem
{
public:
    enum Side
    {
        e_RED, e_BLUE
    };

    GameSystem();

    void start();

protected:
    void init();    // called right after start()

    void spawnSoldier(Side side, const SoldierDef &unit_def);

    void killSoldier(Side side, Soldier *unit);

private:
    b2World *world;

    // created after init()
    b2Body *ground;
    QVector<Soldier *> R_units, B_units;
    b2ParticleSystem *water_system;
};

#endif // GAMESYSTEM_H
