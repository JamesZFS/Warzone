#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>

struct b2BodyDef;
struct b2FixtureDef;

// LiquidFun parameters
namespace LiquidFun {

extern const int n_velocity_iteration;
extern const int n_position_iteration;
extern int n_particle_iteration;
extern const float time_step;

}

namespace GameConsts {

extern const float soldier_density;
extern const float bazooka_density;
extern const int max_n_unit;
extern const float gravity_constant;
extern const quint32 max_n_static_iter;

}

#endif // CONSTANTS_H
