#include "engine.h"
#include "defs.h"
#include "actor.h"
#include <Box2D/Box2D.h>
#include <QTimer>

Engine::Engine(QObject *parent, b2World *world) :
    QObject(parent), m_world(world), m_timer(new QTimer)
{
    Q_ASSERT(world);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(stepWorld()));
}

Engine::~Engine()
{
    m_timer->deleteLater();
}

void Engine::discard(Actor *actor)
{
    m_trash.push(actor);
}

void Engine::doSimulation()
{
    m_n_iter = 0;
    m_timer->start(LiquidFun::time_step * 1000.0);
}

void Engine::stepWorld()
{
    dumpTrash();
    if (!worldIsChanging() && m_n_iter) {   // everything is static, stop simulation
        m_timer->stop();
        emit finished(m_n_iter);
        return;
    }
    ++m_n_iter;
    m_world->Step(LiquidFun::time_step,
                  LiquidFun::n_velocity_iteration,
                  LiquidFun::n_position_iteration);
    emit requiresUpdate();  // will call parent's paint event
}

bool Engine::worldIsChanging()
{
    for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
        if (body->GetType() == b2_staticBody ||
                !body->IsActive() ||
                !body->IsAwake()) continue;
        if (body->GetLinearVelocity().LengthSquared() > b2_epsilon) {
            m_n_static_iter = 0;
            return true;    // the world got something still moving
        }
    }
    if (++m_n_static_iter >= GameConsts::max_n_static_iter) {
        m_n_static_iter = 0;
        return false;
    }
    else
        return true;
}

void Engine::dumpTrash()
{
    while (!m_trash.isEmpty()) {
        auto actor = m_trash.pop();
        m_world->DestroyBody(actor->body());
        delete actor;
    }
}
