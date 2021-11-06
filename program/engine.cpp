#include "engine.h"

Engine::Engine( std::string map_path, std::string wall_tex_path, std::string enemy_tex_path )
    : wall_textures( wall_tex_path ), enemy_textures( enemy_tex_path ) {
    // read in map
    std::fstream f;
    f.open( map_path, std::ios::in );
    std::string width_str, height_str;
    std::getline( f, width_str );
    std::getline( f, height_str );
    map_width = std::stoi( width_str );
    map_height = std::stoi( height_str );
    char tile;
    while ( f >> std::noskipws >> tile ) {
        switch ( tile ) {
            case '\n': // ignore
                break;

            case '_':
                map.push_back( Floor );
                break;

            case '#':
                map.push_back( Wall1 );
                break;

            case '$':
                map.push_back( Wall2 );
                break;

            case '%':
                map.push_back( Wall3 );
                break;

            default:
                map.push_back( Floor );
                std::cerr << "unrecognised tile: " << tile << std::endl;
                break;
        }
    }

    f.close();

    player = Player {
        Vec2 { 2.0, 7.0 },
        1.0,
        M_PI / 3.0
    };

    add_enemy( 4.0, 8.5, 1.0, EnemyType::HotHaw );
    add_enemy( 2.5, 9.0, 1.0, EnemyType::FlushedHaw );
    add_enemy( 5.0, 10.0, 1.0, EnemyType::YeeHaw );
    add_enemy( 5.5, 9.0, 1.0, EnemyType::YeeHaw );
}

void Engine::update( float delta_time ) {
    Vec2 forward = Vec2 {
        std::cos( player.view_angle ),
        std::sin( player.view_angle )
    };
    Vec2 right = Vec2 {
        std::cos( player.view_angle - float(M_PI / 2.0) ),
        std::sin( player.view_angle - float(M_PI / 2.0) )
    };

    player_move_dir_lock.lock();
    player_view_lock.lock();

    // apply player movement
    auto old_pos = player.position;
    auto move_vec = forward * player.move_dir.y * delta_time;
    move_vec += right * player.move_dir.x * delta_time;
    player.position += move_vec;

    // TODO: we still get crashes from going inside of walls
    if ( get_map_tile( int(player.position.x), int(old_pos.y) ) != Floor ) {
        float wall_start = floor( player.position.x );
        if ( move_vec.x < float(0) ) wall_start += 1.0;
        else wall_start -= 0.05;
        player.position.x += wall_start - player.position.x;
    }

    if ( get_map_tile( int(old_pos.x), int(player.position.y) ) != Floor ) {
        float wall_start = floor( player.position.y );
        if ( move_vec.y < 0 ) wall_start += 1.0;
        else wall_start -= 0.05;
        player.position.y += wall_start - player.position.y;
    }

    player_move_dir_lock.unlock();
    player_view_lock.unlock();
}

void Engine::render() {
    framebuffer_lock.lock();
    clear_framebuffer( Color( 0xBBBBBBFF ) );

    const size_t rect_w = WINDOW_WIDTH / (map_width * 2);
    const size_t rect_h = WINDOW_HEIGHT / map_height;

    // draw map
    for ( int y = 0; y < map_height; y++ ) {
        for ( int x = 0; x < map_width; x++ ) {
            Color col;
            auto tile = get_map_tile( x, y );
            switch ( tile ) {
                case Floor:
                    col = Color( 0xBBBBBBFF );
                    break;

                case Wall1:
                case Wall2:
                case Wall3:
                    col = wall_textures.get_pixel( 0, 0, tile );
                    break;

                default:
                    col = Color( 0x00FFFFFF );
                    break;
            }

            int rect_x, rect_y;
            rect_x = x * rect_w;
            rect_y = y * rect_h;
            draw_rect( rect_x, rect_y, rect_w, rect_h, col );
        }
    }

    // draw view cone and 3d view
    for ( int i = 0; i < WINDOW_WIDTH / 2; i++ ) {
        float angle = player.view_angle - player.fov / 2 + player.fov * i / float(WINDOW_WIDTH / 2);
        for ( float ray_dist = 0; ray_dist < 20; ray_dist += .01 ) {
            float cx = player.position.x + ray_dist * cos( angle );
            float cy = player.position.y + ray_dist * sin( angle );

            // view cone
            draw_pixel( cx * rect_w, cy * rect_h, Color( 0x5555DDFF ) );

            // the 3d magic!
            auto tile = get_map_tile( int(cx), int(cy) );
            if ( tile == Floor ) continue; // skip empty space

            float dist = ray_dist * cos( angle - player.view_angle );
            depth_buffer[ i ] = dist;
            int column_height = WINDOW_HEIGHT / dist;

            // wall texturing
            float hit_x = cx - floor( cx + .5 );
            float hit_y = cy - floor( cy + .5 );
            int x_texcoord = hit_x * wall_textures.get_size();
            if ( std::abs( hit_y ) > std::abs( hit_x ) ) { // check if vertical or horizontal wall
                x_texcoord = hit_y * wall_textures.get_size();
            }

            if ( x_texcoord < 0 ) x_texcoord += wall_textures.get_size();

            auto column = wall_textures.get_column( column_height, tile, x_texcoord );
            int pixel_x = i + WINDOW_WIDTH / 2;
            for ( size_t j = 0; j < column_height; j++ ) {
                int pixel_y = j + WINDOW_HEIGHT / 2 - column_height / 2;
                if ( pixel_y < 0 || pixel_y >= WINDOW_HEIGHT ) continue;
                draw_pixel( pixel_x, pixel_y, column[ j ] );
            }

            break;
        }
    }

    // draw the enemies
    // TODO: enemy movement system
    for ( auto &e : active_enemies ) {
        auto move_comp = enemy_manager.get_movement_component( e );
        auto dist_comp = enemy_manager.get_distance_component( e );

        float enemy_dist = std::sqrt( pow( player.position.x - move_comp->x, 2 ) + pow( player.position.y - move_comp->y, 2 ) );
        dist_comp->distance = enemy_dist;
    }

    std::sort( active_enemies.begin(), active_enemies.end(),
        [this]( Entity a, Entity b ) {
            auto a_dist = enemy_manager.get_distance_component( a );
            auto b_dist = enemy_manager.get_distance_component( b );
            return a_dist->distance > b_dist->distance;
        } );

    for ( auto e : active_enemies ) {
        auto move_comp = enemy_manager.get_movement_component( e );
        draw_rect( move_comp->x * rect_w, move_comp->y * rect_h, 5, 5, Color( 0xFF0000FF ) );
        draw_sprite( e );
    }

    framebuffer_lock.unlock();
}

void Engine::get_framebuffer( uint8_t* target ) {
    framebuffer_lock.lock();

    for ( int i = 0; i < FRAMEBUFFER_LENGTH; i++ ) {
        uint8_t r, g, b, a;
        framebuffer[ i ].get_components( r, g, b, a );
        target[ i * 4 ] = r;
        target[ i * 4 + 1 ] = g;
        target[ i * 4 + 2 ] = b;
        target[ i * 4 + 3 ] = a;
    }

    framebuffer_lock.unlock();
}

void Engine::move_view( float delta ) {
    player_view_lock.lock();
    player.view_angle += delta;
    player_view_lock.unlock();
}

void Engine::set_player_move_dir( Vec2 dir ) {
    player_move_dir_lock.lock();
    player.move_dir = dir;
    player_move_dir_lock.unlock();
}

void Engine::draw_rect( int x, int y, int w, int h, Color color ) {
    for ( int i = 0; i < w; i++ ) {
        for ( int j = 0; j < h; j++ ) {
            int cx = x + i;
            int cy = y + j;
            draw_pixel( cx, cy, color );
        }
    }
}

void Engine::draw_sprite( Entity enemy ) {
    auto move_comp = enemy_manager.get_movement_component( enemy );
    auto dist_comp = enemy_manager.get_distance_component( enemy );
    auto type_comp = enemy_manager.get_enemy_type_component( enemy );

    float dir = atan2( move_comp->y - player.position.y, move_comp->x - player.position.x );
    while ( dir - player.view_angle > M_PI ) dir -= 2 * M_PI;
    while ( dir - player.view_angle < -M_PI ) dir += 2 * M_PI;

    size_t sprite_size = std::min( 1000, static_cast<int>( WINDOW_HEIGHT / dist_comp->distance ) );
    int h_offset = (dir - player.view_angle) / player.fov * (WINDOW_WIDTH / 2) + (WINDOW_WIDTH / 4) - (enemy_textures.get_size() / 2);
    h_offset -= sprite_size / 2; // center the sprite
    int v_offset = WINDOW_HEIGHT / 2 - sprite_size / 2;

    for ( size_t i = 0; i < sprite_size; i++ ) {
        if ( h_offset + int(i) < 0 || h_offset + i >= WINDOW_WIDTH / 2 ) continue;
        if ( depth_buffer[ h_offset + i ] < dist_comp->distance ) continue; // occlude sprite
        for ( size_t j = 0; j < sprite_size; j++ ) {
            if ( v_offset + int(j) < 0 || v_offset + j >= WINDOW_HEIGHT ) continue;
            auto col = enemy_textures.get_pixel( i * enemy_textures.get_size() / sprite_size, j * enemy_textures.get_size() / sprite_size, type_comp->type );
            if ( (col.get_hex() & 0x000000FF) < 0x00000080 ) continue; // very simple alpha culling
            int x, y;
            x = WINDOW_WIDTH / 2 + h_offset + i;
            y = v_offset + j;
            draw_pixel( x, y, col );
        }
    }
}

void Engine::clear_framebuffer( Color color ) {
    draw_rect( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, color );
}

void Engine::draw_pixel( int x, int y, Color color ) {
    framebuffer[ x + y * WINDOW_WIDTH ] = color;
}

MapTile Engine::get_map_tile( int x, int y ) {
    return map[ x + y * map_width ];
}

void Engine::add_enemy( float x, float y, float speed, EnemyType type ) {
    auto id = enemy_manager.register_entity();
    if ( id.has_value() ) {
        auto id_val = id.value();
        auto move_comp = enemy_manager.get_movement_component( id_val );
        auto type_comp = enemy_manager.get_enemy_type_component( id_val );

        *move_comp = { x, y };
        *type_comp = { type };

        active_enemies.push_back( id.value() );
    }
}
