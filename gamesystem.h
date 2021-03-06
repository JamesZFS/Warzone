#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include "defs.h"
#include "soldier.h"
#include "land.h"
#include "weapon.h"
#include "brick.h"

#include <QObject>
#include <QSet>

class Engine;
class QGraphicsScene;
class ContactListener;
class Initializer;

class GameSystem : public QObject
{
    Q_OBJECT
public:
    typedef void (GameSystem::*FunPtr)();

    enum GameState
    {
        e_COMMON,   // neither operational nor during a simualtion
        e_OPERATIONAL,
        e_SIMULAING,
        e_PAUSED,
    };

    GameSystem();
    ~GameSystem();

    void start(const QString &map_name);

    // step the system (with updation) until next key event (when everything is static)
    void syncSimulateThen(FunPtr next);
    // step fast without updation
    void asyncSimulateThen(FunPtr next);

    // operations:
    void moveCurUnit(const b2Vec2 &strength);
    void skipCurUnit();
    void fireCurUnit(Weapon::Type weapon, const b2Vec2 &strength);

    // getters:
    Side getCurPlayer() const;
    QGraphicsScene *getScene() const;
    GameState getGamestate() const;
    bool isOperational() const;

    // scene extension:
    void addToScene(QGraphicsItem *item);
    void removeFromScene(QGraphicsItem *item);

    const Soldier *getCurUnit() const;

signals:
    void initializing(bool flag);
    void unitKilled(QString msg);
    void unitHurt(int damage);
    void requireOperation(Side side);
    void playerChanged(Side side);
    void beginSimulating();
    void gameOver(Side winner);
    void setLabelPrompt(QString prompt);
    void setLCDNumber(int number);

protected:
    void resetWorld();
    void initWorld(const QString &map_name);    // called right after start()
    void setoffSoldier(Soldier *unit);
    void setCurUnit(Soldier *unit);
    Side checkWhoWins();  // if either side runs out of soldiers, then the other side wins
    void setGameOver(Side winner);

protected slots:
    void advanceScene();
    void onInitialized();
    void waitForOperation();
    void switchPlayer();  // switch turn to next player with unit automatically selected
    void destroyWeapon();
    void destroySoldier();
    void onSimulationFinished();  // called right after simulation
    void onSoldierHurt(int damage);
    void onSoldierDied();
    void onBrickDied();

private:
    // world members:
    b2World *m_world;
    Land *m_land;
    // red side v.s. black side
    QSet<RedSoldier *> m_R_units;
    QSet<BlackSoldier *> m_B_units;
    QSet<Brick *> m_bricks;
    b2ParticleSystem *m_water_system;
    Soldier *m_cur_unit;  // current moving unit
    Weapon *m_cur_weapon;   // current selected weapon

    // game params:
    Side m_cur_player;
    QSet<RedSoldier*>::const_iterator m_R_iter;
    QSet<BlackSoldier*>::const_iterator m_B_iter;
    GameState m_game_state;
    ContactListener *m_contact_listener;

    // other members:
    QGraphicsScene *m_scene;  // stage
    Initializer *m_initializer;
    FunPtr m_after_simulation;   // call this after simulation
    QSet<Soldier*> m_kill_list;   // kill soldiers after simulation
    QSet<QGraphicsItem *> m_ext_item;  // user defined items, will not be cleared
};

#endif // GAMESYSTEM_H
