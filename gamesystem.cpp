#include "gamesystem.h"
#include "engine.h"
#include "contactlistener.h"
#include "explosioneffect.h"
#include "initializer.h"

#include <QGraphicsScene>
#include <QtMath>

GameSystem::GameSystem() :
    QObject(nullptr),
    m_world(nullptr), m_land(nullptr), m_water_system(nullptr),
    m_cur_unit(nullptr), m_cur_weapon(nullptr), m_cur_player(e_NONE),
    m_game_state(e_COMMON),
    m_contact_listener(new ContactListener),
    m_scene(new QGraphicsScene), m_initializer(nullptr)
{
}

GameSystem::~GameSystem()
{
    delete m_world;
    delete m_contact_listener;
    delete m_scene;
}

void GameSystem::start(const QString &map_name)
{
    emit initializing(true);
    if (s_proxy_engine && s_proxy_engine->isRunning()) {
        qDebug() << "thread still running, waiting for it";
        s_proxy_engine->wait();
    }
    delete s_proxy_engine;
    m_game_state = e_COMMON;
    resetWorld();
    initWorld(map_name);
    // scale to scene x10
    m_scene->setSceneRect(-10 * GameConsts::world_width, -10 * GameConsts::world_height, 20 * GameConsts::world_width, 20 * GameConsts::world_height);
    m_cur_player = e_RED; // let the red operate first
    m_R_iter = m_R_units.constBegin();
    m_B_iter = m_B_units.constBegin();
    setCurUnit(*m_R_iter);
    m_cur_weapon = nullptr;
    s_proxy_engine = new Engine(this, m_world);
    m_kill_list.clear();
    connect(s_proxy_engine, SIGNAL(requiresUpdate()), this, SLOT(advanceScene()), Qt::QueuedConnection);
    connect(s_proxy_engine, SIGNAL(simulationFinished()), this, SLOT(onSimulationFinished()), Qt::DirectConnection);
    asyncSimulateThen(&GameSystem::onInitialized);
}

QGraphicsScene *GameSystem::getScene() const
{
    return m_scene;
}

void GameSystem::syncSimulateThen(FunPtr next)
{
    qDebug("Simulating...");
    m_after_simulation = next;
    m_game_state = e_SIMULAING;   // lock operation
    emit beginSimulating();
    s_proxy_engine->syncSimulate();
}

void GameSystem::asyncSimulateThen(GameSystem::FunPtr next)
{
    qDebug("Skipping...");
    // remake async connection
    m_after_simulation = next;
    m_game_state = e_SIMULAING;   // lock operation
    emit beginSimulating();
    s_proxy_engine->asyncSimulate();
}

void GameSystem::resetWorld()
{
    // clear world
    delete m_world;
    m_world = nullptr;
    delete m_initializer;
    m_initializer = nullptr;
    delete m_land;
    m_land = nullptr;
    m_water_system = nullptr;
    m_R_units.clear();
    m_B_units.clear();
    m_bricks.clear();
    m_cur_unit = nullptr;
    foreach (auto item, m_ext_item) {
        m_scene->removeItem(item);
    }
    m_ext_item.clear();
    m_scene->clear();
}

void GameSystem::initWorld(const QString &map_name)
{
    // create world
    m_world = new b2World(b2Vec2(0, -GameConsts::gravity_constant));
    // link contact listener
    if (map_name == "Fortress")
        m_initializer = new FortressInitializer(m_world);
    else if (map_name == "Classic" || map_name == "")
        m_initializer = new ClassicInitializer(m_world);
    else if (map_name == "Double Deck")
        m_initializer = new DoubleDeckInitializer(m_world);
    else {
        qDebug() << "Got unknown map name: " << map_name;
        qFatal("wrong map name.");
    }
    //else if (map_name == "Double Deck")
    // setup initializer
    m_world->SetContactListener(m_contact_listener);

    // create land
    m_initializer->initLand(m_land);
    Q_ASSERT(m_land);
    m_scene->addItem(m_land);

    // create bricks
    m_initializer->initBricks(m_bricks);
    foreach (auto brick, m_bricks) {
        connect(brick, SIGNAL(died()), this, SLOT(onBrickDied()));
        m_bricks << brick;
        m_scene->addItem(brick);
    }

    // todo generate particles

    // create R units and B units
    m_initializer->initUnits(m_R_units, m_B_units);
    foreach (auto unit, m_R_units) {
        connect(unit, SIGNAL(hurt(int)), this, SLOT(onSoldierHurt(int)));
        connect(unit, SIGNAL(died()), this, SLOT(onSoldierDied()));
        connect(unit, SIGNAL(triggered()), this, SLOT(destroySoldier()));
        m_scene->addItem(unit);
    }
    foreach (auto unit, m_B_units) {
        connect(unit, SIGNAL(hurt(int)), this, SLOT(onSoldierHurt(int)));
        connect(unit, SIGNAL(died()), this, SLOT(onSoldierDied()));
        connect(unit, SIGNAL(triggered()), this, SLOT(destroySoldier()));
        m_scene->addItem(unit);
    }

    LiquidFun::n_particle_iteration =
            m_world->CalculateReasonableParticleIterations(LiquidFun::time_step);
    Q_ASSERT(m_R_units.size() == m_B_units.size() &&
             m_R_units.size() <= GameConsts::max_n_unit);
    qDebug() << "ground and units spwaned! game ready.";
}

//void GameSystem::createLand()

void GameSystem::setoffSoldier(Soldier *unit)
{
    unit->setoff();
}

void GameSystem::setCurUnit(Soldier *unit)
{
    m_cur_unit = unit;
    m_cur_unit->setCurrent(true);
    m_cur_unit->update();
    emit setLabelPrompt("Unit Life");
    emit setLCDNumber(unit->life());
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

void GameSystem::onInitialized()
{
    emit initializing(false);
    waitForOperation();
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
    emit requireOperation(m_cur_player); // wait for next user's operation
    emit playerChanged(m_cur_player);
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
    syncSimulateThen(&GameSystem::waitForOperation);
}

// remove weapon from scene and make explosion effect
void GameSystem::destroyWeapon()
{
    auto weapon = qobject_cast<Weapon*>(sender());
    Q_ASSERT(weapon);
    m_scene->removeItem(weapon);
    s_proxy_engine->discard(weapon);
    // make explosion animation
    auto effect = weapon->createExplosionEffect();  // a polymorphism method
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
    // make explosion effect
    auto effect = unit->createExplosionEffect();
    connect(effect, &ExplosionEffect::animationFinished, this, [this, effect](){
        m_scene->removeItem(effect);
        delete effect;
    });
    m_scene->addItem(effect);
    advanceScene();

    // remove soldier
    m_scene->removeItem(unit);
    s_proxy_engine->discard(unit);  // safe delete
}

void GameSystem::onSimulationFinished()
{
    qDebug() << QString("simulation finished in %1 iters").arg(s_proxy_engine->n_iter());
    advanceScene();
    m_game_state = e_COMMON;
    // deal with dead units
    if (m_kill_list.isEmpty()) {
        m_game_state = e_OPERATIONAL;
        (this->*m_after_simulation)();  // call next method
    }
    else {  // execute one unit per simulation
        Soldier *exec_unit = *m_kill_list.begin();
        m_kill_list.remove(exec_unit);
        exec_unit->setoff();
        syncSimulateThen(m_after_simulation);   // keep exploding until no one killed
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

void GameSystem::onBrickDied()
{
    auto brick = qobject_cast<Brick*>(sender());
    Q_ASSERT(brick);
    qDebug("  a brick died");
    if (!m_bricks.remove(brick)) return;    // avoid dumplicative deletion
    m_scene->removeItem(brick);
    s_proxy_engine->discard(brick);
}

const Soldier *GameSystem::getCurUnit() const
{
    return m_cur_unit;
}

void GameSystem::skipCurUnit()
{
    Q_ASSERT(m_game_state == e_OPERATIONAL);
    m_cur_unit->setCurrent(false);
    m_scene->advance();
    emit setLCDNumber(0);
    switchPlayer();
}

static b2Vec2 rotate(const b2Vec2 &v, qreal deg)
{
    deg = qDegreesToRadians(deg);
    return b2Vec2(v.x * cos(deg) - v.y * sin(deg), v.x * sin(deg) + v.y * cos(deg));
}

void GameSystem::fireCurUnit(Weapon::Type weapon, const b2Vec2 &strength)
{
    Q_ASSERT(m_game_state == e_OPERATIONAL);
    m_cur_unit->setCurrent(false);
    emit setLCDNumber(0);

    b2BodyDef def;
    b2Vec2 dir = strength / strength.Length();
    def.position = m_cur_unit->getBody()->GetPosition();
    def.position += dir * m_cur_unit->getSize() * 2.5;    // ** offset a little
    def.angle = -qAtan2(dir.x, dir.y);   // initial angle, counter-clock-wise +

    auto weapon_body = m_world->CreateBody(&def);
    switch (weapon) {
    case Weapon::e_BAZOOKA:
        m_cur_weapon = new Bazooka(weapon_body, m_cur_unit->getPower());
        break;
    case Weapon::e_GRENADE:
        m_cur_weapon = new Grenade(weapon_body, m_cur_unit->getPower(), 3, s_proxy_engine);
        break;
    case Weapon::e_SHOTGUN: {   // three pieces
        static qreal angles[3] = {0, 6, -6};
        for (int i = 0; i < 3; ++i) {
            def.position = m_cur_unit->getBody()->GetPosition() + rotate(dir, angles[i]) * m_cur_unit->getSize() * 5;
            m_cur_weapon = new Shotgun(m_world->CreateBody(&def), m_cur_unit->getPower());
            m_cur_weapon->setCollisionFilter(m_cur_player);
            m_scene->addItem(m_cur_weapon);
            connect(m_cur_weapon, SIGNAL(triggered()), this, SLOT(destroyWeapon()));
            m_cur_weapon->aim(rotate(strength, angles[i]));
            m_cur_weapon->launch();
        }
        break;
    }
    default:
        qFatal("Weapon not implemented!");
        break;
    }
    if (weapon != Weapon::e_SHOTGUN) {
        m_cur_weapon->setCollisionFilter(m_cur_player); // shouldn't collide with cur_side's fortress
        m_scene->addItem(m_cur_weapon);
        connect(m_cur_weapon, SIGNAL(triggered()), this, SLOT(destroyWeapon()));
        m_cur_weapon->aim(strength);
        m_cur_weapon->launch();
    }
    syncSimulateThen(&GameSystem::switchPlayer);
}

Side GameSystem::getCurPlayer() const
{
    return m_cur_player;
}
