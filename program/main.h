#ifndef MAIN_H
#define MAIN_H

#include <SFML/Graphics.hpp>
#include <thread>

#include "engine.h"
#include "vec2.h"

void logic_loop();
void input();
void update();
void draw();
void handle_key_down( sf::Keyboard::Key key );

#endif
