#include "gamesystem.h"
#include "engine.h"
#include "contactlistener.h"
#include <QGraphicsScene>
#include <QtMath>

GameSystem::GameSystem() :
    QObject(nullptr),
    m_world(nullptr), m_land(nullptr), m_water_system(nullptr),
    m_cur_unit(nullptr), m_cur_weapon(nullptr), m_cur_player(e_NONE),
    m_world_size(50), m_game_state(e_COMMON), m_proxy_engine(nullptr),
    m_contact_listener(new ContactListener),
    m_scene(new QGraphicsScene)
{
}

GameSystem::~GameSystem()
{
    delete m_world;
    delete m_contact_listener;
    delete m_scene;
}

void GameSystem::start()
{
    delete m_proxy_engine;
    m_game_state = e_COMMON;
    resetWorld();
    initWorld();
    m_scene->setSceneRect(-10 * m_world_size, -10 * m_world_size, 20 * m_world_size, 20 * m_world_size);
    m_game_state = e_OPERATIONAL;
    m_cur_player = e_RED; // let the red operate first
    m_prev_R_unit_index = 0;
    m_prev_B_unit_index = -1;
    m_cur_unit = m_R_units[m_prev_R_unit_index];
    m_cur_weapon = nullptr;
    m_proxy_engine = new Engine(this, m_world);
    connect(m_proxy_engine, SIGNAL(requiresUpdate()), this, SLOT(advanceScene()));
    connect(m_proxy_engine, SIGNAL(finished(quint32)), this, SLOT(onSimulationFinished()));
    emit requireOperation(m_cur_player);
}

void GameSystem::end()
{
    // todo
}

void GameSystem::nextPlayer()
{
//    Q_ASSERT(gamestatus == e_OPERATIONAL);
    switch (m_cur_player) {
    case e_RED:
        m_cur_player = e_BLACK;
        m_prev_B_unit_index = (m_prev_B_unit_index + 1) % GameConsts::max_n_unit;
        m_cur_unit = m_B_units[m_prev_B_unit_index];     // focus on the next unit
        break;
    case e_BLACK:
        m_cur_player = e_RED;
        m_prev_R_unit_index = (m_prev_R_unit_index + 1) % GameConsts::max_n_unit;
        m_cur_unit = m_R_units[m_prev_R_unit_index];
        break;
    default:
        qFatal("invalid cur_player value!");
        break;
    }
    m_game_state = e_OPERATIONAL;
    emit playerChanged(m_cur_player);
    emit requireOperation(m_cur_player); // wait for next user's operation
}

QGraphicsScene *GameSystem::getScene() const
{
    return m_scene;
}

void GameSystem::simulate()
{
    qDebug("Simulating...");
    m_game_state = e_SIMULAING;   // lock operation
    // begin simulating
    m_proxy_engine->doSimulation();
}

void GameSystem::resetWorld()
{
    // clear world
    delete m_world;
    m_world = nullptr;
    m_water_system = nullptr;
//    delete land;
//    foreach (Soldier *unit, R_units) {
//        delete unit;
//    }
//    foreach (Soldier *unit, B_units) {
//        delete unit;
//    }
    m_R_units.clear();
    m_B_units.clear();
    m_cur_unit = nullptr;
    m_scene->clear();
}

void GameSystem::initWorld()
{
    // create world
    m_world = new b2World(b2Vec2(0, -GameConsts::gravity_constant));

    // link contact listener
    m_world->SetContactListener(m_contact_listener);

    // create land
    createLand();

    // todo generate particles


    // create R units and B units
    for (int i = 0; i < GameConsts::max_n_unit; ++i) {
        SoldierDef def;
        def.life = 100;
        def.size = 0.5 + i * 0.2;
        def.position.Set(randf(-m_world_size * 0.8, +m_world_size * 0.8), 2);
        //        world->RayCast();
        def.side = e_RED;
        createSoldier(def);
        def.side = e_BLACK;
        def.position.Set(randf(-m_world_size * 0.8, +m_world_size * 0.8), 2);
        createSoldier(def);
    }

    LiquidFun::n_particle_iteration =
            m_world->CalculateReasonableParticleIterations(LiquidFun::time_step);
    Q_ASSERT(m_R_units.length() == m_B_units.length() &&
             m_R_units.length() == GameConsts::max_n_unit);
    qDebug() << "ground and units spwaned! game ready.";
}

void GameSystem::createLand()
{
    // land shape created before Land
    b2BodyDef land_def;
    land_def.position.Set(0, -10);
    auto land_body = m_world->CreateBody(&land_def);

    b2PolygonShape land_shape;
    land_shape.SetAsBox(m_world_size, 10);
    land_body->CreateFixture(&land_shape, 0);
    land_shape.SetAsBox(1, 20, b2Vec2(-m_world_size + 1, 10), 0);
    land_body->CreateFixture(&land_shape, 0);
    land_shape.SetAsBox(1, 20, b2Vec2(+m_world_size - 1, 10), 0);
    land_body->CreateFixture(&land_shape, 0);

    // new land instance
    m_land = new Land(land_body);
    m_land->setB2Pos(land_body->GetPosition());

    m_scene->addItem(m_land);
}

void GameSystem::createSoldier(const SoldierDef &unit_def)
{
    b2BodyDef body_def;
    body_def.position = unit_def.position;
    body_def.type = b2_dynamicBody;
    auto body = m_world->CreateBody(&body_def);

    // new soldier instance
    Soldier *unit;
    switch (unit_def.side) {
    case e_RED:
        unit = new RedSoldier(unit_def.life, unit_def.size, body);
        m_R_units.push_back(static_cast<RedSoldier*>(unit));
        break;
    case e_BLACK:
        unit = new BlackSoldier(unit_def.life, unit_def.size, body);
        m_B_units.push_back(static_cast<BlackSoldier*>(unit));
        break;
    default:
        qFatal("invalid side value!");
    }
    m_scene->addItem(unit);
}

void GameSystem::killSoldier(Soldier *unit)
{
    int id;
    switch (unit->m_side) {
    case e_RED:
        id = m_R_units.indexOf(static_cast<RedSoldier*>(unit));
        m_R_units.erase(m_R_units.begin() + id);
        Q_ASSERT(id >= 0);  // must be found
        emit unitKilled(QString("Red has lost a unit!"));
        break;
    case e_BLACK:
        id = m_B_units.indexOf(static_cast<BlackSoldier*>(unit));
        m_B_units.erase(m_B_units.begin() + id);
        Q_ASSERT(id >= 0);  // must be found
        emit unitKilled(QString("Black has lost a unit!"));
        break;
    default:
        qFatal("invalid unit side!");
    }
    m_world->DestroyBody(unit->body());
    m_scene->removeItem(unit);
    delete unit;
}

void GameSystem::advanceScene()
{
    Q_ASSERT(m_scene);
    m_scene->advance();
}

void GameSystem::waitForOperation(quint32 n_iter)
{
    m_game_state = e_OPERATIONAL;
    qDebug() << QString("simulation finished in %1 iters").arg(n_iter);
    emit requireOperation(m_cur_player);
}

GameSystem::GameState GameSystem::getGamestate() const
{
    return m_game_state;
}

bool GameSystem::isOperational() const
{
    return m_game_state == e_OPERATIONAL;
}

void GameSystem::moveCurUnit(const b2Vec2 &strength)
{
    Q_ASSERT(m_game_state == e_OPERATIONAL);
    m_cur_unit->jump(strength);

    disconnect(m_proxy_engine, SIGNAL(finished(quint32)), 0, 0);
    connect(m_proxy_engine, SIGNAL(finished(quint32)), this, SLOT(waitForOperation(quint32)));  // async operation
    simulate();
}

void GameSystem::destroyCurWeapon()
{
    if (!m_cur_weapon) return;
//    m_scene->removeItem(m_cur_weapon);
    m_proxy_engine->discard(m_cur_weapon);
//    delete m_cur_weapon;
    m_cur_weapon = nullptr;
    qDebug("weapon destroyed");
}

void GameSystem::onSimulationFinished()
{
//    destroyCurWeapon();
}

void GameSystem::fireCurUnit(Weapon::Type weapon, const b2Vec2 &strength)
{
    Q_ASSERT(m_game_state == e_OPERATIONAL);
    Q_ASSERT(!m_cur_weapon);

    b2BodyDef def;
    b2Vec2 dir = strength / strength.Length();
    def.position = m_cur_unit->getBody()->GetPosition();
    def.position += dir * m_cur_unit->getSize() * 3;    // ** offset a little
    def.angle = -qAtan2(dir.x, dir.y);   // initial angle, counter-clock-wise +

    auto weapon_body = m_world->CreateBody(&def);
    switch (weapon) {
    case Weapon::e_BAZOOKA:
        m_cur_weapon = new Bazooka(weapon_body);
        break;
    default:
        qFatal("Weapon not implemented!");
        break;
    }
    m_scene->addItem(m_cur_weapon);

    connect(m_cur_weapon, SIGNAL(triggered()), this, SLOT(destroyCurWeapon()));

    m_cur_weapon->aim(strength);
    m_cur_weapon->fire();

    disconnect(m_proxy_engine, SIGNAL(finished(quint32)), 0, 0);
    connect(m_proxy_engine, SIGNAL(finished(quint32)), this, SLOT(nextPlayer()));  // async switch player
    simulate();
}

Side GameSystem::getCurPlayer() const
{
    return m_cur_player;
}
