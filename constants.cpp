#include "constants.h"

namespace LiquidFun {

const int n_velocity_iteration = 8;
const int n_position_iteration = 3;
const float time_step = 1.0 / 60.0;
int n_particle_iteration = -1;

}

namespace GameConsts {

const qreal world_width = 80;
const qreal world_height = 50;

const int max_n_unit = 5;
const int max_n_brick = 60;

const float soldier_density = 1.05;
const float brick_density1 = 0.5;
const float brick_density2 = 3.0;

const float bazooka_density = 8.0;
const float grenade_density = 2.0;
const float bullet_density = 1.0;
const float gravity_constant = 20.0;
const quint32 max_n_static_iter = 2;

const int brick_life1 = 30;
const int brick_life2 = 100;

const float brick_friction = 0.7;

const float bazooka_velocity = 90;
const float grenade_velocity = 60;
const float bullet_velocity = 5000;
const int max_weapon_id = 3;

}
