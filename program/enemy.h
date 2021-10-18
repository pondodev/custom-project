#ifndef ENEMY_H
#define ENEMY_H

#include "vec2.h"

struct Enemy {
    Vec2 position;
    size_t tex_id;
    float dist_from_player;
};

#endif
