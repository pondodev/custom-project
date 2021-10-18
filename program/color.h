#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

class Color {
public:
    Color();
    Color( uint32_t hex );
    Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
    void set_hex( uint32_t hex );
    void set_components( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
    uint32_t get_hex();
    void get_components( uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a );

private:
    uint32_t hex_value;
};

#endif
