#include "constants.h"

namespace LiquidFun {

const int n_velocity_iteration = 8;
const int n_position_iteration = 3;
const float time_step = 1.0 / 60.0;
int n_particle_iteration = -1;

}

namespace GameConsts {

const float soldier_density = 1.05;
const float bazooka_density = 8.0;
const float grenade_density = 3.0;
const int max_n_unit = 5;
const float gravity_constant = 10.0;
const quint32 max_n_static_iter = 2;

}
