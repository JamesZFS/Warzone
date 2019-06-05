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

// half physical world size
extern const qreal world_width;
extern const qreal world_height;

extern const int max_n_unit;
extern const int max_n_brick;

extern const float soldier_density;
extern const float brick_density1;
extern const float brick_density2;
extern const float bazooka_density;
extern const float grenade_density;
extern const float gravity_constant;
extern const quint32 max_n_static_iter; // stop simulating after how many non-changing times

extern const int brick_life1;
extern const int brick_life2;

extern const float brick_friction;

}

#endif // CONSTANTS_H
