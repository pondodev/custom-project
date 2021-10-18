#ifndef PLAYER_H
#define PLAYER_H

#include "vec2.h"

struct Player {
    Vec2 position;
    float view_angle;
    float fov;
    Vec2 move_dir;
};

#endif
