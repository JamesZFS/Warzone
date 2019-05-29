#include "engine.h"
#include "defs.h"
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

void Engine::doSimulation()
{
    m_timer->start(LiquidFun::time_step * 1000.0);
}

void Engine::stepWorld()
{
    if (!worldIsChanging()) {   // everything is static, stop simulation
        m_timer->stop();
        emit finished();
        return;
    }
    m_world->Step(LiquidFun::time_step,
                  LiquidFun::n_velocity_iteration,
                  LiquidFun::n_position_iteration);
    emit requiresUpdate();  // will call parent's paint event
}

bool Engine::worldIsChanging() const
{
    for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
        if (body->GetType() == b2_staticBody ||
                !body->IsActive() ||
                !body->IsAwake()) continue;
        if (body->GetLinearVelocity().LengthSquared() > b2_epsilon)
            return true;    // the world got something still moving
    }
    return false;
}
