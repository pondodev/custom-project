#include "main.h"

Engine engine( "map.txt", "walltext.png", "monsters.png" );
sf::RenderWindow* window;
sf::Vector2i window_center;
sf::Texture render_texture;
sf::Sprite render_sprite;
sf::Clock delta_clock;
Vec2 move_dir;

int main() {
    window = new sf::RenderWindow(
        sf::VideoMode( WINDOW_WIDTH * 1.5, WINDOW_HEIGHT * 1.5 ),
        "raycaster prototype" );

    window->setMouseCursorGrabbed( true );
    window->setMouseCursorVisible( false );
    auto window_size = window->getSize();
    window_center.x = window_size.x / 2;
    window_center.y = window_size.y / 2;
    sf::Mouse::setPosition( window_center, *window );

    render_texture.create( WINDOW_WIDTH, WINDOW_HEIGHT );
    render_sprite = sf::Sprite( render_texture );
    render_sprite.setScale( 1.5, 1.5 );

    while ( window->isOpen() ) {
        input();
        update();
        draw();
    }

    delete window;

    return 0;
}

void input() {
    move_dir = Vec2 { 0.0, 0.0 };

    sf::Event event;
    while ( window->pollEvent( event ) ) {
        if ( event.type == sf::Event::Closed )
            window->close();

        if ( event.type == sf::Event::KeyPressed )
            handle_key_down( event.key.code );
    }

    // movement input
    if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::W ) )
        move_dir.y += 1.0;
    if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::S ) )
        move_dir.y += -1.0;
    if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::A ) )
        move_dir.x += 1.0;
    if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::D ) )
        move_dir.x += -1.0;

    // mouse move
    int mouse_x = sf::Mouse::getPosition( *window ).x;
    int mouse_delta = mouse_x - window_center.x;
    if ( abs(mouse_delta) > 0 ) {
        sf::Mouse::setPosition( window_center, *window );
        engine.move_view( mouse_delta * .001 ); // TODO: magic number
    }
}

void update() {
    auto delta_time = delta_clock.restart().asSeconds();
    engine.set_player_move_dir( move_dir.normalised() * 2 ); // TODO: magic number
    engine.update( delta_time );
}

void draw() {
    window->clear();

    engine.render();

    uint8_t render_buffer[ FRAMEBUFFER_LENGTH * 4 ];
    engine.get_framebuffer( render_buffer );
    render_texture.update( render_buffer );
    window->draw( render_sprite );

    window->display();
}

void handle_key_down( sf::Keyboard::Key key ) {
    switch ( key ) {
        case sf::Keyboard::Key::Escape:
            window->close();
            break;
    }
}