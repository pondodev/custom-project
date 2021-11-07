#ifndef ENGINE_H
#define ENGINE_H

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 512
#define FRAMEBUFFER_LENGTH WINDOW_WIDTH * WINDOW_HEIGHT

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <cmath>
#include <array>
#include <algorithm>
#include <mutex>

#include "color.h"
#include "player.h"
#include "texture.h"
#include "entity_engine.h"

enum MapTile {
    Floor = -1,
    Wall1 = 0,
    Wall2 = 1,
    Wall3 = 2,
    Wall4 = 3
};

class Engine {
public:
    Engine( const std::string map_path, const std::string wall_tex_path, const std::string enemy_tex_path );
    void update( const float delta_time );
    void render();
    void get_framebuffer( uint8_t* target );

    void move_view( const float delta );
    void set_player_move_dir( const Vec2 dir );

private:
    Color framebuffer[ FRAMEBUFFER_LENGTH ];
    std::vector<MapTile> map;
    unsigned int map_width;
    unsigned int map_height;
    Player player;
    Texture wall_textures;
    Texture enemy_textures;
    EntityEngine enemy_manager;
    std::vector<Entity> active_enemies;
    std::array<float, WINDOW_WIDTH> depth_buffer;

    std::mutex framebuffer_lock;
    std::mutex player_view_lock;
    std::mutex player_move_dir_lock;

    void clear_framebuffer( const Color color );
    void draw_rect( const int x, const int y, const int w, const int h, const Color color );
    void draw_sprite( const Entity enemy );
    void draw_pixel( const int x, const int y, const Color color );

    MapTile get_map_tile( const int x, const int y ) const;
    void add_enemy( const float x, const float y, const float speed, const EnemyType type );
    void enemy_movement_system( const float delta_time );
};

#endif
