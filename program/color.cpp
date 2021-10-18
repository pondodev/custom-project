#include "color.h"

Color::Color() {
    set_hex( 0x000000FF );
}

Color::Color( uint32_t hex ) {
    set_hex( hex );
}

Color::Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a ) {
    set_components( r, g, b, a );
}

void Color::set_hex( uint32_t hex ) {
    hex_value = hex;
}

void Color::set_components( uint8_t r, uint8_t g, uint8_t b, uint8_t a ) {
    hex_value = (r << 24) + (g << 16) + (b << 8) + a;
}

uint32_t Color::get_hex() {
    return hex_value;
}

void Color::get_components( uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a ) {
    r = (hex_value >> 24) & 0xFF;
    g = (hex_value >> 16) & 0xFF;
    b = (hex_value >> 8) & 0xFF;
    a = hex_value & 0xFF;
}
