#ifndef ENGINE_H
#define ENGINE_H

#include <QStack>
#include <QThread>
#include <Box2D/Common/b2Math.h>

class b2World;
class b2Body;
class Actor;
class QTimer;

struct ImpulseRequest   // async impulse request
{
    ImpulseRequest(b2Body *body_, const b2Vec2 &impulse_) : body(body_), impulse(impulse_) {}
    b2Body *body;
    b2Vec2 impulse;
};

struct TransformRequest
{
    TransformRequest(b2Body *body_, const b2Vec2 &pos_, float32 angle_) :
        body(body_), pos(pos_), angle(angle_) {}
    b2Body *body;
    b2Vec2 pos;
    float32 angle;
};

// used as a temporary simulation thread
class Engine : public QThread
{
    Q_OBJECT
public:
    Engine(QObject *parent, b2World *world);
    ~Engine();

    void requestImpulse(ImpulseRequest *request);
    void requestTransform(TransformRequest *request);
    void discard(Actor *actor);
    void syncSimulate();  //  entrance1
    void asyncSimulate();  //  entrance2

    bool busy() const;
    quint32 n_iter() const;

signals:
    void requiresUpdate();
    void simulationFinished();  // don't use QThread::finished()

public slots:
    void enableAlways();     // keep simulation even if world is not changing
    void disableAlways();     // return to normal
    void setSleep();    // set all bodies to sleeping state
    void finish();

private slots:
    void stepWorld();

private:
    bool worldIsChanging();
    void doRequests();
    void dumpTrash();
    void start() { QThread::start(); }     // hiding

    b2World *m_world;
    QTimer *m_timer;
    quint32 m_n_iter;
    quint32 m_n_static_iter;
    QStack<Actor*> m_trash; // will release these bodies after simulation
    QStack<ImpulseRequest*> m_impulse_requests;
    QStack<TransformRequest*> m_transform_requests;
    bool m_always_flag; // whether to keep simulation even if world is not changing
    bool m_busy;
    bool m_skip;

    // QThread interface
protected:
    void run() override;
};

extern Engine *s_proxy_engine;  // static engine

#endif // ENGINE_H
