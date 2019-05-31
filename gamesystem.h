#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include <QObject>
#include "defs.h"
#include "soldier.h"
#include "land.h"
#include "weapon.h"

class Engine;
class QGraphicsScene;
class ContactListener;

class GameSystem : public QObject
{
    Q_OBJECT
public:
    typedef void (GameSystem::*FunPtr)();

    enum GameState
    {
        e_COMMON,   // before game start
        e_OPERATIONAL,
        e_SIMULAING,
        e_PAUSED,
    };

    GameSystem();
    ~GameSystem();

    void start();
    void end();

    // step the system until next key event (when everything is static)
    void simulateThen(FunPtr next);

    // operations:
    void moveCurUnit(const b2Vec2 &strength);
    void fireCurUnit(Weapon::Type weapon, const b2Vec2 &strength);

    // getters:
    Side getCurPlayer() const;
    QGraphicsScene *getScene() const;
    GameState getGamestate() const;
    bool isOperational() const;

signals:
    void unitKilled(QString msg);
    void unitHurt(int damage);
    void requireOperation(Side side);
    void playerChanged(Side side);
    void beginSimulating();

protected:
    void resetWorld();
    void initWorld();    // called right after start()
    void createLand();
    void createSoldier(const SoldierDef &unit_def);
    void setoffSoldier(Soldier *unit);

protected slots:
    void advanceScene();
    void waitForOperation();
    void switchPlayer();  // switch turn to next player with unit automatically selected
    void destroyWeapon();
    void destroySoldier();
    void onSimulationFinished(quint32 n_iter);  // called right after simulation
    void onSoldierHurt(int damage);
    void onSoldierDied();

private:
    // world members:
    b2World *m_world;
    Land *m_land;
    // red side v.s. black side
    QVector<RedSoldier *> m_R_units;
    QVector<BlackSoldier *> m_B_units;
    b2ParticleSystem *m_water_system;
    Soldier *m_cur_unit;  // current moving unit
    Weapon *m_cur_weapon;   // current selected weapon

    // game params:
    Side m_cur_player;
    size_t m_prev_R_unit_index, m_prev_B_unit_index;
    qreal m_world_size;  // half physical world size, the world is ranged (-size, size)^2
    GameState m_game_state;
    Engine *m_proxy_engine;  // proxy simulator
    ContactListener *m_contact_listener;

    QGraphicsScene *m_scene;  // stage

    FunPtr m_after_simulation;   // call this after simulation

    QSet<Soldier*> m_kill_list;   // kill soldiers after simulation
};

#endif // GAMESYSTEM_H
