#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include <QObject>
#include "defs.h"
#include "soldier.h"
#include "land.h"

class Engine;
class QGraphicsScene;

class GameSystem : public QObject
{
    Q_OBJECT
public:
    enum GameStatus
    {
        e_COMMON,   // before game start
        e_OPERATIONAL,
        e_SIMULAING,
        e_PAUSED,
    };
    enum Weapons    // todo, use class definition
    {
        e_BAZOOKA,
        e_GRENADE,
        e_SHOTGUN,
    };

    GameSystem();

    void start();

    void end();

    // step the system until next key event (when everything is static)
    void simulate();

    Side getCurPlayer() const;

    GameStatus getGamestatus() const;

    bool isOperational() const;

    void moveCurUnit(const b2Vec2 &strength);

    void fireCurUnit(Weapons weapon, b2Vec2 strength);

    QGraphicsScene *getScene() const;

signals:
//    void requiresUpdate();  // tell parent to update screen

    void unitKilled(QString msg);

    void requireOperation(Side side);

    void playerChanged(Side side);

protected:
    void resetWorld();

    void initWorld();    // called right after start()

    void createLand();

    void createSoldier(const SoldierDef &unit_def);

    void killSoldier(Soldier *unit);

protected slots:
    void advanceScene();

    void waitForOperation();

    void nextPlayer();  // switch turn to next player with unit automatically selected

private:
    // world members:
    b2World *world;
    Land *land;
    QVector<Soldier *> R_units, B_units;    // red side v.s. black side
    b2ParticleSystem *water_system;
    Soldier *cur_unit;  // current moving unit

    // game params:
    Side cur_player;
    size_t prev_R_unit_index, prev_B_unit_index;

    double world_size;  // half world size, the world is ranged (-size, size)^2

    GameStatus gamestatus;

    Engine *proxy_engine;  // proxy simulator

    QGraphicsScene *scene;  // stage
};

#endif // GAMESYSTEM_H
