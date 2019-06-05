#include "engine.h"
#include "defs.h"
#include "actor.h"
#include <Box2D/Box2D.h>
#include <QTimer>

Engine *s_proxy_engine;  // static engine

Engine::Engine(QObject *parent, b2World *world) :
    QThread(parent), m_world(world), m_timer(nullptr), m_always_flag(false),
    m_busy(false), m_skip(false)
{
    Q_ASSERT(world);
    connect(this, &QThread::finished, this, [this](){
        qDebug() << "catch finished() signal!";
        emit simulationFinished();
    }, Qt::QueuedConnection);
}

Engine::~Engine()
{
    delete m_timer;
}

void Engine::requestImpulse(ImpulseRequest *request)
{
    m_impulse_requests.push(request);
}

void Engine::requestTransform(TransformRequest *request)
{
    m_transform_requests.push(request);
}

void Engine::discard(Actor *actor)
{
    m_trash.push(actor);
}

void Engine::syncSimulate()
{
    Q_ASSERT(!m_busy);
    start();
}

void Engine::asyncSimulate() // in main thread!
{
    Q_ASSERT(!m_busy);
    Q_ASSERT(!m_always_flag);
    m_skip = true;
    start();
}

void Engine::enableAlways()
{
    m_always_flag = true;
}

void Engine::disableAlways()
{
    m_always_flag = false;
}

void Engine::setSleep()
{
    for (b2Body *body = m_world->GetBodyList(); body; body = body->GetNext()) {
        body->SetAwake(false);
    }
}

void Engine::finish()
{
    qDebug() << "in finish()" << QThread::currentThread();
    setSleep();
    m_busy = false;
}

void Engine::stepWorld()    // called in thread
{
    Q_ASSERT(!m_skip);
    dumpTrash();
    doRequests();
    if (!worldIsChanging() && m_n_iter > 0) {   // everything is static, stop simulation
        m_timer->stop();
        m_timer->deleteLater();
        m_timer = nullptr;
        finish();
        exit(0); // emit finished()
        return;
    }
    ++m_n_iter;
    m_world->Step(LiquidFun::time_step,
                  LiquidFun::n_velocity_iteration,
                  LiquidFun::n_position_iteration);
    emit requiresUpdate();  // will call parent's paint event
}

bool Engine::worldIsChanging()  // called in thread
{
    if (m_always_flag) return true;
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

void Engine::doRequests()   // called in thread
{
    while (!m_impulse_requests.empty()) {
        auto request = m_impulse_requests.pop();
        request->body->ApplyLinearImpulse(request->impulse, request->body->GetPosition(), true);
        delete request;
    }
    while (!m_transform_requests.empty()) {
        auto request = m_transform_requests.pop();
        request->body->SetTransform(request->pos, request->angle);
        delete request;
    }
}

void Engine::dumpTrash()    // called in thread
{
    while (!m_trash.isEmpty()) {
        auto actor = m_trash.pop();
        m_world->DestroyBody(actor->body());
        delete actor;
    }
}

quint32 Engine::n_iter() const
{
    return m_n_iter;
}

bool Engine::busy() const
{
    return m_busy;
}

void Engine::run()  // *** in thread ***
{
    qDebug() << "in run()" << QThread::currentThread();
    Q_ASSERT(!m_timer);
    Q_ASSERT(!m_busy);
    m_n_iter = 0;
    m_busy = true;
    if (m_skip) {  // skipping mode
        while (worldIsChanging() || m_n_iter == 0) {
            dumpTrash();
            doRequests();
            ++m_n_iter;
            m_world->Step(LiquidFun::time_step,
                          LiquidFun::n_velocity_iteration,
                          LiquidFun::n_position_iteration);
        }
        finish();
        m_skip = false;
        return;
    }
    // normal mode
    m_timer = new QTimer;
    m_timer->setInterval(LiquidFun::time_step * 1000.0);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(stepWorld()), Qt::DirectConnection);
    m_timer->start();
    this->exec(); // wait for timing stops
}
