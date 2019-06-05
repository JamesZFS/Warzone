#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QStack>

class b2World;
class Actor;
class QTimer;

// used as a temporary simulation thread
class Engine : public QObject
{
    Q_OBJECT
public:
    Engine(QObject *parent, b2World *world);
    ~Engine();

    void discard(Actor *actor);
    bool busy() const;

signals:
    void requiresUpdate();
    void finished(quint32);

public slots:
    void doSimulation();    // entrance, parallel
    void doSimulationSequential();    // sequential
    void enableAlways();     // keep simulation even if world is not changing
    void disableAlways();     // return to normal
    void setSleep();    // set all bodies to sleeping state
    void onFinished();

private slots:
    void stepWorld();

private:
    bool worldIsChanging();
    void dumpTrash();

    b2World *m_world;
    QTimer *m_timer;
    quint32 m_n_iter;
    quint32 m_n_static_iter;
    QStack<Actor*> m_trash; // will release these bodies after simulation
    bool m_always_flag; // whether to keep simulation even if world is not changing
    bool m_busy;
};

#endif // ENGINE_H
