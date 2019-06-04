#include "gamesystem.h"
#include "engine.h"
#include "contactlistener.h"
#include "explosioneffect.h"

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
    m_cur_player = e_RED; // let the red operate first
    m_R_iter = m_R_units.constBegin();
    m_B_iter = m_B_units.constBegin();
    setCurUnit(*m_R_iter);
    m_cur_weapon = nullptr;
    m_proxy_engine = new Engine(this, m_world);
    m_kill_list.clear();
    connect(m_proxy_engine, SIGNAL(requiresUpdate()), this, SLOT(advanceScene()));
    connect(m_proxy_engine, SIGNAL(finished(quint32)), this, SLOT(onSimulationFinished(quint32)));
    simulateThen(&GameSystem::waitForOperation);
}

QGraphicsScene *GameSystem::getScene() const
{
    return m_scene;
}

void GameSystem::simulateThen(FunPtr next)
{
    qDebug("Simulating...");
    // remake async connection
    m_after_simulation = next;
    m_game_state = e_SIMULAING;   // lock operation
    emit beginSimulating();
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
    foreach (auto item, m_ext_item) {
        m_scene->removeItem(item);
    }
    m_ext_item.clear();
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
        def.size = 0.5 + i * 0.2;   // 0.5 - 1.5
        def.position.Set(randf(-m_world_size * 0.8, +m_world_size * 0.8), 2);
        def.side = e_RED;
        createSoldier(def);
        def.side = e_BLACK;
        def.position.Set(randf(-m_world_size * 0.8, +m_world_size * 0.8), 2);
        createSoldier(def);
    }

    LiquidFun::n_particle_iteration =
            m_world->CalculateReasonableParticleIterations(LiquidFun::time_step);
    Q_ASSERT(m_R_units.size() == m_B_units.size() &&
             m_R_units.size() == GameConsts::max_n_unit);
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
    land_shape.SetAsBox(m_world_size, 1, b2Vec2(0, 30), 0);
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
        m_R_units.insert((RedSoldier*)unit);
        break;
    case e_BLACK:
        unit = new BlackSoldier(unit_def.life, unit_def.size, body);
        m_B_units.insert((BlackSoldier*)unit);
        break;
    default:
        qFatal("invalid side value!");
    }
    connect(unit, SIGNAL(hurt(int)), this, SLOT(onSoldierHurt(int)));
    connect(unit, SIGNAL(died()), this, SLOT(onSoldierDied()));
    connect(unit, SIGNAL(triggered()), this, SLOT(destroySoldier()));
    m_scene->addItem(unit);
}

void GameSystem::setoffSoldier(Soldier *unit)
{
    unit->setoff();
}

void GameSystem::setCurUnit(Soldier *unit)
{
    m_cur_unit = unit;
    m_cur_unit->setCurrent(true);
    m_cur_unit->advance(1);
    emit setLabelPrompt("Unit Life");
    emit setLCDNumber(unit->getLife());
}

Side GameSystem::checkWhoWins()
{
    if (m_R_units.isEmpty())
        return m_B_units.isEmpty() ? e_TIE : e_BLACK;  // tie or black wins
    else
        return m_B_units.isEmpty() ? e_RED : e_NONE;    // red wins or game continues
}

void GameSystem::setGameOver(Side winner)
{
    qDebug() << "\n==GAME OVER==\n";
    m_game_state = e_COMMON;
    emit gameOver(winner);
}

void GameSystem::advanceScene()
{
//    Q_ASSERT(m_scene);
    m_scene->advance();
}

void GameSystem::waitForOperation()
{
    Q_ASSERT(m_game_state == e_OPERATIONAL);
    emit requireOperation(m_cur_player);
}

void GameSystem::switchPlayer()
{
    Q_ASSERT(m_game_state == e_OPERATIONAL);
    Side ret = checkWhoWins();
    if (ret != e_NONE) {
        // either side wins or tie
        setGameOver(ret);
        return;
    }
    // game continuing...
    switch (m_cur_player) {
    case e_RED:
        m_cur_player = e_BLACK;
        setCurUnit(*m_B_iter);     // focus on the next unit
        if (++m_B_iter == m_B_units.end()) m_B_iter = m_B_units.begin();
        break;
    case e_BLACK:
        m_cur_player = e_RED;
        setCurUnit(*m_R_iter);     // focus on the next unit
        if (++m_R_iter == m_R_units.end()) m_R_iter = m_R_units.begin();
        break;
    default:
        qFatal("invalid cur_player value!");
        break;
    }
    emit playerChanged(m_cur_player);
    emit requireOperation(m_cur_player); // wait for next user's operation
}

GameSystem::GameState GameSystem::getGamestate() const
{
    return m_game_state;
}

bool GameSystem::isOperational() const
{
    return m_game_state == e_OPERATIONAL;
}

void GameSystem::addToScene(QGraphicsItem *item)
{
    m_scene->addItem(item);
    m_ext_item.insert(item);
}

void GameSystem::removeFromScene(QGraphicsItem *item)
{
    m_scene->removeItem(item);
    m_ext_item.remove(item);
}

void GameSystem::moveCurUnit(const b2Vec2 &strength)
{
    Q_ASSERT(m_game_state == e_OPERATIONAL);
    m_cur_unit->jump(strength);

    simulateThen(&GameSystem::waitForOperation);
}

void GameSystem::destroyWeapon()
{
    auto weapon = qobject_cast<Weapon*>(sender());
    Q_ASSERT(weapon);
    m_scene->removeItem(weapon);
    m_proxy_engine->discard(weapon);
    auto effect = weapon->createExplosionEffect();
    connect(effect, &ExplosionEffect::animationFinished, this, [this, effect](){
        m_scene->removeItem(effect);
        delete effect;
    });
    m_scene->addItem(effect);
    advanceScene();
}

void GameSystem::destroySoldier()
{
    auto unit = qobject_cast<Soldier*>(sender());
    Q_ASSERT(unit);
    qDebug("  a unit destroyed");
    switch (unit->m_side) {
    case e_RED:
        Q_ASSERT(m_R_units.remove((RedSoldier*)unit));
        if (*m_R_iter == (RedSoldier*)unit) {
            qDebug("  >> destroySoldier");
            m_R_iter = m_R_units.constBegin();
        }
        emit unitKilled(QString("Red has lost a unit!"));
        break;
    case e_BLACK:
        Q_ASSERT(m_B_units.remove((BlackSoldier*)unit));
        if (*m_B_iter == (BlackSoldier*)unit) {
            qDebug("  >> destroySoldier");
            m_B_iter = m_B_units.constBegin();
        }
        emit unitKilled(QString("Black has lost a unit!"));
        break;
    default:
        qFatal("invalid unit side!");
    }
    m_scene->removeItem(unit);
    m_proxy_engine->discard(unit);  // safe delete
}

void GameSystem::onSimulationFinished(quint32 n_iter)
{
    qDebug() << QString("simulation finished in %1 iters").arg(n_iter);
    m_game_state = e_COMMON;
    // deal with dead units
    if (m_kill_list.isEmpty()) {
        m_game_state = e_OPERATIONAL;
        (this->*m_after_simulation)();
    }
    else {  // execute one unit per simulation
        Soldier *exec_unit = *m_kill_list.begin();
        m_kill_list.remove(exec_unit);
        exec_unit->setoff();
        simulateThen(m_after_simulation);   // keep exploding until no one killed
    }
}

void GameSystem::onSoldierHurt(int damage)
{
    auto who = qobject_cast<Soldier*>(sender());
    emit unitHurt(damage);
    Q_ASSERT(who);
}

void GameSystem::onSoldierDied()
{
    auto who = qobject_cast<Soldier*>(sender());
    Q_ASSERT(who);
    qDebug("  a unit life = 0");
    m_kill_list.insert(who);
}

const Soldier *GameSystem::getCurUnit() const
{
    return m_cur_unit;
}

void GameSystem::fireCurUnit(Weapon::Type weapon, const b2Vec2 &strength)
{
    Q_ASSERT(m_game_state == e_OPERATIONAL);
    m_cur_unit->setCurrent(false);
    emit setLCDNumber(0);

    b2BodyDef def;
    b2Vec2 dir = strength / strength.Length();
    def.position = m_cur_unit->getBody()->GetPosition();
    def.position += dir * m_cur_unit->getSize() * 3;    // ** offset a little
    def.angle = -qAtan2(dir.x, dir.y);   // initial angle, counter-clock-wise +

    auto weapon_body = m_world->CreateBody(&def);
    switch (weapon) {
    case Weapon::e_BAZOOKA:
        m_cur_weapon = new Bazooka(weapon_body, m_cur_unit->getPower());
        break;
    default:
        qFatal("Weapon not implemented!");
        break;
    }
    m_scene->addItem(m_cur_weapon);

    connect(m_cur_weapon, SIGNAL(triggered()), this, SLOT(destroyWeapon()));

    m_cur_weapon->aim(strength);
    m_cur_weapon->launch();

    simulateThen(&GameSystem::switchPlayer);
}

Side GameSystem::getCurPlayer() const
{
    return m_cur_player;
}
