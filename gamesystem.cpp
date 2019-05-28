#include "gamesystem.h"
//#include <Box2D/Box2D.h>

GameSystem::GameSystem() :
    world(new b2World({0, -10}))
{
}

void GameSystem::start()
{
    init();
}

void GameSystem::init()
{
    // create ground R units and B units

}

void GameSystem::spawnSoldier(Side side, const SoldierDef &unit_def)
{
    b2BodyDef body_def;
    body_def.position = unit_def.position;
    auto body = world->CreateBody(&body_def);

    // fixture must be created before unit
    b2CircleShape shape;
    shape.m_radius = unit_def.size;
    body->CreateFixture(&shape, GameConsts::soldier_density);

    auto unit = new Soldier(unit_def.life, unit_def.size, body);

    switch (side) {
    case e_RED:
        R_units.push_back(unit);
        break;
    case e_BLUE:
        B_units.push_back(unit);
        break;
    default:
        qFatal("invalid side value!");
    }
}

void GameSystem::killSoldier(GameSystem::Side side, Soldier *unit)
{
    auto &unit_list = (side == e_RED) ? R_units : B_units;
    int id = unit_list.indexOf(unit);
    Q_ASSERT(id >= 0);  // must be found
    unit_list.erase(unit_list.begin() + id);
    delete unit;
}
