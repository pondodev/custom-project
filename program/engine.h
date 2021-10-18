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
#include "enemy.h"
#include "texture.h"

enum MapTile {
    Floor = -1,
    Wall1 = 0,
    Wall2 = 1,
    Wall3 = 2,
    Wall4 = 3
};

class Engine {
public:
    Engine( std::string map_path, std::string wall_tex_path, std::string enemy_tex_path );
    void update( float delta_time );
    void render();
    void get_framebuffer( uint8_t* target );

    void move_view( float delta );
    void set_player_move_dir( Vec2 dir );

private:
    Color framebuffer[ FRAMEBUFFER_LENGTH ];
    std::vector<MapTile> map;
    unsigned int map_width;
    unsigned int map_height;
    Player player;
    Texture wall_textures;
    Texture enemy_textures;
    std::vector<Enemy> enemies;
    std::array<float, WINDOW_WIDTH> depth_buffer;
    std::mutex framebuffer_lock;
    std::mutex player_view_lock;
    std::mutex player_move_dir_lock;

    void clear_framebuffer( Color color );
    void draw_rect( int x, int y, int w, int h, Color color );
    void draw_sprite( Enemy enemy );
    void draw_pixel( int x, int y, Color color );

    MapTile get_map_tile( int x, int y );
};

#endif
