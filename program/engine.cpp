#include "engine.h"

Engine::Engine( const std::string map_path, const std::string wall_tex_path, const std::string enemy_tex_path )
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

    add_enemy( 4.0, 8.5, 0.5, EnemyType::HotHaw );
    add_enemy( 2.5, 9.0, 0.5, EnemyType::FlushedHaw );
    add_enemy( 5.0, 10.0, 0.5, EnemyType::YeeHaw );
    add_enemy( 5.5, 9.0, 0.5, EnemyType::YeeHaw );
}

void Engine::update( const float delta_time ) {
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

    enemy_movement_system( delta_time );

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
            const auto tile = get_map_tile( x, y );
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
        const float angle = player.view_angle - player.fov / 2 + player.fov * i / float(WINDOW_WIDTH / 2);
        for ( float ray_dist = 0; ray_dist < 20; ray_dist += .01 ) {
            const float cx = player.position.x + ray_dist * cos( angle );
            const float cy = player.position.y + ray_dist * sin( angle );

            // view cone
            draw_pixel( cx * rect_w, cy * rect_h, Color( 0x5555DDFF ) );

            // the 3d magic!
            const auto tile = get_map_tile( int(cx), int(cy) );
            if ( tile == Floor ) continue; // skip empty space

            const float dist = ray_dist * cos( angle - player.view_angle );
            depth_buffer[ i ] = dist;
            const int column_height = WINDOW_HEIGHT / dist;

            // wall texturing
            const float hit_x = cx - floor( cx + .5 );
            const float hit_y = cy - floor( cy + .5 );
            int x_texcoord = hit_x * wall_textures.get_size();
            if ( std::abs( hit_y ) > std::abs( hit_x ) ) { // check if vertical or horizontal wall
                x_texcoord = hit_y * wall_textures.get_size();
            }

            if ( x_texcoord < 0 ) x_texcoord += wall_textures.get_size();

            const auto column = wall_textures.get_column( column_height, tile, x_texcoord );
            const int pixel_x = i + WINDOW_WIDTH / 2;
            for ( size_t j = 0; j < column_height; j++ ) {
                const int pixel_y = j + WINDOW_HEIGHT / 2 - column_height / 2;
                if ( pixel_y < 0 || pixel_y >= WINDOW_HEIGHT ) continue;
                draw_pixel( pixel_x, pixel_y, column[ j ] );
            }

            break;
        }
    }

    // draw the enemies
    std::sort( active_enemies.begin(), active_enemies.end(),
        [this]( Entity a, Entity b ) {
            const auto a_dist = enemy_manager.get_distance_component( a );
            const auto b_dist = enemy_manager.get_distance_component( b );
            return a_dist->distance > b_dist->distance;
        } );

    for ( auto& e : active_enemies ) {
        const auto move_comp = enemy_manager.get_movement_component( e );
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

void Engine::move_view( const float delta ) {
    player_view_lock.lock();
    player.view_angle += delta;
    player_view_lock.unlock();
}

void Engine::set_player_move_dir( const Vec2 dir ) {
    player_move_dir_lock.lock();
    player.move_dir = dir;
    player_move_dir_lock.unlock();
}

void Engine::clear_framebuffer( const Color color ) {
    draw_rect( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, color );
}

void Engine::draw_rect( const int x, const int y, const int w, const int h, const Color color ) {
    for ( int i = 0; i < w; i++ ) {
        for ( int j = 0; j < h; j++ ) {
            const int cx = x + i;
            const int cy = y + j;
            draw_pixel( cx, cy, color );
        }
    }
}

void Engine::draw_sprite( const Entity enemy ) {
    const auto move_comp = enemy_manager.get_movement_component( enemy );
    const auto dist_comp = enemy_manager.get_distance_component( enemy );
    const auto type_comp = enemy_manager.get_enemy_type_component( enemy );

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

void Engine::draw_pixel( const int x, const int y, const Color color ) {
    framebuffer[ x + y * WINDOW_WIDTH ] = color;
}

MapTile Engine::get_map_tile( const int x, const int y ) const {
    return map[ x + y * map_width ];
}

void Engine::add_enemy( const float x, const float y, const float speed, const EnemyType type ) {
    auto id = enemy_manager.register_entity();
    if ( id.has_value() ) {
        const auto id_val = id.value();
        const auto move_comp = enemy_manager.get_movement_component( id_val );
        const auto type_comp = enemy_manager.get_enemy_type_component( id_val );

        *move_comp = { x, y, speed };
        *type_comp = { type };

        active_enemies.push_back( id.value() );
    }
}

void Engine::enemy_movement_system( const float delta_time ) {
    for ( auto& e : active_enemies ) {
        auto move_comp = enemy_manager.get_movement_component( e );
        auto dist_comp = enemy_manager.get_distance_component( e );

        // simplified raycast to check if the player can be seen by the enemy
        int x1, y1, x2, y2;
        if ( move_comp->x > player.position.x ) {
            x1 = player.position.x;
            y1 = player.position.y;
            x2 = move_comp->x;
            y2 = move_comp->y;
        } else {
            x1 = move_comp->x;
            y1 = move_comp->y;
            x2 = player.position.x;
            y2 = player.position.y;
        }

        const int dx = x2 - x1;
        const int dy = y2 - y1;

        bool can_see_player = true;
        for ( int x = x1; x < x2; ++x ) {
            int y = y1 + dy * (x - x1) / dx;
            if ( map[ x + y * map_width ] != MapTile::Floor ) {
                can_see_player = false;
                break;
            }
        }

        // TODO: magic number
        // only move if we are far enough away and can see the player
        if ( dist_comp->distance > 1.0f && can_see_player ) {
            auto dir = Vec2 {
                player.position.x - move_comp->x,
                player.position.y - move_comp->y
            };
            dir = dir.normalised();

            move_comp->x += dir.x * move_comp->speed * delta_time;
            move_comp->y += dir.y * move_comp->speed * delta_time;
        }

        // calculate distance from player
        float enemy_dist = std::sqrt( pow( player.position.x - move_comp->x, 2 ) + pow( player.position.y - move_comp->y, 2 ) );
        dist_comp->distance = enemy_dist;
    }
}
