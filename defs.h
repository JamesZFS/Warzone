#ifndef DEFS_H
#define DEFS_H

#include <vector>
#include <queue>
#include <QDebug>
#include "constants.h"

using std::vector;
using std::queue;

inline int randint(int lo, int hi)
{
    return lo + qrand() % (hi - lo);
}

inline double randf(double lo, double hi)
{
    return lo + (hi - lo) * qrand() / RAND_MAX;
}

//// LiquidFun fwd definitions
//class b2World;
//class b2Body;
////class b2BodyDef;
//class b2Fixture;
////class b2FixtureDef;
//class b2ParticleSystem;
////class b2ParticleSystemDef;
//class b2CircleShape;
//class b2PolygonShape;
//class b2Contact;
//class b2ContactListener;

#endif // DEFS_H
