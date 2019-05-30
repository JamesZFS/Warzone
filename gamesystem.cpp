#include "gamesystem.h"
#include "engine.h"
#include <QGraphicsScene>


GameSystem::GameSystem() :
    QObject(nullptr),
    world(nullptr), land(nullptr), water_system(nullptr),
    cur_unit(nullptr), cur_player(e_NONE),
    world_size(50), gamestatus(e_COMMON), proxy_engine(nullptr),
    scene(new QGraphicsScene)
{
}

void GameSystem::start()
{
    delete proxy_engine;
    gamestatus = e_COMMON;
    resetWorld();
    initWorld();
    scene->setSceneRect(-10 * world_size, -10 * world_size, 20 * world_size, 20 * world_size);
    gamestatus = e_OPERATIONAL;
    cur_player = e_RED; // let the red operate first
    prev_R_unit_index = 0;
    prev_B_unit_index = -1;
    cur_unit = R_units[prev_R_unit_index];
    proxy_engine = new Engine(this, world);
    connect(proxy_engine, SIGNAL(requiresUpdate()), this, SLOT(advanceScene()));
    emit requireOperation(cur_player);
}

void GameSystem::end()
{
    // todo
}

void GameSystem::nextPlayer()
{
//    Q_ASSERT(gamestatus == e_OPERATIONAL);
    switch (cur_player) {
    case e_RED:
        cur_player = e_BLACK;
        prev_B_unit_index = (prev_B_unit_index + 1) % GameConsts::max_n_unit;
        cur_unit = B_units[prev_B_unit_index];     // focus on the next unit
        break;
    case e_BLACK:
        cur_player = e_RED;
        prev_R_unit_index = (prev_R_unit_index + 1) % GameConsts::max_n_unit;
        cur_unit = R_units[prev_R_unit_index];
        break;
    default:
        qFatal("invalid cur_player value!");
        break;
    }
    gamestatus = e_OPERATIONAL;
    emit playerChanged(cur_player);
    emit requireOperation(cur_player); // wait for next user's operation
}

QGraphicsScene *GameSystem::getScene() const
{
    return scene;
}

void GameSystem::simulate()
{
    gamestatus = e_SIMULAING;   // lock operation
    // begin simulating
    proxy_engine->doSimulation();
}

void GameSystem::resetWorld()
{
    // clear world
    delete world;
    world = nullptr;
    water_system = nullptr;
//    delete land;
//    foreach (Soldier *unit, R_units) {
//        delete unit;
//    }
//    foreach (Soldier *unit, B_units) {
//        delete unit;
//    }
    R_units.clear();
    B_units.clear();
    cur_unit = nullptr;
    scene->clear();
}

void GameSystem::initWorld()
{
    // create world
    world = new b2World(b2Vec2(0, -GameConsts::gravity_constant));

    // create land
    createLand();

    // todo generate particles


    // create R units and B units
    for (int i = 0; i < GameConsts::max_n_unit; ++i) {
        SoldierDef def;
        def.life = 100;
        def.size = 0.5 + i * 0.2;
        def.position.Set(randf(-world_size * 0.8, +world_size * 0.8), 2);
        //        world->RayCast();
        def.side = e_RED;
        createSoldier(def);
        def.side = e_BLACK;
        def.position.Set(randf(-world_size * 0.8, +world_size * 0.8), 2);
        createSoldier(def);
    }

    LiquidFun::n_particle_iteration =
            world->CalculateReasonableParticleIterations(LiquidFun::time_step);
    Q_ASSERT(R_units.length() == B_units.length() &&
             R_units.length() == GameConsts::max_n_unit);
    qDebug() << "ground and units spwaned! game ready.";
}

void GameSystem::createLand()
{
    // todo various land shape
    b2BodyDef land_def;
    land_def.position.Set(0, -10);
    land_def.type = b2_staticBody;
    auto land_body = world->CreateBody(&land_def);

    b2PolygonShape land_shape;
    land_shape.SetAsBox(world_size, 10);
    land_body->CreateFixture(&land_shape, 0);
    land_shape.SetAsBox(1, 20, b2Vec2(-world_size + 1, 10), 0);
    land_body->CreateFixture(&land_shape, 0);
    land_shape.SetAsBox(1, 20, b2Vec2(+world_size - 1, 10), 0);
    land_body->CreateFixture(&land_shape, 0);

    // new land instance
    land = new Land(land_body);
    land->setB2Pos(land_body->GetPosition());

    scene->addItem(land);
}

void GameSystem::createSoldier(const SoldierDef &unit_def)
{
    b2BodyDef body_def;
    body_def.position = unit_def.position;
    body_def.type = b2_dynamicBody;
    auto body = world->CreateBody(&body_def);

    // fixture must be created before unit
    b2CircleShape shape;
    shape.m_radius = unit_def.size;
    body->CreateFixture(&shape, GameConsts::soldier_density);

    // new soldier instance
    auto unit = new Soldier(unit_def.side, unit_def.life, unit_def.size, body);
    unit->setB2Pos(body->GetPosition());

    switch (unit->side) {
    case e_RED:
        R_units.push_back(unit);
        break;
    case e_BLACK:
        B_units.push_back(unit);
        break;
    default:
        qFatal("invalid side value!");
    }
    scene->addItem(unit);
}

void GameSystem::killSoldier(Soldier *unit)
{
    auto &unit_list = (unit->side == e_RED) ? R_units : B_units;
    int id = unit_list.indexOf(unit);
    Q_ASSERT(id >= 0);  // must be found
    unit_list.erase(unit_list.begin() + id);
    scene->removeItem(unit);
    delete unit;
    emit unitKilled(QString("%1 team has lost a unit!").arg(unit->side == e_RED ? "Red" : "Black"));
}

void GameSystem::advanceScene()
{
    Q_ASSERT(scene);
    scene->advance();
}

void GameSystem::waitForOperation()
{
    gamestatus = e_OPERATIONAL;
    emit requireOperation(cur_player);
}

GameSystem::GameStatus GameSystem::getGamestatus() const
{
    return gamestatus;
}

bool GameSystem::isOperational() const
{
    return gamestatus == e_OPERATIONAL;
}

void GameSystem::moveCurUnit(const b2Vec2 &strength)
{
    Q_ASSERT(gamestatus == e_OPERATIONAL);
    cur_unit->move(strength);

    disconnect(proxy_engine, SIGNAL(finished()), 0, 0);
    connect(proxy_engine, SIGNAL(finished()), this, SLOT(waitForOperation()));  // async operation
    simulate();
}

void GameSystem::fireCurUnit(GameSystem::Weapons weapon, b2Vec2 strength)
{
    Q_ASSERT(gamestatus == e_OPERATIONAL);

    disconnect(proxy_engine, SIGNAL(finished()), 0, 0);
    connect(proxy_engine, SIGNAL(finished()), this, SLOT(nextPlayer()));  // async switch player
    simulate();
}

Side GameSystem::getCurPlayer() const
{
    return cur_player;
}
