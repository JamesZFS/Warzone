#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QStack>

class b2World;
class b2Body;
class QTimer;

// used as a temporary simulation thread
class Engine : public QObject
{
    Q_OBJECT
public:
    Engine(QObject *parent, b2World *world);
    ~Engine();

    void discardBody(b2Body *body);

signals:
    void requiresUpdate();
    void finished(quint32);

public slots:
    void doSimulation();    // entrance

private slots:
    void stepWorld();

private:
    bool worldIsChanging() const;
    void dumpTrash();

    b2World *m_world;
    QTimer *m_timer;
    quint32 m_n_iter;
    QStack<b2Body*> m_body_trash; // will release these bodies after simulation
};

#endif // ENGINE_H
