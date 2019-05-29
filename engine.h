#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>

class b2World;
class QTimer;

// used as a temporary simulation thread
class Engine : public QObject
{
    Q_OBJECT
public:
    Engine(QObject *parent, b2World *world);

    ~Engine();

signals:
    void requiresUpdate();
    void finished();

public slots:
    void doSimulation();    // entrance

private slots:
    void stepWorld();

private:
    bool worldIsChanging() const;

    b2World *m_world;
    QTimer *m_timer;
};

#endif // ENGINE_H
